#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QMap>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connect(); // Сюда добавим ожидание
    QSqlDatabase db() const;

    bool loadQueries(const QString &filePath);
    QString getQuery(const QString &key) const;

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase m_db;
    QMap<QString, QString> m_queries;
};

#endif // DATABASEMANAGER_H