#include "rinexnamegenerator.h"

#include <algorithm>
#include <chrono>
#include <regex>

rr::RinexNameGenerator::RinexNameGenerator() {}


//-----------------------------------------------------------------------------------------------------------------------
std::vector<std::string> rr::RinexNameGenerator::getDefault_types_files() const
{
    return default_types_files;
}

void rr::RinexNameGenerator::setDefault_types_files(const std::vector<std::string> &newDefault_types_files)
{
    default_types_files = newDefault_types_files;
}
//-----------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------
void replaceWord(std::string &str, std::string oldWord, std::string newWord) {
    size_t pos = str.find(oldWord);
    if (pos != std::string::npos)
        str.replace(pos, oldWord.length(), newWord);
}

char getCharByHours(std::string h){
    return std::stoi(h) + 97;
}

int getTimeByChar(char t){
    return t - 97;
}

std::string genSsssdddh(std::string ssss, std::string ddd, std::string h){
    return ssss + ddd + getCharByHours(h);
}

std::vector<std::string> getStrDate(const std::string Date) {
    std::regex rx("^(\\d{1,2})[-./](\\d{1,2})[-./](\\d{2}|\\d{4})\\s+(\\d{1,2}):(\\d{1,2}):(\\d{1,2})$");
    std::smatch match;
    if(!(std::regex_match(Date, match, rx) && (match.size() == 7)))
        throw std::runtime_error("Invalide input date");
    std::vector<std::string> date;
    for (size_t i(1); i < match.size(); i++ )
        date.push_back(match.str(i));
    return date;
}

std::string rr::RinexNameGenerator::generateUrl(IdPDP ssss, std::time_t curr_time_t, std::string extension, std::string base_directory)
{
    struct tm* now = localtime(&curr_time_t);
    return generateUrl(ssss, std::to_string(now->tm_hour),std::to_string( now->tm_mday), std::to_string(now->tm_mon), std::to_string( now->tm_year + 1900), extension, base_directory);
}

std::string rr::RinexNameGenerator::generateUrl(IdPDP ssss, std::string date_time, std::string extension, std::string base_directory)
{
    std::vector<std::string> date = getStrDate(date_time);
    return generateUrl(ssss, date.at(3), date.at(0), date.at(1), date.at(2), extension, base_directory);
}

std::string rr::RinexNameGenerator::generateUrl(IdPDP ssss, unsigned int h, unsigned int dd, unsigned int mm, unsigned int yyyy, std::string extension, std::string base_directory)
{
    return generateUrl(ssss, std::to_string(h), std::to_string(dd), std::to_string(mm), std::to_string(yyyy), extension, base_directory);
}

std::string rr::RinexNameGenerator::generateUrl(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy,  std::string extension, std::string base_directory)
{
    if (Date::isDateTrue(stoi(dd), stoi(mm), stoi(yyyy))){
        std::string url(maskUrl);
        replaceWord(url, this->base_directory, base_directory);
        replaceWord(url, this->yyyy, yyyy);
        mm = (mm.size() == 2 ? mm : mm.insert(0,"0"));
        dd = (dd.size() == 2 ? dd : dd.insert(0,"0"));
        replaceWord(url, this->mm, mm);
        replaceWord(url, this->dd, dd);
        replaceWord(url, this->ssss, getPDPUpperCase(ssss));
        std::string days_before = std::to_string(Date::howDaysBefore(stoi(dd), stoi(mm), stoi(yyyy)));
        if (days_before.size() == 1)
            days_before.insert(0,"00");
        if (days_before.size() == 2)
            days_before.insert(0,"0");
        replaceWord(url, this->ssssdddh, genSsssdddh(getPDPLowerCase(ssss), days_before, h));
        replaceWord(url, this->extension, extension);
        return url;
    }
    return {};
}

std::vector<std::string> rr::RinexNameGenerator::generateRinexFilesNames(IdPDP ssss, std::time_t curr_time_t)
{
    struct tm* now = localtime(&curr_time_t);
    return generateRinexFilesNames(ssss, std::to_string(now->tm_hour),std::to_string( now->tm_mday), std::to_string(now->tm_mon), std::to_string( now->tm_year + 1900));
}

std::vector<std::string> rr::RinexNameGenerator::generateRinexFilesNames(IdPDP ssss, std::string date_time)
{
    std::vector<std::string> date = getStrDate(date_time);
    return generateRinexFilesNames(ssss, date.at(3), date.at(0), date.at(1), date.at(2));
}

std::vector<std::string> rr::RinexNameGenerator::generateRinexFilesNames(IdPDP ssss, unsigned int h, unsigned int dd, unsigned int mm, unsigned int yyyy)
{
    return generateRinexFilesNames(ssss, std::to_string(h), std::to_string(dd), std::to_string(mm), std::to_string(yyyy));
}

