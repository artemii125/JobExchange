#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connect(); // Сюда добавим ожидание
    QSqlDatabase db() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H