#include "ApplicationDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicationDao::addApplication(int applicantId, int vacancyId, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    
    q.prepare("SELECT COUNT(*) FROM applications WHERE applicant_id = :a_id AND vacancy_id = :v_id AND status = 'Ожидание'");
    q.bindValue(":a_id", applicantId);
    q.bindValue(":v_id", vacancyId);
    q.exec();
    if (q.next() && q.value(0).toInt() > 0) {
        error = "Вы уже откликнулись на эту вакансию!";
        return false;
    }
    
    q.prepare("INSERT INTO applications (applicant_id, vacancy_id, status) "
              "VALUES (:aid, :vid, 'Ожидание')");
    
    q.bindValue(":aid", applicantId);
    q.bindValue(":vid", vacancyId);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool ApplicationDao::updateStatus(int applicationId, const QString& status) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE applications SET status = :status WHERE id = :id");
    q.bindValue(":status", status);
    q.bindValue(":id", applicationId);
    return q.exec();
}

bool ApplicationDao::removeApplication(int id, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM applications WHERE id = :id");
    q.bindValue(":id", id);
    
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}