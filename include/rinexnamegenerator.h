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
    bool isDateTrue(const int day, const int month, const int year, const int hour = 0, const int min = 0, const int sec = 0);
    int howDaysBefore(int day, int month, int year);
}

/*
 * This class used for generate URL addres of RINEX zip file and names of Rinex files in archive.
 * Class have standarts getters and setters.
 * Fields:
 *   string mask            - mask for generation URL
 *   default_types_files    - default types are: Observer file,GPS, GLONASS, GALILEO, BEIDOU, HTM
 *   base_directory         - a separate operator directory on the FTP server in which the data is stored
 *
 * Methods:
 *      Generate the URL in the data.
 *      Patamets:
 *          # ssss - IdPDP (adout IdPDP see above)
 *          # time_t - see <chrono> and <ctime>
 *          # date_time - date and time in string format like "01/06/2023 13:04:12"
 *          # base_directory - the root of "ftp" server
 *              std::string generateUrl(IdPDP ssss, time_t curr_time_t,  std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
 *              std::string generateUrl(IdPDP ssss, std::string date_time, std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
 *              std::string generateUrl(IdPDP ssss, int h, int dd, int mm, int yyyy, std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
 *              std::string generateUrl(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy,std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
 *
 *      Generate the names of files in archive( you can change the types or leave it as default)
 *      Patamets:
 *          # ssss - IdPDP (adout IdPDP see above)
 *          # time_t - see <chrono> and <ctime>
 *          # date_time - date and time in string format like "01/06/2023 13:04:12"
 *              std::vector<std::string> generateRinexFilesNames(IdPDP ssss, time_t curr_time_t);
 *              std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string date_time);
 *              std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy);
 *
 * Examples:
 *      C++:
 *      ---------------------------------------------------------------------------------------------------------------
 *          RinexNameGenerator rng;
 *          std::string extension = "rnx.zip";
 *          std::chrono::time_point current_time = std::chrono::system_clock::now();
 *          std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);*
 *          std::cout <<  rng.generateUrl(IdPDP::BARA, "12", "24", "1", "2024", extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA, 12, 24, 1, 2024, extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA,current_time_t, extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA, "01/06/2023 13:04:12", extension) << std::endl;
 *          out << "File names: "<< endl;
 *          for(const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,current_time_t))
 *              std::cout << x << std::endl;
 *      ---------------------------------------------------------------------------------------------------------------
 *      Qt:
 *      ---------------------------------------------------------------------------------------------------------------
 *          RinexNameGenerator rng;
 *          std::string extension = "rnx.zip";
 *          QDateTime::currentSecsSinceEpoch();// new
 *          QDateTime::currentDateTimeUtc().toTime_t();// (obsolete) ver. 5.15 and below
 *          std::cout <<  rng.generateUrl(IdPDP::KLEC, std::to_string(dt.time().hour()),std::to_string( dt.date().day()), std::to_string( dt.date().month()), std::to_string( dt.date().year()), extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA, "12", "24", "1", "2024", extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA, 12, 24, 1, 2024, extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::KLEC, "24/1/2023 12:00:00", extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::KLEC, QDateTime::currentSecsSinceEpoch(), extension) << std::endl;
 *          std::cout <<  rng.generateUrl(IdPDP::BARA,QDateTime::currentSecsSinceEpoch(), extension) << std::endl;
 *          out << "File names: "<< endl;
 *          for( const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,QDateTime::currentSecsSinceEpoch()))
 *              std::cout << x << std::endl;
 *      ---------------------------------------------------------------------------------------------------------------
 */
class RinexNameGenerator
{
public:
    RinexNameGenerator();

    std::string getMask() const;
    std::vector<std::string> getDefault_types_files() const;
    void setDefault_types_files(const std::vector<std::string> &newDefault_types_files);

    std::string generateUrl(IdPDP ssss, time_t curr_time_t,  std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
    std::string generateUrl(IdPDP ssss, std::string date_time, std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
    std::string generateUrl(IdPDP ssss, unsigned int h, unsigned int dd, unsigned int mm, unsigned int yyyy, std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");
    std::string generateUrl(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy,std::string extension = "rnx.zip", std::string base_directory = "ftp://sstp.geo.by");

    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, time_t curr_time_t);
    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string date_time);
    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, unsigned int h, unsigned int dd, unsigned int mm, unsigned int yyyy);
    std::vector<std::string> generateRinexFilesNames(IdPDP ssss, std::string h, std::string dd, std::string mm, std::string yyyy);

private:
    std::string maskUrl = "base_directory/yyyy/mm/dd/ssss/ssssdddh.extension";
    std::string maskFile = "ssssdddh.yysystem";
    std::vector<std::string> default_types_files{ "f", "l", "g", "n", "o", "htm"};
    std::string base_directory = "base_directory";
    std::string yyyy = "yyyy";
    std::string mm = "mm";
    std::string dd = "dd";
    std::string ssss = "ssss";
    std::string ssssdddh = "ssssdddh";
    std::string h = "h";
    std::string extension = "extension";
};

std::string getNamePDP(IdPDP id);
std::string getPDPLowerCase(IdPDP id);
std::string getPDPUpperCase(IdPDP id);

#endif // RINEXNAMEGENERATOR_H
