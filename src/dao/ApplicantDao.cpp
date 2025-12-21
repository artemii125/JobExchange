#include "ApplicantDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicantDao::addApplicant(const ApplicantData& data, QString& error) {
    if (exists(data)) {
        error = "Соискатель с такими ФИО, телефоном или Email уже существует!";
        return false;
    }
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO applicants (full_name, birth_date, phone, email, specialty, experience_years) "
              "VALUES (:name, :birth, :phone, :email, :spec, :exp)");
    
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

bool ApplicantDao::exists(const ApplicantData& data) {
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare("SELECT COUNT(*) FROM applicants WHERE full_name = :name OR phone = :phone OR email = :email");
    query.bindValue(":name", data.fullName);
    query.bindValue(":phone", data.phone);
    query.bindValue(":email", data.email);
    query.exec();

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
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