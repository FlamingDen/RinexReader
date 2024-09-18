#include <QCoreApplication>
#include <QtSql>
#include <QTextStream>

#include "facadedb.h"
#include "rinexreader.h"

void uploadDatatoDB(FacadeDB* db, RinexReader& rr);
void testFacadeDB();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    using Qt::endl;
    out << QCoreApplication::applicationDirPath() << endl;

    //==========================Declare=============================================
    QString obs("C:/Utils/RinexSample/goml299o.23o");
    QString navGPS("C:/Utils/RinexSample/goml299o.23n");
    QString navGLO("C:/Utils/RinexSample/goml299o.23g");
    QString navGAL("C:/Utils/RinexSample/goml299o.23l");
    QString navBEI("C:/Utils/RinexSample/goml299o.23f");

    QStringList navPaths{navGPS, navGLO, navGAL, navBEI};

    QString pathCSVobs = "C:/Utils/RinexSample/infEpochs.csv";
    QString pathCSVnav = "C:/Utils/RinexSample/infNavs.csv";
    //==============================================================================


    //=============================RinexReader API(sample)==========================

    RinexReader rr(obs,navPaths);
    //rr.saveAsCSV(pathCSVobs, RinexType::OBSERVATION);

    rr.nextNav();
    rr.nextNav();
    rr.nextNav();
    rr.nextNav();
    rr.saveAsCSV(pathCSVnav, RinexType::NAVIGATION);




/*
    //work with navigation files
    out << "Nav info :" << endl;
    out <<"All nav files: " << endl;
    QStringList paths = rr.getPaths_nav();
    for(const QString &s :  qAsConst(paths)){ out << s << endl; }
    out << endl;
    Rinex3Nav infNav;

    rr.nextNav();
    out << rr.getCurr_path_nav() << endl;
    out << rr.getRinex_type_nav() << endl;
    out << rr.getRinex_version_nav() << endl << endl;
    infNav = rr.getNav();




    rr.nextNav();
    out << rr.getCurr_path_nav() << endl;
    out << rr.getRinex_type_nav() << endl;
    out << rr.getRinex_version_nav() << endl << endl;
    infNav = rr.getNav();

    rr.nextNav();
    out << rr.getCurr_path_nav() << endl;
    out << rr.getRinex_type_nav() << endl;
    out << rr.getRinex_version_nav() << endl << endl;
    infNav = rr.getNav();

    rr.nextNav();
    out << rr.getCurr_path_nav() << endl;
    out << rr.getRinex_type_nav() << endl;
    out << rr.getRinex_version_nav() << endl << endl;
    infNav = rr.getNav();


    rr.clearNav();
    rr.clearObs();*/
    //==============================================================================

}

void uploadDatatoDB(FacadeDB* db, RinexReader &rr){
    QDateTime st = QDateTime::currentDateTime();

    //===============================================================
    QList<Rinex3Obs::ObsEpochInfo> inf = rr.getEpochs();
    QList<Rinex3Obs::ObsEpochInfo>::Iterator listIt = inf.begin();
    for(listIt = inf.begin(); listIt != inf.end(); ++listIt){
        std::string obsTimeHMS = HHMMSS(listIt->epochRecord[3], listIt->epochRecord[4], listIt->epochRecord[5]);
        std::cout << "OBS Time : " << obsTimeHMS;
        std::map<std::string, std::map<int,std::vector<double>>>::iterator itEpoch = listIt->observations.begin();
        for(itEpoch = listIt->observations.begin(); itEpoch != listIt->observations.end(); itEpoch++){
            std::map<int,std::vector<double>> data = itEpoch->second;
            std::map<int,std::vector<double>>::iterator it = data.begin();
            for(it = data.begin(); it != data.end();it++){
                QString code = QString(itEpoch->first.data()) + QString("%1").arg(it->first,2,10,QChar('0'));
                std::vector<double> time = listIt->epochRecord;
                QDateTime dt(QDate(time.at(0),time.at(1),time.at(2)),QTime(time.at(3),time.at(4),time.at(5)));
                QList<double> qdate(it->second.begin(),it->second.end());

                if(!db->isExistSL(code))
                    db->addSatellite(code);
                db->addData(dt,code,qdate);
            }
        }
        std::cout << " + \n";
    }
    inf.clear();
    //===============================================================

    QDateTime finish = QDateTime::currentDateTime();
    double min;
    int secs, msecs;
    secs = st.secsTo(finish);
    st = st.addSecs(secs);
    msecs = st.time().msecsTo(finish.time());
    std::modf(secs / 60, &min);
    secs -= min * 60;
    std::cout << min <<" min. "<<secs << " sec. " << msecs << " ms."<< std::endl;
}

