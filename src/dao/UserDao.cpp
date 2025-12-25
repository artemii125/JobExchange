#include "UserDao.h"
#include "../core/DatabaseManager.h"
#include "../core/QueryLoader.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QVariant>

QString UserDao::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

UserDao::AuthResult UserDao::login(const QString& login, const QString& password) {
    QString sql = QueryLoader::instance().getQuery("LoginUser");
    QSqlQuery query(DatabaseManager::instance().db());
    
    query.prepare(sql);
    query.bindValue(":login", login);
    query.bindValue(":password", hashPassword(password));

    if (!query.exec()) return {false, "", "Ошибка БД: " + query.lastError().text()};

    if (query.next()) {
        return {true, query.value("role_name").toString(), ""};
    }

    return {false, "", "Неверный логин или пароль"};
}

UserDao::AuthResult UserDao::registerUser(const QString& login, const QString& password) {
    // Проверка на дубликаты
    QString checkSql = QueryLoader::instance().getQuery("CheckUserExists");
    QSqlQuery check(DatabaseManager::instance().db());
    check.prepare(checkSql);
    check.bindValue(":login", login);
    if(check.exec() && check.next()) return {false, "", "Пользователь уже существует"};

    // Регистрация
    QString registerSql = QueryLoader::instance().getQuery("RegisterUser");
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(registerSql);
    query.bindValue(":login", login);
    query.bindValue(":pass", hashPassword(password));

    if (query.exec()) return {true, "user", ""};
    
    return {false, "", "Ошибка регистрации: " + query.lastError().text()};
}