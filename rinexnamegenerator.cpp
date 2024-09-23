#include "rinexnamegenerator.h"

#define     default_year    2015
#define     default_month   1
#define     default_day     1
#define     default_hour    0
#define     default_min     0
#define     default_sec     0

using namespace std;

RinexNameGenerator::RinexNameGenerator() {}


std::string RinexNameGenerator::getBase_directory() const{ return base_directory;}

void RinexNameGenerator::setBase_directory(const std::string &newBase_directory){ base_directory = newBase_directory;}

std::string RinexNameGenerator::getYyyy() const{ return yyyy;}

void RinexNameGenerator::setYyyy(const std::string &newYyyy){ yyyy = newYyyy;}

std::string RinexNameGenerator::getMm() const{ return mm;}

void RinexNameGenerator::setMm(const std::string &newMm){ mm = newMm;}

std::string RinexNameGenerator::getDd() const{ return dd;}

void RinexNameGenerator::setDd(const std::string &newDd){ dd = newDd;}
std::string RinexNameGenerator::getSsss() const{ return ssss;}

void RinexNameGenerator::setSsss(const std::string &newSsss) {ssss = newSsss;}


void replaceWord(string& str, string oldWord, string newWord){
    size_t pos = str.find(oldWord);
    if (pos != string::npos)
        str.replace(pos, oldWord.length(), newWord);
}

string genSsssdddh(string ssss, string ddd, string h){
    return ssss + ddd + h;
}

string RinexNameGenerator::generateUrl(std::string ssss, std::string h, std::string dd, std::string mm, std::string yyyy,
                                       std::string base_directory)
{
    if (Date::isDateTrue(stoi(dd), stoi(mm), stoi(yyyy))){
        string url = mask;
        replaceWord(url, "base_directory", base_directory);
        replaceWord(url, "yyyy", yyyy);
        mm = (mm.size() == 2 ? mm : mm.insert(0,"0"));
        dd = (dd.size() == 2 ? dd : dd.insert(0,"0"));
        replaceWord(url, "mm", mm);
        replaceWord(url, "dd", dd);
        replaceWord(url, "ssss", ssss);
        string days_before = std::to_string(Date::howDaysBefore(stoi(dd), stoi(mm), stoi(yyyy)));
        if (days_before.size() == 1)
            days_before.insert(0,"00");
        if (days_before.size() == 2)
            days_before.insert(0,"0");
        replaceWord(url, "ssssdddh", genSsssdddh(ssss, days_before, h));
        return url;
    }
    return {};
}

std::vector<string> RinexNameGenerator::generateRinexFilesNames(std::string ssss, std::string h, std::string dd, std::string mm, std::string yyyy)
{
    string maskName = mask.substr(31, 9) + "yy" + "system";
    vector<string> sys{ "f", "l", "g", "n", "o", "htm"};
    vector<string> names;
    string yy = yyyy.size() > 2 ? yyyy.substr(2, 2) : yyyy;
    for (size_t i(0); i < sys.size() ; i++){
        string name = maskName;
        replaceWord(name, "ssssdddh", genSsssdddh(ssss, std::to_string(Date::howDaysBefore(stoi(dd), stoi(mm), stoi(yyyy))), h));
        replaceWord(name, "yy", yy);
        replaceWord(name, "system", sys[i]);
        names.push_back(name);
    }
    return names;
}



bool Date::isDateTrue(const int day, const int month, const int year)
{
    if (day > 31 || !day || month > 12 || !month || year < default_year)
        return false;
    int n(year % 4 ? 28 : 29);
    switch (month) {
    case 2:
        if (day > n)
            return false;
    case 4:
    case 6:
    case 9:
    case 11:
        if (day > 30)
            return false;
    default:
        if (day > 31)
            return false;
    }
    return true;
}

int Date::howDaysBefore(int day, int month, int year)
{
    day = abs(day); month = abs(month); year = abs(year);
    if (Date::isDateTrue(day, month, year)) {
        int dif(0);
        //year in day
        for (int i(year); i <= year; i++)
            dif += i % 4 ? 365 : 366;
        //mounth in day
        for (int i(year); i >= year; i--) {
            int n(year % 4 ? 28 : 29), m(1);
            if (i == year)
                m = month;
            for (int j(12); j >= m; j--) {
                switch (j) {
                case 2:
                    if (j == month && i == year)
                        dif -= (n - day);
                    else
                        dif -= n;
                    continue;
                case 4:
                case 6:
                case 9:
                case 11:
                    if (j == month && i == year)
                        dif -= (30 - day);
                    else
                        dif -= 30;
                    continue;
                default:
                    if (j == month && i == year)
                        dif -= (31 - day);
                    else
                        dif -= 31;
                    continue;
                }
            }
        }
        return dif-1;
    }
    return EOF;
}
