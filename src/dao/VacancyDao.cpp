#include "VacancyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

bool VacancyDao::addVacancy(const VacancyData& data, QString& error) {
    if (exists(data)) {
        error = "Такая вакансия в этой компании уже существует!";
        return false;
    }

    QString sql = DatabaseManager::instance().getQuery("AddVacancy");
    if (sql.isEmpty()) {
        error = "Ошибка: Запрос 'AddVacancy' не найден в queries.sql";
        return false;
    }

    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    
    q.bindValue(":company_id", data.companyId);
    q.bindValue(":spec", data.specialty);
    q.bindValue(":salary", data.salary);
    q.bindValue(":stat", data.status);
    q.bindValue(":posted_date", QDate::currentDate());

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool VacancyDao::exists(const VacancyData& data) {
    QString sql = DatabaseManager::instance().getQuery("CheckVacancyExists");
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);
    
    query.bindValue(":company_id", data.companyId);
    query.bindValue(":spec", data.specialty);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool VacancyDao::removeVacancy(int id, QString& error) {
    QString sql = DatabaseManager::instance().getQuery("RemoveVacancy");
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    q.bindValue(":id", id);
    
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}