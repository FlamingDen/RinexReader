#include <QCoreApplication>
#include <QtSql>
#include <QTextStream>

#include "rinexnamegenerator.h"
#include "rinexreader.h"
#include "pdp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    using Qt::endl;
    out << QCoreApplication::applicationDirPath() << endl;


    //==========================Declare=============================================
    QString pathObs("D:/QtProjects/RinexReaderLib/core/tests/file_for_samples/goml299o.23o");
    QString navGPS("D:/QtProjects/RinexReaderLib/core/tests/file_for_samples/goml299o.23n");
    QString navGLO("D:/QtProjects/RinexReaderLib/core/tests/file_for_samples/goml299o.23g");
    QString navGAL("D:/QtProjects/RinexReaderLib/core/tests/file_for_samples/goml299o.23l");
    QString navBEI("D:/QtProjects/RinexReaderLib/core/tests/file_for_samples/goml299o.23f");

    QStringList navPaths{navGPS, navGLO, navGAL, navBEI};

    QString pathCSVobs = "D:/QtProjects/RinexReaderLib/test/file_for_samples/infEpochs.csv";
    QString pathCSVnav = "D:/QtProjects/RinexReaderLib/test/file_for_samples/infNavs.csv";
    //==============================================================================

    //=============================RinexReader API(sample)==========================

    rr::RinexReader rr(pathObs, navPaths);
    rr::Rinex3Obs::ObsHeaderInfo headObs = rr.getObsHeaderInfo();

    // QList<Rinex3Obs::ObsEpochInfo> inf = rr.getEpochs();
    rr::ViewNav viewNav;

    rr.nextNav();
    viewNav = rr.getNav();
    rr.nextNav();
    viewNav = rr.getNav();
    rr.nextNav();
    viewNav = rr.getNav();
    rr.clearNavData();
    rr.nextNav();
    viewNav = rr.getNav();

    rr::RinexNameGenerator qwe;
    qwe.generateUrl(IdPDP::KLEC, "24/1/2023 12:00:00");

    // rr.saveAsCSV(pathCSVobs, RinexType::OBSERVATION);
    // rr.saveAsCSV(pathCSVnav, RinexType::NAVIGATION);

    rr::Pdp pdp(1, "Барановичи", "BARA", {25.973639, 53.083056, 0});
    std::cout << pdp;
    //==============================================================================
}
