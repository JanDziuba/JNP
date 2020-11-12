#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <cmath>

using namespace std;

enum class LineType
{
    IncorrectLine,
    EmptyLine,
    TrafficInfo,
    Query
};

//Zawiera dystans przejechany przez samochod
//Pierwszy element odpowiada drodze expresowej, drugi autostradzie
typedef pair<int32_t, int32_t> chainages;

//Zawiera informację o obecnym węźle drogi.
typedef pair<string, uint32_t> roadInfo;

//Zawiera linię wejścia wraz z jej numerem
typedef pair<string, uint32_t> lineInfo;

//Zawiera informacje profilu samochodu
typedef tuple<chainages, roadInfo, lineInfo> Car;

//Zawiera algorytm sortowania dróg zgodny z poleceniem
class roadCmp
{
public:
    bool operator()(const string &t, const string &t2) const
    {
        if (t.size() == t2.size())
        {
            int temp = t.substr(1, t.size() - 1).compare(t2.substr(1,
                                                                   t2.size() - 1));
            if (temp < 0 || (temp == 0 && t.at(0) == 'A' && t2.at(0) == 'S'))
                return true;

            return false;
        }
        else return t.size() < t2.size();
    }
};


// maksymalny obsługiwany kilometraż to 99999999,9
LineType checkLineType(const string &line);

void removeWhitespaces(string &str);

// jeśli nie mieści się w uint32_t kończy program
uint32_t getChainageTimes10(const string &chainageString);

// zapisuje dane z lini typu TrafficInfo do zmiennych registrationNumber,
// roadName, chainageTimes10.
void parseTrafficInfo(const string &line, string &registrationNumber, string &roadName,
                      uint32_t &chainageTimes10);

void removeQuestionMarks(string &str);

// Jeśli parametr lini typu Query pasuje jako numer rejestracyjny to
// zapisuje go do registrationNumber.
// Jeśli parametr lini typu Query pasuje jako nazwa drogi to zapisuje go
// do roadName.
void parseQuery(const string &line, string &registrationNumber, string &roadName);

// Czyta linie ustala jej typ i zapisuje dane do argumentów.
// Jeśli argument nie dotyczy danej linii to dostaje wartość
// string = "", uint32_t = 0.
// maksymalny obsługiwany kilometraż to 99999999,9
void parseLine(bool &eof, uint32_t &lineNumber, LineType &lineType, string &registrationNumber,
               string &roadName, uint32_t &chainageTimes10, string &line);

//Zmienia informacje dla samochodu przejeżdzającego przez węzeł drogi.
void carProfileUpdate(map<string, Car> &cars, map<string, uint32_t, roadCmp> &roads,
                      const Car &car, const string &registrationNumber);

//Drukuje informację o błędnej lini.
void wrongLine(const lineInfo &lineInfo);

//Zwiększa całkowity dystans przebyty na drodze 'road' o dChainage.
void
roadProfileUpdate(map<string, uint32_t, roadCmp> &roads, const string &road, uint32_t dChainage);

//Drukuje informacje o podanej drodze
//Zwaraca false jeśli droga nie istnieje
bool printRoad(map<string, uint32_t, roadCmp> roads, const string &road);

//Drukuje informacje o podanym samochodzie
//Zwraca false jeśli samochód nie istnieje.
bool printCar(map<string, Car> cars, const string &car);

//Drukuje informacje o wszystkich drogach
void printRoads(map<string, uint32_t, roadCmp> roads);

//Drukuje informacje o wszystkich samochodach
void printCars(map<string, Car> cars);

chainages getCarChainages(Car car);

lineInfo getCarLineInfo(Car car);

roadInfo getCarRoadInfo(Car car);

int main()
{
    bool eof;
    uint32_t lineNumber = 0;
    LineType lineType;
    string registrationNumber;
    string roadName;
    uint32_t chainageTimes10;
    string line;
    map<string, Car> cars;
    map<string, uint32_t, roadCmp> roads;

    while (true)
    {
        parseLine(eof, lineNumber, lineType, registrationNumber, roadName,
                  chainageTimes10, line);

        if (eof)
        {
            break;
        }
        if (lineType == LineType::EmptyLine)
        {
            continue;
        }
        else if (lineType == LineType::IncorrectLine)
        {
            wrongLine(make_pair(line, lineNumber));
        }
        else if (lineType == LineType::TrafficInfo)
        {
            chainages p0 = make_pair(-1, -1);
            roadInfo p1 = make_pair(roadName, chainageTimes10);
            lineInfo p2 = make_pair(line, lineNumber);
            Car tp = make_tuple(p0, p1, p2);

            carProfileUpdate(cars, roads, tp, registrationNumber);
        }
        else if (lineType == LineType::Query)
        {
            if (registrationNumber.empty() && roadName.empty())
            {
                printCars(cars);
                printRoads(roads);
            }
            else
            {
                printCar(cars, registrationNumber);
                printRoad(roads, roadName);
            }
        }
    }
}


