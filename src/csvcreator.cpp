#include "csvcreator.h"
#include <QString>
#include <QDateTime>
#include "enumtypes.h"



rr::CSVCreator::CSVCreator(QString sep): sep(sep){}

rr::CSVCreator::~CSVCreator(){}

QString rr::CSVCreator::getSep() const
{
    return sep;
}

void rr::CSVCreator::setSep(QString newSep)
{
    sep = newSep;
}



rr::CSVobs::CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs) : CSVobs(epochs,",") {}

rr::CSVobs::CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs, QString sep) : CSVCreator(sep), epochs(epochs){}

rr::CSVobs::~CSVobs() {}


void rr::CSVobs::createCSV(QString pathToSave)
{
    std::ofstream out(pathToSave.toStdString());
    if(out.is_open() && !out.bad()){
        QString line;
        QList<Rinex3Obs::ObsEpochInfo>::iterator listIt = epochs.begin();
        for(listIt = epochs.begin(); listIt != epochs.end(); ++listIt){
            std::map<std::string, std::map<int,std::vector<double>>>::iterator itObs = listIt->observations.begin();
            for(itObs = listIt->observations.begin(); itObs != listIt->observations.end(); itObs++){
                std::map<int,std::vector<double>> data = itObs->second;
                std::map<int,std::vector<double>>::iterator it = data.begin();
                for(it = data.begin(); it != data.end();it++){
                    //prn
                    QString prn = QString(itObs->first.data()) + QString("%1").arg(it->first,2,10,QChar('0'));
                    //datetime
                    std::vector<double> time = listIt->epochRecord;
                    QDateTime dt(QDate(time.at(0),time.at(1),time.at(2)),QTime(time.at(3),time.at(4),time.at(5)));
                    //epoch data
                    QList<double> qdate(it->second.begin(),it->second.end());
                    QString epochData;
                    foreach (double x, qdate)
                        epochData = epochData.append("%1%2").arg(QString::number(x,'f',5), sep);
                    epochData.remove(epochData.size()-1,1);

                    line = line.append("%1%5%2%5%3%5%4").arg(dt.toString(Qt::ISODate), prn, prn.at(0), QString("\"{" + epochData + "}\"\n"), sep);
                    out << line.toStdString();

                    line.clear();
                }
            }
        }
        out.close();
    }
}



rr::CSVnav::CSVnav(const Rinex3Nav &nav) : CSVnav(nav,","){}

rr::CSVnav::CSVnav(const Rinex3Nav &nav, QString sep): CSVCreator(sep), nav(nav){}

rr::CSVnav::~CSVnav(){}

template<typename T>
void createCSVHelperNav(std::map<int, std::vector<T>> _nav, std::ofstream& out, QString sep){
    QString line;
    typename std::map<int, std::vector<T>>::iterator it = _nav.begin();
    for (it = _nav.begin(); it != _nav.end(); it++){
        typename std::vector<T>::iterator satIt = it->second.begin();
        for(satIt = it->second.begin(); satIt != it->second.end(); satIt++){
            //prn
            QString prn = rr::getSatelliteSystemShort(satIt->SatelliteSystem) + QString("%1").arg(satIt->PRN,2,10,QChar('0'));
            //datetime
            std::vector<double> time = satIt->epochInfo;
            QDateTime dt(QDate(time.at(0),time.at(1),time.at(2)),QTime(time.at(3),time.at(4),time.at(5)));
            //nav data
            std::vector<std::optional<double>> vec = satIt->toVec();
            QString strNavData;
            foreach (std::optional<double> x, vec){
                x.has_value() ? strNavData = strNavData.append("%1%2").arg(QString::number(x.value(),'f',30), sep) :
                                strNavData = strNavData.append("%1%2").arg("", sep);
            }
            strNavData.remove(strNavData.size()-1,1);

            line = line.append("%1%5%2%5%3%5%4").arg(dt.toString(Qt::ISODate), prn, prn.at(0), QString("\"{" + strNavData + "}\"\n"), sep);
            out << line.toStdString();

            line.clear();
        }
    }
}

void rr::CSVnav::createCSV(QString pathToSave)
{
    std::ofstream out(pathToSave.toStdString());
    if(out.is_open() && !out.bad()){
        //GPS
        if(!nav._navGPS.empty()){
            createCSVHelperNav(nav._navGPS,out, sep);
        }
        //GLONASS
        if(!nav._navGLO.empty()){
            createCSVHelperNav(nav._navGLO, out, sep);
        }
        //GALILEO
        if(!nav._navGAL.empty()){
            createCSVHelperNav(nav._navGAL,out, sep);
        }
        //BEIDOU
        if(!nav._navBEI.empty()){
            createCSVHelperNav(nav._navBEI,out, sep);
        }
    }
}
