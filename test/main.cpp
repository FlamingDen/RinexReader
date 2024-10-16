#include <QCoreApplication>
#include <QtSql>
#include <QTextStream>

#include "rinexreader.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    using Qt::endl;
    out << QCoreApplication::applicationDirPath() << endl;


    //==========================Declare=============================================
    QString pathObs("C:/Utils/RinexSample/goml299o.23o");
    QString navGPS("C:/Utils/RinexSample/goml299o.23n");
    QString navGLO("C:/Utils/RinexSample/goml299o.23g");
    QString navGAL("C:/Utils/RinexSample/goml299o.23l");
    QString navBEI("C:/Utils/RinexSample/goml299o.23f");

    QStringList navPaths{navGPS, navGLO, navGAL, navBEI};

    QString pathCSVobs = "C:/Utils/RinexSample/infEpochs.csv";
    QString pathCSVnav = "C:/Utils/RinexSample/infNavs.csv";
    //==============================================================================


    //=============================RinexReader API(sample)==========================

    RinexReader rr(pathObs,navPaths);
    Rinex3Obs::ObsHeaderInfo headObs = rr.getObsHeaderInfo();

    //QList<Rinex3Obs::ObsEpochInfo> inf = rr.getEpochs();
    ViewNav viewNav;

    rr.nextNav();
    viewNav= rr.getNav();
    rr.nextNav();
    viewNav= rr.getNav();
    rr.nextNav();
    viewNav = rr.getNav();
    rr.clearNavData();
    rr.nextNav();
    viewNav = rr.getNav();

    //rr.saveAsCSV(pathCSVobs, RinexType::OBSERVATION);
    //rr.saveAsCSV(pathCSVnav, RinexType::NAVIGATION);
    //==============================================================================
}
