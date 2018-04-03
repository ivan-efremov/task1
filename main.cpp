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


const int         N = 4;
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


static PSegments makeSegments(const std::string a_line)
{
    PSegments segs = std::make_shared<Segments>();
    const size_t l = a_line.length() - N + 1;
    for(size_t i = 0; i < l; ++i) {
        std::string section = a_line.substr(i, N);
    #ifdef DEBUG
        std::cout << "SECTION: " << section << std::endl;
    #endif
        segs->insert(section);
    }
    return segs;
}

static PDictionary loadDictionary(PSegments a_segs)
{
    PDictionary dict = std::make_shared<Dictionary>();
    PRedisIO    redisIO = std::make_shared<RedisIO>(REDIS_HOST, REDIS_PORT);
    PRedisApi   redisApi = std::make_shared<RedisApi>();
    // ожидаем соединение к редис
    while(redisIO->error()) {
        try {
            redisIO->close();
            redisIO->connect();
        } catch(const std::exception& err) {
            std::cerr << "loadDictionary: " << err.what() << std::endl;
            sleep(1);
        }
    }
    // загружаем словарь
    RedisIO::PListValue listKeys;
    try {
        listKeys = redisIO->api(redisApi->keys("c:*"));
    } catch(const std::exception& err) {
        std::cerr << "loadDictionary: " << err.what() << std::endl;
    }

    for(auto& key : *listKeys) {
#ifdef DEBUG
        std::cout << "REDIS_KEY: " << key << " => ";
#endif
        auto itSeg = a_segs->find(key.substr(2));
        if(itSeg != a_segs->end()) {
        #ifdef DEBUG
//            std::cout << " *** FIND ***" << std::endl;
        #endif
            try {
                PSetId setId = std::make_shared<SetId>();
                RedisIO::PListValue listItems = redisIO->api(redisApi->smembers(key));
                for(auto& id : *listItems) {
                #ifdef DEBUG
                    std::cout << id << ' ';
                #endif
                    setId->insert(std::stol(id));
                }
                dict->emplace(*itSeg, setId);
            #ifdef DEBUG
                std::cout << std::endl;
            #endif
            } catch(const std::exception& err) {
                std::cerr << "loadDictionary: " << err.what() << std::endl;
                exit(-1);
            }
        } else {
#ifdef DEBUG
        std::cout << std::endl;
#endif
        }
    }
    return dict;
}

static PDesignations loadDesignations(PDictionary a_dictionary)
{
    PDesignations sign = std::make_shared<Designations>();
    PRedisIO      redisIO = std::make_shared<RedisIO>(REDIS_HOST, REDIS_PORT);
    PRedisApi     redisApi = std::make_shared<RedisApi>();
    
    return sign;
}


int main(int argc, const char *argv[])
{
    std::string line;

    // 1) Принять произвольное слово (образец) из командной строки.
    std::cout << "Enter characters: ";
    std::cin >> line;

    // 2) Разбить это слово на все возможные четырёхсимвольные отрезки
    PSegments segs = makeSegments(line);

    // 3) Загрузить из базы все обозначения, где эти отрезки встречаются.
    PDictionary dict = loadDictionary(segs);
    PDesignations sign = loadDesignations(dict);


    return 0;
}
