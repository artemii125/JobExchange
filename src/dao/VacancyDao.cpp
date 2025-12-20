#include "VacancyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool VacancyDao::addVacancy(const VacancyData& data, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO vacancies (company_id, position, specialty, salary, status) "
              "VALUES (:cid, :pos, :spec, :sal, :stat)");
    
    q.bindValue(":cid", data.companyId);
    q.bindValue(":pos", data.position);
    q.bindValue(":spec", data.specialty);
    q.bindValue(":sal", data.salary);
    q.bindValue(":stat", data.status);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool VacancyDao::removeVacancy(int id, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM vacancies WHERE id = :id");
    q.bindValue(":id", id);
    
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}