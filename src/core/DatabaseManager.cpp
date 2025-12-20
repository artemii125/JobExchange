#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>
#include <QThread> // Для паузы

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::connect() {
    m_db.setHostName("localhost");
    m_db.setPort(5432);
    m_db.setDatabaseName("jobexchange");
    m_db.setUserName("postgres");
    m_db.setPassword("secret_password");

    int retries = 5; // Ждем базу 10 секунд (5 * 2)
    while (retries > 0) {
        if (m_db.open()) {
            qInfo() << "Успешное подключение к БД.";
            return true;
        }
        qWarning() << "База данных недоступна. Повтор через 2 сек... (" << m_db.lastError().text() << ")";
        QThread::sleep(2);
        retries--;
    }
    
    qCritical() << "ОШИБКА: Не удалось подключиться к БД.";
    return false;
}

QSqlDatabase DatabaseManager::db() const {
    return m_db;
}