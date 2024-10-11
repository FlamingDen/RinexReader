#include "facadedb.h"

#include <QString>
#include <QVariant>

FacadeDB::FacadeDB(): QObject{}
{
    connectPostgresDB();
}

FacadeDB* FacadeDB::facade_db = nullptr;

FacadeDB* FacadeDB::getInstance()
{
    if(facade_db == nullptr)
        facade_db = new FacadeDB();
    return facade_db;
}


//====================================Satellites=========================================
bool FacadeDB::addSatellite(QString prn)
{
    QSqlQuery q;
    q.prepare("insert into satellites(prn) values (?);");
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeSatellite(int id)
{
    QSqlQuery q;
    q.prepare("delete from satellites where id_satellite = ?;");
    q.addBindValue(id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeSatellite(QString prn)
{
    QSqlQuery q;
    q.prepare("delete from satellites where prn = ?;");
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeSatelliteAll()
{
    QSqlQuery q;
    q.prepare("delete from satellites;");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

QString FacadeDB::getSatelliteCode(int id)
{
    QSqlQuery q;
    q.prepare("select s.prn "
              "from satellites s  "
              "where s.id_satellite = ?;");
    q.addBindValue(id);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    return q.value(0).toString();
}

int FacadeDB::getSatelliteID(QString prn)
{
    QSqlQuery q;
    q.prepare("select s.id_satellite "
              "from satellites s  "
              "where s.prn = ?;");
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    return q.value(0).toInt();
}

QList<QString> FacadeDB::getAllSatellites()
{
    QSqlQuery q;
    q.prepare("select * from satellites;");
    QList<QString> satellites;
    if(q.exec()){
        QSqlRecord rec = q.record();
        QString st;
        while(q.next()){
            st = q.value(rec.indexOf("prn")).toString();
            satellites.append(st);
        }
    }
    return satellites;
}

bool FacadeDB::isExistSL(QString prn)
{
    QSqlQuery q;
    q.prepare("select exists (  select 1 "
              "                 from satellites s "
              "                 where s.prn = ? "
              "                 limit 1);");
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    return  q.value(0).toBool();
}

bool FacadeDB::isExistSL(int id)
{
    QSqlQuery q;
    q.prepare("select exists (  select 1 "
              "                 from satellites s "
              "                 where s.id_satellite = ? "
              "                 limit 1);");
    q.addBindValue(id);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    return q.value(0).toBool();
}
//=======================================================================================



//=======================================================================================
bool FacadeDB::addGNSS(char gnss, QList<QString> obs_types)
{
    QSqlQuery q;
    q.prepare("insert into gnss_obs_type "
              "values (?, ?); ");
    q.addBindValue(QChar(gnss));
    QListIterator it(obs_types);
    QString strTypes;
    while(it.hasNext()){
        strTypes.append(it.next() + ',');
    }
    strTypes.remove(strTypes.size()-1,1);
    q.addBindValue("{"+strTypes+"}");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::updateGNSS(char gnss, QList<QString> obs_types)
{
    QSqlQuery q;
    q.prepare("update gnss_obs_type "
              "set obs_types = ? "
              "where id_gnss = ?; ");
    QListIterator it(obs_types);
    QString strTypes;
    while(it.hasNext()){
        strTypes.append(it.next() + ',');
    }
    strTypes.remove(strTypes.size()-1,1);
    q.addBindValue('{'+strTypes+'}');
    q.addBindValue(QChar(gnss));
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeGNSS(char gnss)
{
    QSqlQuery q;
    q.prepare("delete from gnss_obs_type "
              "where id_gnss = ?; ");
    q.addBindValue(QChar(gnss));
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeGNSSAll()
{
    QSqlQuery q;
    q.prepare("delete from gnss_obs_type;");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

QList<char> FacadeDB::getAllGNSS()
{
    QSqlQuery q;
    q.prepare("select g.id_gnss "
              "from gnss_obs_type g; ");
    QList<char> gnss;
    if(q.exec()){
        QString st;
        while(q.next()){
            st = q.value(0).toString();
            gnss.append(st.toStdString().at(0));
        }
    }
    return gnss;
}

QMap<char, QList<QString> > FacadeDB::getObsTypes(char gnss)
{
    QSqlQuery q;
    q.prepare("select * "
              "from gnss_obs_type g"
              "where g.id_gnss = ?; ");
    q.addBindValue(QChar(gnss));
    QMap<char, QList<QString>> gnss_types;
    if(q.exec()){
        QString st;
        QList<QString > types;
        while(q.next()){
            st = q.value(0).toString();
            types = q.value(1).toString().replace("{","").replace("}","").split(QRegularExpression("\\s*,\\s*"));
            gnss_types.insert(st.toStdString().at(0),types);
        }
    }
    return gnss_types;
}

QMap<char, QList<QString>> FacadeDB::getAllObsTypes()
{
    QSqlQuery q;
    q.prepare("select * "
              "from gnss_obs_type g; ");
    QMap<char, QList<QString>> gnss_types;
    if(q.exec()){
        QString st;
        QList<QString > types;
        while(q.next()){
            st = q.value(0).toString();
            types = q.value(1).toString().replace("{","").replace("}","").split(QRegularExpression("\\s*,\\s*"));
            gnss_types.insert(st.toStdString().at(0),types);
        }
    }
    return gnss_types;
}

bool FacadeDB::isExistGNSS(char gnss)
{
    QSqlQuery q;
    q.prepare("select exists (  select 1 "
              "                 from gnss_obs_type g "
              "                 where g.id_gnss = ? "
              "                 limit 1);");
    q.addBindValue(QChar(gnss));
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    return  q.value(0).toBool();
}
//=======================================================================================



//=======================================================================================
bool FacadeDB::addData(QDateTime dt, QString prn, QList<double> data)
{
    QSqlQuery q;
    q.prepare("insert into public.data_obs "
              "values (?, ?, ?, ?);");
    q.addBindValue(dt.toString(Qt::ISODate));
    q.addBindValue(prn);
    q.addBindValue(prn.at(0));
    QListIterator it(data);
    QString strData("");
    while(it.hasNext()){
        strData.append(QString::number(it.next())+ ',');
    }
    strData.remove(strData.size()-1,1);
    q.addBindValue("{"+strData+"}");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataForSatellite(QString prn)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where prn = ?; ");
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataByTimeForSatellite(QDateTime dt, QString prn)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where date_time = ? and prn = ?; ");
    q.addBindValue(dt.toString(Qt::ISODate));
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataByTimeForSatellite(QDateTime startDT, QDateTime endDT, QString prn)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where  (date_time between ? and ? ) "
              "       and prn = ?; ");
    q.addBindValue(startDT.toString(Qt::ISODate));
    q.addBindValue(endDT.toString(Qt::ISODate));
    q.addBindValue(prn);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataByTimeForSatellites(QDateTime startDT, QDateTime endDT, QList<QString> prns)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where  (date_time between ? and ? ) "
              "       and prn in (?); ");
    q.addBindValue(startDT.toString(Qt::ISODate));
    q.addBindValue(endDT.toString(Qt::ISODate));
    QString data;
    QListIterator it(prns);
    while (it.hasNext()) {
        data.append('\''+it.next()+ "',");
    }
    q.addBindValue(data);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataByTime(QDateTime dt)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where date_time = ?; ");
    q.addBindValue(dt.toString(Qt::ISODate));
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataByTime(QDateTime startDT, QDateTime endDT)
{
    QSqlQuery q;
    q.prepare("delete from data_obs "
              "where date_time between ? and ?  ;");
    q.addBindValue(startDT.toString(Qt::ISODate));
    q.addBindValue(endDT.toString(Qt::ISODate));
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::removeDataAll()
{
    QSqlQuery q;
    q.prepare("delete from data_obs;");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool FacadeDB::updateData(QDateTime dt, QString prn, QList<double> data)
{
    QSqlQuery q;
    q.prepare("update data_obs o "
              "set o.obs_data = :data  "
              "where o.date_time = ? and o.prn = ?;");
    q.addBindValue(dt);
    q.addBindValue(prn);
    QListIterator it(data);
    QString strTypes;
    while(it.hasNext()){
        strTypes.append(QString::number(it.next()) + ',');
    }
    strTypes.remove(strTypes.size()-1,1);
    q.bindValue(":data","{"+strTypes+"}");
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

QMap<QDateTime, QMap<QString, QList<double>>> FacadeDB::getAllData()
{
    QSqlQuery q;
    q.prepare("select d.date_time, d.prn, d.obs_data "
              "from data_obs d; ");
    QMap<QDateTime, QMap<QString, QList<double>>> answer;
    if(q.exec()){
        QDateTime dt;
        QMap<QString, QList<double>> m;
        QString prn;
        QList<double> data;
        while(q.next()){
            m.clear();
            dt = q.value(0).toDateTime();
            prn = q.value(1).toString();
            QStringList strList = q.value(2).toString().replace("{","").replace("}","").split(QRegularExpression("\\s*,\\s*"));
            auto getDoubleList {[](QStringList strlist) -> QList<double> {
                QList<double> res;
                for(const QString &str : strlist)
                    res.append(str.toDouble());
                return res;
            }};
            data = getDoubleList(strList);
            if (answer.contains(dt)){
                answer[dt].insert(prn,getDoubleList(strList));
            }else {
                m.insert(prn,getDoubleList(strList));
                answer.insert(dt,m);
            }
        }
    }
    return answer;
}

QMap<QDateTime, QMap<QString, QList<double> > > FacadeDB::getDataByTime(QDateTime startDT, QDateTime endDT)
{
    QSqlQuery q;
    q.prepare("select d.date_time, d.prn, d.obs_data "
              "from data_obs d "
              "where d.date_time between ? and ? ; ");
    q.addBindValue(startDT);
    q.addBindValue(endDT);
    QMap<QDateTime, QMap<QString, QList<double>>> answer;
    if(q.exec()){
        QDateTime dt;
        QString prn;
        QMap<QString, QList<double>> m;
        QList<double> data;

        while(q.next()){
            m.clear();
            dt = q.value(0).toDateTime();
            prn = q.value(1).toString();
            QStringList strList = q.value(2).toString().replace("{","").replace("}","").split(QRegularExpression("\\s*,\\s*"));
            auto getDoubleList {[](QStringList strlist) -> QList<double> {
                QList<double> res;
                for(const QString &str : strlist)
                    res.append(str.toDouble());
                return res;
            }};
            data = getDoubleList(strList);
            if (answer.contains(dt)){
                answer[dt].insert(prn,getDoubleList(strList));
            }else {
                m.insert(prn,getDoubleList(strList));
                answer.insert(dt,m);
            }
        }
    }
    return answer;
}
//=======================================================================================


//=======================================================================================
void FacadeDB::removeAll()
{
    removeDataAll();
    removeSatelliteAll();
    //removeGNSSAll();
}
//=======================================================================================



//=======================================================================================
QSqlDatabase FacadeDB::getDatabase() const
{
    return database;
}

void FacadeDB::disconnectDB()
{
    database.close();
    database.removeDatabase(database.connectionName());
}

bool FacadeDB::connectPostgresDB()
{
    database = QSqlDatabase::addDatabase("QPSQL");
    database.setDatabaseName("rinex_test");
    database.setUserName("postgres");
    database.setPassword("postgres");
    if(!database.open()){
        qDebug() << "Cannot open database: " << database.lastError();
        return false;
    }
    return true;
}
//=======================================================================================
