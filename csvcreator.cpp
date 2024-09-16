#include "csvcreator.h"
#include <QString>
#include <QDateTime>

CSVCreator::CSVCreator(QString sep): sep(sep){}

CSVCreator::~CSVCreator(){}

QString CSVCreator::getSep() const
{
    return sep;
}

void CSVCreator::setSep(QString newSep)
{
    sep = newSep;
}



CSVobs::CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs) : CSVobs(epochs,",") {}

CSVobs::CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs, QString sep) : CSVCreator(sep), epochs(epochs){}

CSVobs::~CSVobs() {}

bool CSVobs::createCSV(QString pathToSave)
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
                    QString code = QString(itObs->first.data()) + QString("%1").arg(it->first,2,10,QChar('0'));
                    //datetime
                    std::vector<double> time = listIt->epochRecord;
                    QDateTime dt(QDate(time.at(0),time.at(1),time.at(2)),QTime(time.at(3),time.at(4),time.at(5)));
                    //epoch data
                    QList<double> qdate(it->second.begin(),it->second.end());
                    QString epochData;
                    foreach (double x, qdate)
                        epochData = epochData.append("%1%2").arg(QString::number(x,'f',5), sep);
                    epochData.remove(epochData.size()-1,1);

                    line = line.append("%1%5%2%5%3%5%4").arg(dt.toString(Qt::ISODate), code, code.at(0), QString("\"{" + epochData + "}\"\n"), sep);
                    out << line.toStdString();

                    line.clear();
                }
            }
        }
        out.close();
    }
    return false;
}
