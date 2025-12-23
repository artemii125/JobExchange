#include "ApplicationDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicationDao::addApplication(int applicantId, int vacancyId, QString& error) {
    QString sql = DatabaseManager::instance().getQuery("CheckApplicationExists");
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    q.bindValue(":a_id", applicantId);
    q.bindValue(":v_id", vacancyId);
    q.exec();
    if (q.next() && q.value(0).toInt() > 0) {
        error = "Вы уже откликнулись на эту вакансию!";
        return false;
    }
    
    QString addsql = DatabaseManager::instance().getQuery("AddApplication");
    q.prepare(addsql);
    q.bindValue(":aid", applicantId);
    q.bindValue(":vid", vacancyId);

    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool ApplicationDao::updateStatus(int applicationId, const QString& status) {
    QString sql = DatabaseManager::instance().getQuery("UpdateApplicationStatus");
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    q.bindValue(":status", status);
    q.bindValue(":id", applicationId);
    return q.exec();
}

bool ApplicationDao::removeApplication(int id, QString& error) {
    QString sql = DatabaseManager::instance().getQuery("RemoveApplication");
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    q.bindValue(":id", id);
    
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

