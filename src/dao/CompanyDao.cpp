#include "CompanyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool CompanyDao::addCompany(const CompanyData& data, QString& error) {
    if (exists(data)) {
        error = "Компания уже существует!";
        return false;
    }

    QString sql = DatabaseManager::instance().getQuery("AddCompany");
    QSqlQuery q(DatabaseManager::instance().db());
    
    if (!q.prepare(sql)) {
        error = "Ошибка подготовки: " + q.lastError().text();
        return false;
    }
    q.bindValue(":name", data.name);
    q.bindValue(":inn", data.inn);
    q.bindValue(":address", data.address); 
    q.bindValue(":phone", data.phone);
    q.bindValue(":contact_person", data.contact); // Было :contact

    if (!q.exec()) {
        error = q.lastError().text(); // Ошибка EXECUTE вылетает тут
        return false;
    }
    return true;
}
bool CompanyDao::exists(const CompanyData& data) {
    QString sql = DatabaseManager::instance().getQuery("CheckCompanyExists");
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);
    query.bindValue(":name", data.name);
    query.bindValue(":inn", data.inn);
    query.bindValue(":address", data.address);
    query.bindValue(":phone", data.phone);
    query.exec();

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool CompanyDao::removeCompany(int id, QString& error) {
    //получаем user_id компании
    QSqlQuery getUserQuery(DatabaseManager::instance().db());
    getUserQuery.prepare("SELECT user_id FROM companies WHERE id = :id");
    getUserQuery.bindValue(":id", id);
    
    if (!getUserQuery.exec() || !getUserQuery.next()) {
        error = "Компания не найдена";
        return false;
    }
    
    int userId = getUserQuery.value(0).toInt();
    
    //проверяем наличие связанных вакансий
    QSqlQuery checkQuery(DatabaseManager::instance().db());
    checkQuery.prepare("SELECT COUNT(*) FROM vacancies WHERE company_id = :id");
    checkQuery.bindValue(":id", id);
    
    if (!checkQuery.exec()) {
        error = checkQuery.lastError().text();
        return false;
    }
    
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        error = "Нельзя удалить компанию: у неё есть активные вакансии";
        return false;
    }

    //удаляем пользователя (каскадно удалится и компания)
    if (userId > 0) {
        QSqlQuery deleteUserQuery(DatabaseManager::instance().db());
        deleteUserQuery.prepare("DELETE FROM users WHERE id = :user_id");
        deleteUserQuery.bindValue(":user_id", userId);
        
        if (!deleteUserQuery.exec()) {
            error = deleteUserQuery.lastError().text();
            return false;
        }
    } else {
        //если нет user_id, удаляем напрямую
        QString sql = DatabaseManager::instance().getQuery("RemoveCompany");
        QSqlQuery q(DatabaseManager::instance().db());
        q.prepare(sql);
        q.bindValue(":id", id);

        if (!q.exec()) {
            error = "Не удалось удалить: компания связана с активными вакансиями или заявками.";
            return false;
        }
    }
    
    return true;
}