void testFacadeDB(){
    QTextStream out(stdout);
    using Qt::endl;
    FacadeDB* f_db = FacadeDB::getInstance();
    QDateTime dt(QDate(2024,9,3),QTime(8,13,39));
    QList<double> data = {26067809.260, 136987264.509, 2118.927, 37.600, -1, -1 , -1, -1, 26067810.580, 106743333.387, 1651.112, 26.450,-1,-1,-1,-1};
    QString prn("G03");

    QDateTime st = QDateTime::currentDateTime();

    out << "================TESTS===============================================================================" << endl;
    out << "================Satellite==========================" << endl;
    out << f_db->addSatellite(prn) << " - add satellite"<< endl;
    out << f_db->getSatelliteID(prn) << " - getSatelliteID(prn) " << endl;
    out << f_db->getSatelliteCode(f_db->getSatelliteID(prn)) << " - getSatelliteCode(id) " << endl;
    f_db->addSatellite("G04");
    f_db->addSatellite("G05");
    f_db->addSatellite("G06");
    QList<QString> satellites = f_db->getAllSatellites();
    out << "Satellites : ";
    foreach (QString var, satellites)
        out << var << ", ";
    out << " - getAllSatellites() "<< endl;
    out << f_db->isExistSL("G06") << " - isExist(prn) "<< endl;
    out << f_db->isExistSL(f_db->getSatelliteID("G06")) << " - isExist(id) "<< endl;
    out << f_db->removeSatellite(f_db->getSatelliteID("G06")) << " - removeSatellite(id)"<< endl;
    out << f_db->removeSatellite("G05") << " - removeSatellite(pnr)"<< endl;
    out << "===================================================" << endl;
    out << endl;


    out << "================GNSS_types=========================" << endl;
    out << f_db->addGNSS('K',QList<QString>{"C2I", "L2I", "D2I", "S2I", "C6I", "L6I", "D6I", "S6I", "C7I", "L7I", "D7I", "S7I"} ) << " - addGNSS() " << endl;
    out << f_db->updateGNSS('K',QList<QString>{"C2I", "L2I", "D2I", "S2I", "C6I", "L6I", "D6I", "S6I", "C7I", "L7I", "D7I", "S7I", "C8Q", "L8Q", "D8Q", "S8Q"}) << " - updateGNSS()" << endl;
    QList<char> list = f_db-> getAllGNSS();
    foreach (char c, list)
        out << c << ", ";
    out << " - getAllGNSS() " << endl;
    out << f_db->removeGNSS('K') << " - removeGNSS(gnss) " << endl;
    QMap<char,QList<QString>> m = f_db->getAllObsTypes();
    QMap<char,QList<QString>>::Iterator m_it = m.begin();
    for(m_it = m.begin(); m_it != m.end() ; ++m_it){
        out << m_it.key() << " : ";
        for(const QString &str:m_it.value())
            out << str << ", ";
        out << endl;
    }
    out << " - getAllObsTypes() " << endl;
    QList<QString>::Iterator it = m['G'].begin();
    for (it = m['G'].begin(); it != m['G'].end(); ++it)
        out << it->toStdString().c_str() << ", ";
    out << " - getAllObsTypes() 'G'" << endl;
    out << f_db->isExistGNSS('G') << " - isExistGNSS(gnss) " << endl;
    out << "===================================================" << endl;
    out << endl;


    out << "================Data_obs============================" << endl;
    out << f_db->addData(dt,prn,data)  << " - add data"<< endl;
    out << f_db->addData(dt,"G04",data)  << " - add data"<< endl;
    QMap<QDateTime, QMap<QString, QList<double>>> epochs  = f_db->getAllData();
    QMap<QDateTime, QMap<QString, QList<double>>>::Iterator ite = epochs.begin();
    for(ite = epochs.begin(); ite != epochs.end(); ++ite){
        QMap<QString, QList<double>>::Iterator it = ite.value().begin();
        out << ite.key().toString() << endl;
        for(it = ite.value().begin(); it != ite.value().end(); ++it){
            out << it.key() << " : ";
            for(const double x : it.value())
                out << x << ", ";
            out << endl;
        }
    }
    out << " - getAllData() " << endl;
    out << f_db->removeDataByTimeForSatellite(dt,prn) << " - remove data for satellite by data time" << endl;
    out << "====================================================" << endl;
    out << "====================================================================================================" << endl;

    out  << endl;
    f_db->removeAll();


    //std::this_thread::sleep_for(std::chrono::seconds(5));

    QDateTime finish = QDateTime::currentDateTime();
    double min = 0.;
    int secs, msecs;
    secs = st.secsTo(finish);
    st = st.addSecs(secs);
    msecs = st.time().msecsTo(finish.time());
    std::modf(secs / 60, &min);
    secs -= min * 60;
    out << min <<" min. "<<secs << " sec. " << msecs << " ms."<< endl;
}
