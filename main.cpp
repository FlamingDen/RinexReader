#include <QCoreApplication>
#include <QtSql>
#include <QTextStream>

#include "facadedb.h"
#include "rinexreader.h"

void testFacadeDB();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    using Qt::endl;

    QString obs("C:/Utils/RinexSample/goml299o.23o");
    QString nav("C:/Utils/RinexSample/goml299o.23n");
    //FacadeDB* db = FacadeDB::getInstance();
    std::ifstream inputfile(obs.toStdString());
    if (!inputfile.is_open()) {
        perror("Error while opening file");
    }
    //==============================================================================
    // RinexReader rnxReader(obs, nav, db);
    // rnxReader.uploadObsToDB();
    // out << rnxReader.getRinex_type_obs() << endl;
    // out << rnxReader.getRinex_version_obs() << endl;
    // out << rnxReader.getDb()->getDatabase().connectionName() << endl;
    // out << rnxReader.getDb()->getDatabase().databaseName() << endl;
    // rnxReader.getDb()->removeAll();
    //==============================================================================


    //=============================RinexReader API=============================================
    RinexReader rr(obs,nav);// obs + 1nav
    RinexReader rr(obs,RinexType::OBSERVATION); // onle obs
    RinexReader rr(nav,RinexType::NAVIGATION);  // onle nav

    //rr.addPath_nav(nav);
    out << "Nav info :" << endl;
    for(const QString &s : rr.getPaths_nav())
        out << s << endl;
    out << rr.getRinex_type_nav() << endl;
    out << rr.getRinex_version_nav() << endl << endl;

    //rr.setPath_obs(obs);
    out << "Obs info :" << endl;
    out << rr.getPath_obs() << endl;
    out << rr.getRinex_type_obs() << endl;
    out << rr.getRinex_version_obs() << endl << endl;

    rr.readNav();
    rr.readObs(); // or     rr.readObsHeader(); rr.readEpochs();


    rr.clearNav();
    rr.clearObs();

    out << endl;
    //==============================================================================


    //=============================TestFacadeDB=============================================
    //testFacadeDB();
    //==============================================================================
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
