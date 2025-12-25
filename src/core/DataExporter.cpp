#include "DataExporter.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

bool DataExporter::exportToCSV(const QString &tableName, const QString &filePath, QString &error) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error = "Не удалось открыть файл для записи";
        return false;
    }

    QTextStream out(&file);
    QSqlQuery query(DatabaseManager::instance().db());
    
    if (!query.exec("SELECT * FROM " + tableName)) {
        error = query.lastError().text();
        return false;
    }

    QSqlRecord record = query.record();
    QStringList headers;
    for (int i = 0; i < record.count(); ++i) {
        headers << record.fieldName(i);
    }
    out << headers.join(",") << "\n";

    while (query.next()) {
        QStringList row;
        for (int i = 0; i < record.count(); ++i) {
            QString value = query.value(i).toString();
            if (value.contains(",") || value.contains("\"")) {
                value = "\"" + value.replace("\"", "\"\"") + "\"";
            }
            row << value;
        }
        out << row.join(",") << "\n";
    }

    file.close();
    return true;
}

bool DataExporter::importFromCSV(const QString &tableName, const QString &filePath, QString &error) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = "Не удалось открыть файл для чтения";
        return false;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");

    QSqlDatabase db = DatabaseManager::instance().db();
    db.transaction();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");
        
        if (values.size() != headers.size()) continue;

        QString placeholders = QString("?,").repeated(values.size());
        placeholders.chop(1);
        
        QSqlQuery query(db);
        query.prepare(QString("INSERT INTO %1 VALUES (%2)").arg(tableName, placeholders));
        
        for (const QString &value : values) {
            query.addBindValue(value);
        }
        
        if (!query.exec()) {
            db.rollback();
            error = query.lastError().text();
            return false;
        }
    }

    db.commit();
    file.close();
    return true;
}

bool DataExporter::exportDatabaseToSQL(const QString &filePath, QString &error) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error = "Не удалось открыть файл для записи";
        return false;
    }

    QTextStream out(&file);
    QSqlDatabase db = DatabaseManager::instance().db();
    
    QStringList tables = {"roles", "users", "companies", "applicants", "vacancies", "applications"};
    
    for (const QString &table : tables) {
        QSqlQuery query(db);
        if (!query.exec("SELECT * FROM " + table)) continue;
        
        QSqlRecord record = query.record();
        
        while (query.next()) {
            QStringList values;
            for (int i = 0; i < record.count(); ++i) {
                QVariant val = query.value(i);
                if (val.isNull()) {
                    values << "NULL";
                } else if (val.typeId() == QMetaType::QString) {
                    values << "'" + val.toString().replace("'", "''") + "'";
                } else {
                    values << val.toString();
                }
            }
            out << QString("INSERT INTO %1 VALUES (%2);\n").arg(table, values.join(", "));
        }
    }

    file.close();
    return true;
}

bool DataExporter::importDatabaseFromSQL(const QString &filePath, QString &error) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = "Не удалось открыть файл для чтения";
        return false;
    }

    QTextStream in(&file);
    QSqlDatabase db = DatabaseManager::instance().db();
    db.transaction();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("--")) continue;

        QSqlQuery query(db);
        if (!query.exec(line)) {
            db.rollback();
            error = query.lastError().text();
            return false;
        }
    }

    db.commit();
    file.close();
    return true;
}
