/**
 * @file main.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>


const int N = 4;


typedef std::vector<std::string>  Segments;
typedef std::shared_ptr<Segments> PSegments;



typedef std::set<long>                  SetId;
typedef std::shared_ptr<SetId>          PSetId;
typedef std::map<std::string, PSetId >  Dictionary;
typedef std::shared_ptr<Segments>       PDictionary;




static PSegments makeSegments(const std::string a_line)
{
    PSegments segs = std::make_shared<Segments>();
    const size_t l = a_line.length() - N + 1;
    for(size_t i = 0; i < l; ++i) {
        std::string section = a_line.substr(i, N);
    #ifdef DEBUG
        std::cout << "SECTION: " << section << std::endl;
    #endif
        segs->push_back(section);
    }
    return segs;
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
    


    return 0;
}
