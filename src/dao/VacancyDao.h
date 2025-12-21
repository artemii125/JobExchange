#ifndef VACANCYDAO_H
#define VACANCYDAO_H

#include <QString>

class VacancyDao {
public:
    struct VacancyData {
        int id;
        int companyId;
        QString position;
        QString specialty;
        double salary;
        QString status;
    };

    bool addVacancy(const VacancyData& data, QString& error);
    bool removeVacancy(int id, QString& error);
};

#endif // VACANCYDAO_H