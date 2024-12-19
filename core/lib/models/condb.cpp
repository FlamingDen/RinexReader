#include "condb.h"

//=======================================================================================
rr::ConDb::ConDb(){
    connectPostgresDB();
}

bool rr::ConDb::connectPostgresDB()
{
    database = QSqlDatabase::addDatabase(DRIVER);
    database.setHostName(HOST_NAME);
    database.setPort(PORT);
    database.setDatabaseName(DATABASE_NAME);
    database.setUserName(USER);
    database.setPassword(PASSWORD);
    if (!database.open())
    {
        qDebug() << "Cannot open database: " << database.lastError();
        return false;
    }
    return true;
}

void rr::ConDb::disconnectDB()
{
    database.close();
    database.removeDatabase(database.connectionName());
}

rr::ConDb *rr::ConDb::con_db = nullptr;

rr::ConDb *rr::ConDb::GetInstance()
{
    if(con_db == nullptr)
        con_db = new ConDb();
    return con_db;
}

QSqlDatabase rr::ConDb::getDatabase() const
{
    return database;
}
//=======================================================================================

rr::Pdp rr::ConDb::GetPdp(const QString &identifige_name_) const
{
    QSqlQuery q;
    rr::Pdp res;

    q.prepare(
        "SELECT idpdp, \"name\", idname,  st_x(geom), st_y(geom) "
        "FROM pole.cur_pdp p "
        "where p.idname = ? ;");
    q.addBindValue(identifige_name_);
    if (!q.exec())
    {
        qDebug() << q.lastError();
    }
    q.next();

    res.SetId(q.value(0).toInt());
    res.SetName(q.value(1).toString().toStdString());
    res.SetIdentifierName(q.value(2).toString().toStdString());
    std::tuple<double, double, double> point{
        q.value(4).toDouble(),
        q.value(4).toDouble(),
        0
    };
    res.SetCoordinates(point);

    return res;
}

QHash<QString, rr::Pdp> rr::ConDb::GetPdpAll() const
{
    QSqlQuery q;
    

    q.prepare(
        "SELECT idpdp, \"name\", idname,  st_x(geom), st_y(geom) "
        "FROM pole.cur_pdp p ;"
        );
    if (!q.exec())
    {
        qDebug() << q.lastError();
    }

    QHash<QString, rr::Pdp> res;
    rr::Pdp pdp;
    while(q.next()){
        pdp.SetId(q.value(0).toInt());
        pdp.SetName(q.value(1).toString().toStdString());
        pdp.SetIdentifierName(q.value(2).toString().toStdString());
        std::tuple<double, double, double> point{
            q.value(4).toDouble(),
            q.value(4).toDouble(),
            0};
        pdp.SetCoordinates(point); 
        res[pdp.GetIdentifierName().data()] = pdp; 
    }
    return res;
}

bool rr::ConDb::IsExist(const QString &identifige_name_) const
{
    QSqlQuery q;
    rr::Pdp res;

    q.prepare(
        "select exists(                    "
        "   select 1 from pole.cur_pdp p   "
        "   where p.idname = ?             "
        "   limit 1);                      "
    );
    q.addBindValue(identifige_name_);
    if (!q.exec())
    {
        qDebug() << q.lastError();
    }
    q.next();
    return q.value(0).toBool();
}