LineType checkLineType(const string &line)
{
    regex trafficInfoRegex(R"(\s*[A-Za-z0-9]{3,11}\s+[AS][1-9][0-9]{0,2}\s+([1-9][0-9]{0,7}|0),[0-9]\s*)");
    regex queryRegex(R"(\s*\?\s*([AS][1-9][0-9]{0,2}|[A-Za-z0-9]{3,11}|)\s*)");
    regex emptyLineRegex("");

    if (regex_match(line, trafficInfoRegex))
    {
        return LineType::TrafficInfo;
    }
    else if (regex_match(line, queryRegex))
    {
        return LineType::Query;
    }
    else if (regex_match(line, emptyLineRegex))
    {
        return LineType::EmptyLine;
    }
    else
    {
        return LineType::IncorrectLine;
    }
}

void removeWhitespaces(string &str)
{
    regex reg("\\s+");
    str = regex_replace(str, reg, "");
}

uint32_t getChainageTimes10(const string &chainageString)
{
    regex reg(",");
    string chainageStrTimes10 = regex_replace(chainageString, reg, "");

    if (chainageStrTimes10 == "00")
    {
        return 0;
    }

    unsigned long chainageTimes10 = stoul(chainageStrTimes10, nullptr, 10);
    if (chainageTimes10 > UINT32_MAX)
    {
        cerr << "string to uint32_t failure\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        return (uint32_t) chainageTimes10;
    }
}

void parseTrafficInfo(const string &line, string &registrationNumber, string &roadName,
                      uint32_t &chainageTimes10)
{
    regex registrationNumberRegex("[A-Za-z0-9]{3,11}\\s");
    regex roadNameRegex("[AS][1-9][0-9]{0,2}\\s");
    regex chainageRegex("([1-9][0-9]*|0),[0-9]");
    smatch match;

    regex_search(line, match, registrationNumberRegex);
    registrationNumber = match[0];
    removeWhitespaces(registrationNumber);

    string lineSuffix = match.suffix().str();
    regex_search(lineSuffix, match, roadNameRegex);
    roadName = match[0];
    removeWhitespaces(roadName);

    lineSuffix = match.suffix().str();
    regex_search(lineSuffix, match, chainageRegex);
    string chainageString = match[0];
    chainageTimes10 = getChainageTimes10(chainageString);
}

void removeQuestionMarks(string &str)
{
    regex reg("\\?+");
    str = regex_replace(str, reg, "");
}

void parseQuery(const string &line, string &registrationNumber, string &roadName)
{
    regex registrationNumberRegex(R"(\?\s*[A-Za-z0-9]{3,11}(\s|$))");
    regex roadNameRegex(R"(\?\s*[AS][1-9][0-9]{0,2}(\s|$))");
    smatch match;

    if (regex_search(line, match, registrationNumberRegex))
    {
        registrationNumber = match[0];
        removeWhitespaces(registrationNumber);
        removeQuestionMarks(registrationNumber);
    }

    if (regex_search(line, match, roadNameRegex))
    {
        roadName = match[0];
        removeWhitespaces(roadName);
        removeQuestionMarks(roadName);
    }
}

void parseLine(bool &eof, uint32_t &lineNumber, LineType &lineType, string &registrationNumber,
               string &roadName, uint32_t &chainageTimes10, string &line)
{
    eof = false;
    lineNumber++;
    lineType = LineType::IncorrectLine;
    registrationNumber = "";
    roadName = "";
    chainageTimes10 = 0;
    line = "";

    getline(cin, line);
    if (!cin)
    {
        if (cin.eof())
        {
            eof = true;
            return;
        }
        else
        {
            cerr << "getline failure\n";
            exit(EXIT_FAILURE);
        }
    }

    lineType = checkLineType(line);

    if (lineType == LineType::EmptyLine ||
        lineType == LineType::IncorrectLine)
    {
        return;
    }
    else if (lineType == LineType::TrafficInfo)
    {
        parseTrafficInfo(line, registrationNumber, roadName,
                         chainageTimes10);
    }
    else if (lineType == LineType::Query)
    {
        parseQuery(line, registrationNumber, roadName);
    }
}

