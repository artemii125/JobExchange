#include "VacancyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>

bool VacancyDao::addVacancy(const VacancyData& data, QString& error) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO vacancies (company_id, position,  specialty, salary, status) "
              "VALUES (:cid, :spec, :spec, :sal, :stat)");
    
    q.bindValue(":cid", data.companyId);
    q.bindValue(":pos", data.specialty);
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

    q.prepare("SELECT COUNT(*) FROM applications WHERE vacancy_id = :id AND status = 'Активна'");
    q.bindValue(":id", id);
    q.exec();
    if (q.next() && q.value(0).toInt() > 0) {
        error = "Нельзя снять вакансию, пока есть активные отклики! Сначала обработайте заявки.";
        return false;
    }

    q.prepare("DELETE FROM applications WHERE vacancy_id = :id");
    q.bindValue(":id", id);
    q.exec();

    q.prepare("DELETE FROM vacancies WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec();
}