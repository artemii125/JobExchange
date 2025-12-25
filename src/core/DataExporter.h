#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QString>

class DataExporter {
public:
    static bool exportToCSV(const QString &tableName, const QString &filePath, QString &error);
    static bool importFromCSV(const QString &tableName, const QString &filePath, QString &error);
    static bool exportDatabaseToSQL(const QString &filePath, QString &error);
    static bool importDatabaseFromSQL(const QString &filePath, QString &error);
};

#endif