void carProfileUpdate(map<string, Car> &cars, map<string, uint32_t, roadCmp> &roads, const Car &car,
                      const string &registrationNumber)
{
    map<string, Car>::iterator itr;
    itr = cars.find(registrationNumber);

    lineInfo lineInfo2 = getCarLineInfo(car);
    roadInfo roadInfo2 = getCarRoadInfo(car);

    if (itr == cars.end())
    {
        cars.insert({registrationNumber, car});
    }
    else
    {
        chainages chainage = getCarChainages(itr->second);
        roadInfo roadInfo = getCarRoadInfo(itr->second);
        lineInfo lineInfo = getCarLineInfo(itr->second);

        if (roadInfo.first == roadInfo2.first)
        {
            uint32_t dChainage = max(roadInfo2.second, roadInfo.second)
                                 - min(roadInfo2.second, roadInfo.second);

            if (roadInfo2.first.at(0) == 'S') 
            {
                if(chainage.first==-1) chainage.first=0;
                chainage.first += dChainage;
            }

            else 
            {
                if(chainage.second==-1) chainage.second=0;
                chainage.second += dChainage;
            }

            roadProfileUpdate(roads, roadInfo2.first, dChainage);

            itr->second = make_tuple(chainage, make_pair("", 0), lineInfo2);

        }
        else
        {
            if (!roadInfo.first.empty()) wrongLine(getCarLineInfo(itr->second));

            itr->second = make_tuple(chainage, roadInfo2, lineInfo2);
        }
    }
}

void wrongLine(const lineInfo &lineInfo)
{
    cerr << "Error in line " << lineInfo.second << ": " << lineInfo.first << endl;
}

void printCars(map<string, Car> cars)
{
    map<string, Car>::iterator itr;

    for (itr = cars.begin(); itr != cars.end(); ++itr)
    {
        chainages temp = getCarChainages(itr->second);
        if (temp.first==-1 && temp.second==-1) continue;

        cout << itr->first;

        if (temp.second!=-1) cout << " A " << temp.second / 10 << "," << temp.second % 10;
        if (temp.first!=-1) cout << " S " << temp.first / 10 << "," << temp.first % 10;

        cout << endl;
    }
}

void printRoads(map<string, uint32_t, roadCmp> roads)
{
    map<string, uint32_t, roadCmp>::iterator itr;

    for (itr = roads.begin(); itr != roads.end(); ++itr)
        cout << itr->first << " " << itr->second / 10 << "," << itr->second % 10 << endl;
}

bool printCar(map<string, Car> cars, const string &car)
{
    map<string, Car, roadCmp>::iterator itr;

    itr = cars.find(car);
    if (itr == cars.end()) return false;

    chainages temp = getCarChainages(itr->second);
    if (temp.first==-1 && temp.second==-1) return true;

    cout << itr->first;

    if (temp.second!=-1) cout << " A " << temp.second / 10 << "," << temp.second % 10;
    if (temp.first!=-1) cout << " S " << temp.first / 10 << "," << temp.first % 10;

    cout << endl;
    return true;
}

bool printRoad(map<string, uint32_t, roadCmp> roads, const string &road)
{
    map<string, uint32_t, roadCmp>::iterator itr;

    itr = roads.find(road);

    if (itr == roads.end()) return false;

    cout << itr->first << " " << itr->second / 10 << "," << itr->second % 10 << endl;
    return true;
}


void roadProfileUpdate(map<string, uint32_t, roadCmp> &roads, const string &road,
                       uint32_t dChainage)
{
    map<string, uint32_t, roadCmp>::iterator itr;

    itr = roads.find(road);

    if (itr == roads.end())
        roads.insert({road, dChainage});

    else
        itr->second += dChainage;
}

chainages getCarChainages(Car car)
{
    return get<0>(car);
}

lineInfo getCarLineInfo(Car car)
{
    return get<2>(car);
}

roadInfo getCarRoadInfo(Car car)
{
    return get<1>(car);
}
