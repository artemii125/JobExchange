#include "UserDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QVariant>

QString UserDao::hashPassword(const QString& password) {
    // Хешируем пароль (SHA-256), чтобы не хранить в открытом виде
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

UserDao::AuthResult UserDao::login(const QString& login, const QString& password) {
    QSqlQuery query(DatabaseManager::instance().db());
    
    // Параметризованный запрос (защита от SQL Injection)
    query.prepare("SELECT r.name FROM users u "
                  "JOIN roles r ON u.role_id = r.id "
                  "WHERE u.login = :login AND u.password_hash = :pass");
    
    query.bindValue(":login", login);
    query.bindValue(":pass", hashPassword(password));

    if (!query.exec()) return {false, "", "Ошибка БД: " + query.lastError().text()};

    if (query.next()) {
        return {true, query.value(0).toString(), ""};
    }

    return {false, "", "Неверный логин или пароль"};
}

UserDao::AuthResult UserDao::registerUser(const QString& login, const QString& password) {
    QSqlQuery query(DatabaseManager::instance().db());
    
    // Проверка на дубликаты
    QSqlQuery check(DatabaseManager::instance().db());
    check.prepare("SELECT id FROM users WHERE login = :login");
    check.bindValue(":login", login);
    if(check.exec() && check.next()) return {false, "", "Пользователь уже существует"};

    // Регистрация
    query.prepare("INSERT INTO users (login, password_hash, role_id) "
                  "VALUES (:login, :pass, (SELECT id FROM roles WHERE name = 'user'))");
    
    query.bindValue(":login", login);
    query.bindValue(":pass", hashPassword(password));

    if (query.exec()) return {true, "user", ""};
    
    return {false, "", "Ошибка регистрации: " + query.lastError().text()};
}