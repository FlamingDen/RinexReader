﻿#pragma once

#include <QtSql>

#include "pdp.h"

namespace rr
{
    class ConDb{
    private:
        static ConDb* con_db;
        QSqlDatabase database;

        const QString DRIVER = "QPSQL";
        const QString HOST_NAME = "192.168.1.155";
        const int PORT = 5432;
        const QString DATABASE_NAME = "pole";
        const QString USER = "admin";
        const QString PASSWORD = "admin";

        ConDb();

        bool connectPostgresDB();

        void disconnectDB();
        
    public:
        ConDb(const ConDb& other) = delete;
        void operator=(const ConDb& other) = delete;
        static ConDb* GetInstance();

        QSqlDatabase getDatabase() const;

        /*
        */
        rr::Pdp GetPdp(QString identifige_name_);
        QHash<QString, rr::Pdp> GetPdpAll();
    };
} // namespace rr