#ifndef VACANCYDAO_H
#define VACANCYDAO_H

#include <QString>
#include <QVector>

class VacancyDao {
public:
    struct VacancyData {
        int id;
        int companyId;
        QString position;
        QString specialty;
        double salary;
        QString status;
        QString companyName;
    };

    bool addVacancy(const VacancyData& data, QString& error);
    bool removeVacancy(int id, QString& error);
    bool exists(const VacancyData& data);
    //QVector<VacancyData> getAllVacancies();
};

#endif // VACANCYDAO_H