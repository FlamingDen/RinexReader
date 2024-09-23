#ifndef RINEXNAMEGENERATOR_H
#define RINEXNAMEGENERATOR_H

#include <string>
#include <vector>

namespace Date {
    bool isDateTrue(const int day, const int month, const int year);
    int howDaysBefore(int day, int month, int year);
}


class RinexNameGenerator
{
public:
    RinexNameGenerator();

    std::string getMask() const;
    std::string getBase_directory() const;
    void setBase_directory(const std::string &newBase_derectory);
    std::string getYyyy() const;
    void setYyyy(const std::string &newYyyy);
    std::string getMm() const;
    void setMm(const std::string &newMm);
    std::string getDd() const;
    void setDd(const std::string &newDd);
    std::string getSsss() const;
    void setSsss(const std::string &newSsss);

    std::string generateUrl(std::string ssss, std::string h, std::string dd, std::string mm, std::string yyyy,
                            std::string base_directory = "ftp://sstp.geo.by");
    std::vector<std::string> generateRinexFilesNames(std::string ssss, std::string h, std::string dd, std::string mm,
                                                   std::string yyyy);

private:

    std::string mask = "base_directory/yyyy/mm/dd/ssss/ssssdddh.rnx.zip";
    std::string base_directory;//- отдельная директория оператора на FTP-сервере, вкоторой размещаются данные
    std::string yyyy;//- год по шкале времени GPS
    std::string mm;//- месяц по шкале времени GPS
    std::string dd;//- день с начала года по шкале времени GPS
    std::string ssss;//– четырехбуквенный идентификатор ПДП;
    std::string h;//- буквенный идентификатор часового блока по времени GPS
};

#endif // RINEXNAMEGENERATOR_H
