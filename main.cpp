/**
 * @file main.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "redis/RedisIO.h"


const std::string REDIS_HOST = "127.0.0.1";
const std::string REDIS_PORT = "6379";

// сегменты - четырёхсимвольные отрезки: { ABCD, EFGH, ...}
typedef std::set<std::string>           Segments;
typedef std::shared_ptr<Segments>       PSegments;

// словарь сегментов: ABCD = [1,2,3,4]
typedef std::set<size_t>                SetId;
typedef std::shared_ptr<SetId>          PSetId;
typedef std::map<std::string, PSetId>   Dictionary;
typedef std::shared_ptr<Dictionary>     PDictionary;

// обозначения: 1|ABCDEFGH
typedef std::map<size_t, std::string>   Designations;
typedef std::shared_ptr<Designations>   PDesignations;

// результат сравнения с образцом
typedef std::map<size_t, PSegments>     CompareResult;
typedef std::shared_ptr<CompareResult>  PCompareResult;


static PSegments makeSegments(const std::string a_line, int a_n)
{
    PSegments segs = std::make_shared<Segments>();
    const size_t l = a_line.length() - a_n + 1;
    for(size_t i = 0; i < l; ++i) {
        std::string segment = a_line.substr(i, a_n);
    #ifdef DEBUG
        std::cout << "SEGMENT: " << segment << std::endl;
    #endif
        segs->insert(segment);
    }
    return segs;
}

static PDictionary loadDictionary(PSegments a_segs)
{
    PDictionary dict = std::make_shared<Dictionary>();
    PRedisIO    redisIO = std::make_shared<RedisIO>(REDIS_HOST, REDIS_PORT);
    PRedisApi   redisApi = std::make_shared<RedisApi>();
    // формируем интересующий нас словарь
    for(auto& seg : *a_segs) {
    #ifdef DEBUG
        std::cout << "REDIS_KEY: " << seg << " => ";
    #endif
        try {
            PSetId setId = std::make_shared<SetId>();
            RedisIO::PListValue listItems = redisIO->api(
                redisApi->smembers(std::string("c:") + seg)
            );
            for(auto& id : *listItems) {
            #ifdef DEBUG
                std::cout << id << ' ';
            #endif
                setId->insert(std::stol(id));
            }
            if(!dict->emplace(seg, setId).second) {
                throw std::runtime_error("Segment already exist: " + seg);
            }
        #ifdef DEBUG
            std::cout << std::endl;
        #endif
        } catch(const std::exception& err) {
            std::cerr << "loadDictionary: " << err.what() << std::endl;
            exit(-2);
        }
    }
    return dict;
}

static PDesignations loadDesignations(PDictionary a_dict)
{
    PDesignations sign = std::make_shared<Designations>();
    PRedisIO      redisIO = std::make_shared<RedisIO>(REDIS_HOST, REDIS_PORT);
    PRedisApi     redisApi = std::make_shared<RedisApi>();
    for(auto& itDict : *a_dict) {
        PSetId setId = itDict.second;
        for(auto& id : *setId) {
            try {
                RedisIO::PListValue val = redisIO->api(
                    redisApi->get(std::string("t:") + std::to_string(id))
                );
                if(!val->empty()) {
                    std::string sval(val->front());
                    sval = sval.substr(sval.find('|') + 1);
                #ifdef DEBUG
                    std::cout << "SIGN: " << id << " => " << sval << std::endl;
                #endif
                    sign->emplace(id, sval);
                }
            } catch(const std::exception& err) {
                std::cerr << "loadDesignations: " << err.what() << std::endl;
                exit(-1);
            }
        }
    }
    return sign;
}

static PCompareResult compareWithSample(const std::string& a_sample, PDesignations a_sign)
{
    PCompareResult compareResult = std::make_shared<CompareResult>();
    PSegments segsSample = makeSegments(a_sample, 3);
    for(auto& itSign : *a_sign) {
        size_t id = itSign.first;
        std::string word = itSign.second;
        PSegments segsWord = makeSegments(word, 3);
        for(auto& segS :  *segsSample) {
            PSegments findSeg = std::make_shared<Segments>();
            for(auto& segW : *segsWord) {
                if(segS == segW) {
                #ifdef DEBUG
                    std::cout << "COMPARE: " << segS << std::endl;
                #endif
                    findSeg->insert(segS);
                }
            }
            if(!findSeg->empty()) {
                auto res = compareResult->emplace(id, findSeg);
                if(!res.second) {
                    res.first->second->insert(findSeg->begin(), findSeg->end());
                }
            }
        }
    }
    return compareResult;
}

static void printResult(PCompareResult a_compareResult)
{
    std::cout << "Result:" << std::endl;
    for(auto& itResult : *a_compareResult) {
        size_t id = itResult.first;
        PSegments cmpSeg = itResult.second;
        std::cout << "\t" << id << " : " << cmpSeg->size() << std::endl;
    }
}


int main(int argc, const char *argv[])
{
    std::string line;

    // 1) Принять произвольное слово (образец) из командной строки.
    std::cout << "Enter characters: ";
    std::cin >> line;

    // 2) Разбить это слово на все возможные четырёхсимвольные отрезки
    PSegments segs = makeSegments(line, 4);

    // 3) Загрузить из базы все обозначения, где эти отрезки встречаются.
    PDictionary dict = loadDictionary(segs);
    PDesignations sign = loadDesignations(dict);

    // 4) Сравнить каждое обозначение с образцом
    PCompareResult compareResult = compareWithSample(line, sign);

    // 5) Построчно вывести в консоль ID загруженных знаков и количество совпадений
    printResult(compareResult);

    return 0;
}
