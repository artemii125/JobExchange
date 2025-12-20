#include "CompanyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool CompanyDao::addCompany(const CompanyData& data, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO companies (name, inn, phone, contact_person, address) "
              "VALUES (:name, :inn, :phone, :contact, :addr)");
    
    q.bindValue(":name", data.name);
    q.bindValue(":inn", data.inn);
    q.bindValue(":phone", data.phone);
    q.bindValue(":contact", data.contact);
    q.bindValue(":addr", data.address);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool CompanyDao::removeCompany(int id, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM companies WHERE id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}