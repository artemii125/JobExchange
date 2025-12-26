#include "ApplicantDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool ApplicantDao::addApplicant(const ApplicantData& data, QString& error) {
    if (exists(data)) {
        error = "Соискатель с такими ФИО, телефоном или Email уже существует!";
        return false;
    }
    //получаем SQL из внешнего хранилища
    QString sql = DatabaseManager::instance().getQuery("AddApplicant");
    if (sql.isEmpty()) {
        error = "Запрос 'AddApplicant' не найден в файле конфигурации!";
        return false;
    }

    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(sql);
    
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
    QString sql = DatabaseManager::instance().getQuery("CheckApplicantExists");
    QSqlQuery query(DatabaseManager::instance().db());
    
    query.prepare(sql);
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
    //получаем user_id соискателя
    QSqlQuery getUserQuery(DatabaseManager::instance().db());
    getUserQuery.prepare("SELECT user_id FROM applicants WHERE id = :id");
    getUserQuery.bindValue(":id", id);
    
    if (!getUserQuery.exec() || !getUserQuery.next()) {
        error = "Соискатель не найден";
        return false;
    }
    
    int userId = getUserQuery.value(0).toInt();
    
    //проверяем наличие связанных заявок
    QSqlQuery checkQuery(DatabaseManager::instance().db());
    checkQuery.prepare("SELECT COUNT(*) FROM applications WHERE applicant_id = :id");
    checkQuery.bindValue(":id", id);
    
    if (!checkQuery.exec()) {
        error = checkQuery.lastError().text();
        return false;
    }
    
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        error = "Нельзя удалить соискателя: он привязан к заявкам";
        return false;
    }

    //удаляем пользователя (каскадно удалится и соискатель)
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
        QString sql = DatabaseManager::instance().getQuery("RemoveApplicant");
        QSqlQuery q(DatabaseManager::instance().db());
        q.prepare(sql);
        q.bindValue(":id", id);

        if (!q.exec()) {
            error = q.lastError().text();
            return false;
        }
    }
    
    return true;
}