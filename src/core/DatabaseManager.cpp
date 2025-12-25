#include "DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlError>
#include <QThread>
#include <QSqlQuery>

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
    m_db.setPort(5433);
    m_db.setDatabaseName("jobexchange");
    m_db.setUserName("postgres");
    m_db.setPassword("secret_password");

    int retries = 5;
    bool connected = false;
    while (retries > 0) {
        if (m_db.open()) {
            connected = true;
            break;
        }
        qWarning() << "База данных недоступна. Повтор через 2 сек... (" << m_db.lastError().text() << ")";
        QThread::sleep(2);
        retries--;
    }
    if (connected) {
        qInfo() << "Успешное подключение к БД.";
        // Мы вызываем loadQueries прямо здесь
        if (!loadQueries("db/queries.sql")) {
            qCritical() << "ОШИБКА: Подключились, но файл запросов queries.sql не найден!";
            return false; // Считаем подключение неудачным, так как работать без запросов нельзя
        }
        qInfo() << "SQL-запросы успешно инициализированы.";
        return true;
    }
    
    qCritical() << "ОШИБКА: Не удалось подключиться к БД.";
    return false;
}

QSqlDatabase DatabaseManager::db() const {
    return m_db;
}

bool DatabaseManager::loadQueries(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Не удалось открыть файл запросов:" << filePath;
        return false;
    }

    QTextStream in(&file);
    m_queries.clear();
    QString currentKey;
    QString currentSql;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Если нашли заголовок ключа
        if (line.startsWith("-- [") && line.contains("]")) {
            // Сохраняем предыдущий накопленный запрос
            if (!currentKey.isEmpty() && !currentSql.trimmed().isEmpty()) {
                m_queries[currentKey] = currentSql.trimmed();
                qDebug() << "Добавлен запрос в память:" << currentKey;
            }
            
            // Вырезаем новый ключ
            int start = line.indexOf('[') + 1;
            int end = line.lastIndexOf(']');
            currentKey = line.mid(start, end - start).trimmed();
            currentSql = ""; // Сбрасываем текст для нового ключа
        } 
        // Если это не заголовок и не пустой комментарий — копим SQL
        else if (!line.startsWith("--") && !line.isEmpty()) {
            currentSql += line + " ";
        }
    }
    
    // Не забываем сохранить самый последний запрос из файла
    if (!currentKey.isEmpty() && !currentSql.trimmed().isEmpty()) {
        m_queries[currentKey] = currentSql.trimmed();
        qDebug() << "Добавлен запрос в память (последний):" << currentKey;
    }

    file.close();
    qDebug() << "ИТОГО загружено ключей:" << m_queries.keys(); // Выведет список всех имен
    return true;
}
QString DatabaseManager::getQuery(const QString &key) const {
    return m_queries.value(key, "");
}