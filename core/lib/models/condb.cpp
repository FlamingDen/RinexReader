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

rr::Pdp rr::ConDb::GetPdp(QString identifige_name_)
{
    
    return rr::Pdp();
}

QHash<QString, rr::Pdp> rr::ConDb::GetPdpAll()
{
    return QHash<QString, rr::Pdp>();
}
