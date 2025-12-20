#include "ApplicantDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicantDao::addApplicant(const ApplicantData& data, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO applicants (full_name, birth_date, phone, email, specialty, experience_years, salary_expectation) "
              "VALUES (:name, :birth, :phone, :email, :spec, :exp, :salary)");
    
    q.bindValue(":name", data.fullName);
    q.bindValue(":birth", data.birthDate);
    q.bindValue(":phone", data.phone);
    q.bindValue(":email", data.email);
    q.bindValue(":spec", data.specialty);
    q.bindValue(":exp", data.experience);
    q.bindValue(":salary", data.salary);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool ApplicantDao::removeApplicant(int id, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM applicants WHERE id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}