std::vector<std::string> rr::RinexNameGenerator::generateRinexFilesNames(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy)
{
    if (!Date::isDateTrue(stoi(dd), stoi(mm), stoi(yyyy)))
        return {};
    std::vector<std::string> names;
    std::string yy = yyyy.size() > 2 ? yyyy.substr(2, 2) : yyyy;
    for (size_t i(0); i < this->default_types_files.size() ; i++){
        std::string name(maskFile);
        replaceWord(name, this->ssssdddh, genSsssdddh(getPDPLowerCase(ssss), std::to_string(Date::howDaysBefore(stoi(dd), stoi(mm), stoi(yyyy))), h));
        replaceWord(name, "yy", yy);
        replaceWord(name, "system", default_types_files[i]);
        names.push_back(name);
    }
    return names;
}
//-----------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------
bool Date::isDateTrue(const int day, const int month, const int year, const int hour, const int min, const int sec)
{
    if (day > 31 || day < 1 || !day || month > 12 || month < 1 || !month || hour > 23 || hour < 0 || min > 59 || min < 0 || sec > 59 || sec < 0)
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
//-----------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------
std::string getNamePDP(IdPDP id)
{
    switch (id) {
    case 0: return "Барановичи"; break;
    case 1: return "Большая Берестовица"; break;
    case 2: return "Белыничи"; break;
    case 3: return "Береза"; break;
    case 4: return "Бешенковичи"; break;
    case 5: return "Березино"; break;
    case 6: return "Бобруйск"; break;
    case 7: return "Борисов"; break;
    case 8: return "Браслав"; break;
    case 9: return "Брест"; break;
    case 10: return "Буда-Кошелево"; break;
    case 11: return "Быхов"; break;
    case 12: return "Верхнедвинск"; break;
    case 13: return "Вилейка"; break;
    case 14: return "Витебск"; break;
    case 15: return "Вороново"; break;
    case 16: return "Воложин"; break;
    case 17: return "Высокое"; break;
    case 18: return "Ганцевичи"; break;
    case 19: return "Глубокое"; break;
    case 20: return "Гомель"; break;
    case 21: return "Горки"; break;
    case 22: return "Городок"; break;
    case 23: return "Гродно"; break;
    case 24: return "Добруш"; break;
    case 25: return "Довск"; break;
    case 26: return "Дрогичин"; break;
    case 27: return "Докшицы"; break;
    case 28: return "Дубровно"; break;
    case 29: return "Дятлово"; break;
    case 30: return "Ельск"; break;
    case 31: return "Зельва"; break;
    case 32: return "Житковичи"; break;
    case 33: return "Жлобин"; break;
    case 34: return "Ивацевичи"; break;
    case 35: return "Ивье"; break;
    case 36: return "Калинковичи"; break;
    case 37: return "Каменец"; break;

    case 38: return "Клецк"; break;
    case 39: return "Кличев"; break;
    case 40: return "Кобрин"; break;
    case 41: return "Комaрово"; break;
    case 42: return "Костюковичи"; break;
    case 43: return "Кричев"; break;
    case 44: return "Крупки"; break;
    case 45: return "Лепель"; break;
    case 46: return "Лельчицы"; break;
    case 47: return "Лида"; break;
    case 48: return "Лиозно"; break;
    case 49: return "Лоев"; break;
    case 50: return "Лунинец"; break;
    case 51: return "Любань"; break;
    case 52: return "Малорита"; break;
    case 53: return "Марьина Горка"; break;
    case 54: return "Минск"; break;
    case 55: return "Могилев"; break;
    case 56: return "Мстиславль"; break;
    case 57: return "Мядель"; break;
    case 58: return "Новогрудок"; break;
    case 59: return "Новополоцк"; break;
    case 60: return "Новолукомль"; break;
    case 61: return "Оболь"; break;
    case 62: return "Октябрьский"; break;
    case 63: return "Орша"; break;
    case 64: return "Осиповичи"; break;
    case 65: return "Островец"; break;
    case 66: return "Пинск"; break;
    case 67: return "Плещеницы"; break;
    case 68: return "Поставы"; break;
    case 69: return "Пружаны"; break;
    case 70: return "Петриков"; break;
    case 71: return "Радошковичи"; break;
    case 72: return "Речица"; break;
    case 73: return "Россоны"; break;
    case 74: return "Светлогорск"; break;
    case 75: return "Сенно"; break;
    case 76: return "Скидель"; break;
    case 77: return "Славгород"; break;
    case 78: return "Слоним"; break;
    case 79: return "Слуцк"; break;

    case 80: return "Сокол"; break;
    case 81: return "Сморгонь"; break;
    case 82: return "Старые Дороги"; break;
    case 83: return "Столин"; break;
    case 84: return "Столбцы"; break;
    case 85: return "Старобин"; break;
    case 86: return "Телеханы"; break;
    case 87: return "Толочин"; break;
    case 88: return "Узда"; break;
    case 89: return "Ушачи"; break;
    case 90: return "Шарковщина"; break;
    case 91: return "Шклов"; break;
    case 92: return "Щучин"; break;
    case 93: return "Фаниполь"; break;
    case 94: return "Хойники"; break;
    case 95: return "Хоневичи"; break;
    case 96: return "Чаусы"; break;
    case 97: return "Чечерск"; break;
    default:
        break;
    }
    return {};
}

std::string getPDPUpperCase(IdPDP id)
{
    switch (id) {
    case 0: return "BARA"; break;
    case 1: return "BBER"; break;
    case 2: return "BELN"; break;
    case 3: return "BERZ"; break;
    case 4: return "BESH"; break;
    case 5: return "BERE"; break;
    case 6: return "BOBR"; break;
    case 7: return "BORI"; break;
    case 8: return "BRAS"; break;
    case 9: return "BRTS"; break;
    case 10: return "BUDA"; break;
    case 11: return "BYHO"; break;
    case 12: return "VERH"; break;
    case 13: return "VILE"; break;
    case 14: return "VITR"; break;
    case 15: return "VORO"; break;
    case 16: return "VOLO"; break;
    case 17: return "VYSO"; break;
    case 18: return "GANC"; break;
    case 19: return "GLUB"; break;
    case 20: return "GOML"; break;
    case 21: return "GORK"; break;
    case 22: return "GORO"; break;
    case 23: return "GROD"; break;
    case 24: return "DOBR"; break;
    case 25: return "DVSK"; break;
    case 26: return "DROG"; break;
    case 27: return "DKSH"; break;
    case 28: return "DUBR"; break;
    case 29: return "DYAT"; break;
    case 30: return "ELSK"; break;
    case 31: return "ZELV"; break;
    case 32: return "ZHIT"; break;
    case 33: return "ZHLO"; break;
    case 34: return "IVAC"; break;
    case 35: return "IVYE"; break;
    case 36: return "KLNK"; break;
    case 37: return "KAME"; break;
    case 38: return "KLEC"; break;
    case 39: return "KLCH"; break;
    case 40: return "KOBN"; break;
    case 41: return "KOMR"; break;
    case 42: return "KOST"; break;
    case 43: return "KRCH"; break;
    case 44: return "KRUP"; break;
    case 45: return "LEPE"; break;
    case 46: return "LELC"; break;
    case 47: return "LIDA"; break;
    case 48: return "LIOZ"; break;
    case 49: return "LOEV"; break;
    case 50: return "LUNI"; break;
    case 51: return "LJUB"; break;
    case 52: return "MRIT"; break;
    case 53: return "MRGO"; break;
    case 54: return "MNKW"; break;
    case 55: return "MOGI"; break;
    case 56: return "MSTS"; break;
    case 57: return "MJAD"; break;
    case 58: return "NOVO"; break;
    case 59: return "NOVP"; break;
    case 60: return "NLUK"; break;
    case 61: return "OBOL"; break;
    case 62: return "OKTB"; break;
    case 63: return "ORSH"; break;
    case 64: return "OSIP"; break;
    case 65: return "OSTR"; break;
    case 66: return "PINS"; break;
    case 67: return "PLES"; break;
    case 68: return "PSTV"; break;
    case 69: return "PRUZ"; break;
    case 70: return "PTRV"; break;
    case 71: return "RADO"; break;
    case 72: return "RECH"; break;
    case 73: return "ROSS"; break;
    case 74: return "SVET"; break;
    case 75: return "SENN"; break;
    case 76: return "SKDL"; break;
    case 77: return "SLAV"; break;
    case 78: return "SLON"; break;
    case 79: return "SLUC"; break;
    case 80: return "SOKO"; break;
    case 81: return "SMOR"; break;
    case 82: return "STDO"; break;
    case 83: return "STLN"; break;
    case 84: return "STOL"; break;
    case 85: return "STRB"; break;
    case 86: return "TELE"; break;
    case 87: return "TOLO"; break;
    case 88: return "UZDA"; break;
    case 89: return "USHA"; break;
    case 90: return "SHAR"; break;
    case 91: return "SHKL"; break;
    case 92: return "SHCH"; break;
    case 93: return "FANI"; break;
    case 94: return "HOYN"; break;
    case 95: return "HONV"; break;
    case 96: return "CHAU"; break;
    case 97: return "CHEC"; break;
    default: break;
    }
    return {};
}

std::string getPDPLowerCase(IdPDP id)
{
    std::string ssss  = getPDPUpperCase(id);
    transform(ssss.begin(), ssss.end(), ssss.begin(),tolower);
    return ssss;
}


//-----------------------------------------------------------------------------------------------------------------------
