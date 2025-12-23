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
    QString sql = DatabaseManager::instance().getQuery("RemoveCompany");
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    q.bindValue(":id", id);

    if (!q.exec()) {
        error ="Не удалось удалить: компания связана с активными вакансиями или заявками.";;
        return false;
    }
    return true;
}