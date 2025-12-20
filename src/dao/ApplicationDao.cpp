#include "ApplicationDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicationDao::addApplication(int applicantId, int vacancyId, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO applications (applicant_id, vacancy_id, status) "
              "VALUES (:aid, :vid, 'Новая')");
    
    q.bindValue(":aid", applicantId);
    q.bindValue(":vid", vacancyId);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}