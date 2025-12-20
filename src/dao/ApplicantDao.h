#ifndef APPLICANTDAO_H
#define APPLICANTDAO_H

#include <QString>
#include <QDate>

class ApplicantDao {
public:
    struct ApplicantData {
        QString fullName;
        QDate birthDate;
        QString phone;
        QString email;
        QString specialty;
        int experience;
        double salary;
    };

    bool addApplicant(const ApplicantData& data, QString& error);
    bool removeApplicant(int id, QString& error);
};

#endif // APPLICANTDAO_H