#ifndef APPLICANTDAO_H
#define APPLICANTDAO_H

#include <QString>
#include <QDate>

class ApplicantDao {
public:
    struct ApplicantData {
        int id;
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
    bool exists(const ApplicantData& data);
};

#endif // APPLICANTDAO_H