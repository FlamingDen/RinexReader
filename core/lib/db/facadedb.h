#ifndef FACADEDB_H
#define FACADEDB_H

#include <QObject>
#include <QtSql>

#include "pdp.h"

class FacadeDB : public QObject
{
protected:
    FacadeDB();

    static FacadeDB* facade_db;

public:
    FacadeDB(FacadeDB &other) = delete;
    void operator= (const FacadeDB &) = delete;
    static FacadeDB* getInstance();

    

    /*Satellite
     *code - обозначение спутника например 'G05' , 'E08' и т.д.
     *id - номер под которым он в БД
     */
    bool addSatellite(QString prn);
    bool removeSatellite(int id);
    bool removeSatellite(QString prn);
    bool removeSatelliteAll();
    QString getSatelliteCode(int id);
    int getSatelliteID(QString prn);
    QList<QString> getAllSatellites();
    bool isExistSL(QString prn);
    bool isExistSL(int id);

    /*GNSS observation types
     *gnss - тип систтемы(G = GPS, R = ГЛОНАСС, E = Galileo, C = Beidou, J = QZSS, I = NavIC/IRNSS, S = SBAS)
     *obs_types - типы наблюдений(по стандарту состоит из 3 сиволов)
     *Трехсимвольные коды наблюдения:
     *  -тип наблюдения (C = псевдодальность, L = фаза несущей, D = доплеровский, S = мощность сигнала, X = номер канала приемника),
     *  -диапазон/частоту (диапазон 1-9) и
     *  -атрибут (режим отслеживания или канал, например C, P, W, I, Q)
     */
    bool addGNSS(char gnss,QList<QString> obs_types);
    bool updateGNSS(char gnss,QList<QString> obs_types);
    bool removeGNSS(char gnss);
    bool removeGNSSAll();
    QList<char> getAllGNSS();
    QMap<char,QList<QString>> getObsTypes(char gnss);
    QMap<char,QList<QString>> getAllObsTypes();
    bool isExistGNSS(char gnss);

    /*data obsrvation
     * code - обозначение спутника например 'G05' , 'E08' и т.д.
     */
    bool addData(QDateTime dt, QString prn, QList<double> data);
    bool removeDataForSatellite(QString prn);
    bool removeDataByTimeForSatellite(QDateTime dt, QString prn);
    bool removeDataByTimeForSatellite(QDateTime startDT, QDateTime endDT, QString prn);
    bool removeDataByTimeForSatellites(QDateTime startDT, QDateTime endDT, QList<QString> prns);
    bool removeDataByTime(QDateTime dt);
    bool removeDataByTime(QDateTime startDT, QDateTime endDT);
    bool removeDataAll();
    bool updateData(QDateTime dt, QString prn, QList<double> data);
    QMap<QDateTime,QMap<QString,QList<double>>> getAllData();
    QMap<QDateTime,QMap<QString,QList<double>>> getDataByTime(QDateTime startDT, QDateTime endDT);

    void removeAll();

    QSqlDatabase getDatabase() const;


private:
    QSqlDatabase database;

    void disconnectDB();
    bool connectPostgresDB();

};

#endif // FACADEDB_H
