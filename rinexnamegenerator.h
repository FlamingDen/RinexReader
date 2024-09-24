#ifndef RINEXNAMEGENERATOR_H
#define RINEXNAMEGENERATOR_H

#include <string>
#include <vector>



/*
 * The enum class, witch contains the identifier of permanent point
 * Examples:
 *      Барановичи - BARA
 *      Большая Берестовица - BBER
 *      Белыничи - BELN
 * Methods:
 *      string getNamePDP(IdPDP id)            - return the original name(Барановичи)
 *      string getPDPLowerCase(IdPDP id)       - return the ID in lower case (BARA)
 *      string getPDPUpperCase(IdPDP id)       - return the ID in upper case (bara)
 */
enum IdPDP
{
    BARA,
    BBER,
    BELN,
    BERZ,
    BESH,
    BERE,
    BOBR,
    BORI,
    BRAS,
    BRTS,
    BUDA,
    BYHO,
    VERH,
    VILE,
    VITR,
    VORO,
    VOLO,
    VYSO,
    GANC,
    GLUB,
    GOML,
    GORK,
    GORO,
    GROD,
    DOBR,
    DVSK,
    DROG,
    DKSH,
    DUBR,
    DYAT,
    ELSK,
    ZELV,
    ZHIT,
    ZHLO,
    IVAC,
    IVYE,
    KLNK,
    KAME,
    KLEC,
    KLCH,
    KOBN,
    KOMR,
    KOST,
    KRCH,
    KRUP,
    LEPE,
    LELC,
    LIDA,
    LIOZ,
    LOEV,
    LUNI,
    LJUB,
    MRIT,
    MRGO,
    MNKW,
    MOGI,
    MSTS,
    MJAD,
    NOVO,
    NOVP,
    NLUK,
    OBOL,
    OKTB,
    ORSH,
    OSIP,
    OSTR,
    PINS,
    PLES,
    PSTV,
    PRUZ,
    PTRV,
    RADO,
    RECH,
    ROSS,
    SVET,
    SENN,
    SKDL,
    SLAV,
    SLON,
    SLUC,
    SOKO,
    SMOR,
    STDO,
    STLN,
    STOL,
    STRB,
    TELE,
    TOLO,
    UZDA,
    USHA,
    SHAR,
    SHKL,
    SHCH,
    FANI,
    HOYN,
    HONV,
    CHAU,
    CHEC
};

/*
 * Namespace Date constaints the 2 methods.
 * This namespace helped to work with date&time for generation URL and name of files.
 * Methods:
 *      bool isDateTrue(const int day, const int month, const int year) - checked the correctness of the entered date
 *      int howDaysBefore(int day, int month, int year)                 - calculate how many day have passed since the beginning of the year
 */
namespace Date {
    bool isDateTrue(const int day, const int month, const int year);
    int howDaysBefore(int day, int month, int year);
}

/*
 * This class used for generate URL addres of RINEX zip file and names of Rinex files in archive.
 * Class have standarts getters and setters.
 * Fields:
 *   string mask            - mask for generation URL
 *   default_types_files    - default types are: Observer file,GPS, GLONASS, GALILEO, BEIDOU, HTM
 *   base_directory         - a separate operator directory on the FTP server in which the data is stored
 *   yyyy                   - year according to GPS time scale
 *   mm                     - month according to GPS time scale
 *   dd                     - day since the beginning of the year according to the GPS time scale
 *   ssss                   - four-letter RDP identifier
 *   h                      - letter identifier of the hour block according to GPS time
 * Methods:
 *      Generate the URL in the data.
 *      Patamets:
 *          # ssss - IdPDP (adout IdPDP see above)
 *          # time_t - see <chrono> and <ctime>
 *          # base_directory - the root of "ftp" server
 *      std::string generateUrl(IdPDP ssss, time_t curr_time_t,
 *                           std::string base_directory = "ftp://sstp.geo.by");
 *      std::string generateUrl(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy,
 *                           std::string base_directory = "ftp://sstp.geo.by");
 *      Generate the names of files in archive( you cfv change the types or leave it as default)
 *      Patamets:
 *          #  ssss - IdPDP (adout IdPDP see above)
 *          #  time_t - see <chrono> and <ctime>
 *          std::vector<std::string> generateRinexFilesNames(IdPDP ssss, time_t curr_time_t);
 *          std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy);
 * Example:
 *      C++:
 *      ---------------------------------------------------------------------------------------------------------------
 *          RinexNameGenerator rng;
 *          std::chrono::time_point current_time = std::chrono::system_clock::now();
 *          std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);*
 *          std::cout <<  rng.generateUrl(IdPDP::BARA,"a","24", "1", "2024") << std::endl;
 *          out << "File names: "<< endl;
 *          for(const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,"a","1", "5", "2024"))
 *              std::cout << x << std::endl;
 *
 *          std::cout <<  rng.generateUrl(IdPDP::BARA,current_time_t) << std::endl;
 *          out << "File names: "<< endl;
 *          for(const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,current_time_t))
 *              std::cout << x << std::endl;
 *      ---------------------------------------------------------------------------------------------------------------
 *      Qt:
 *      ---------------------------------------------------------------------------------------------------------------
 *          QDateTime::currentSecsSinceEpoch();// new
 *          QDateTime::currentDateTimeUtc().toTime_t();// obsolete ver. 5.15 and below
 *          std::cout <<  rng.generateUrl(IdPDP::BARA,QDateTime::currentSecsSinceEpoch()) << std::endl;
 *          out << "File names: "<< endl;
 *          for(const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,QDateTime::currentSecsSinceEpoch()))
 *              std::cout << x << std::endl;
 *      ---------------------------------------------------------------------------------------------------------------
 */
class RinexNameGenerator
{
public:
    RinexNameGenerator();

    std::string getMask() const;
    std::string getBase_directory() const;
    std::string getYyyy() const;
    void setYyyy(const std::string &newYyyy);
    std::string getMm() const;
    void setMm(const std::string &newMm);
    std::string getDd() const;
    void setDd(const std::string &newDd);
    std::string getSsss() const;
    void setSsss(const std::string &newSsss);
    std::vector<std::string> getDefault_types_files() const;
    void setDefault_types_files(const std::vector<std::string> &newDefault_types_files);

    std::string generateUrl(IdPDP ssss, time_t curr_time_t, std::string base_directory = "ftp://sstp.geo.by");
    std::string generateUrl(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy, std::string base_directory = "ftp://sstp.geo.by");

    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, time_t curr_time_t);
    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy);

private:
    std::string mask = "base_directory/yyyy/mm/dd/ssss/ssssdddh.rnx.zip";
    std::vector<std::string> default_types_files{ "f", "l", "g", "n", "o", "htm"};
    std::string base_directory;
    std::string yyyy;
    std::string mm;
    std::string dd;
    std::string ssss;
    std::string h;
};

std::string getNamePDP(IdPDP id);
std::string getPDPLowerCase(IdPDP id);
std::string getPDPUpperCase(IdPDP id);

#endif // RINEXNAMEGENERATOR_H
