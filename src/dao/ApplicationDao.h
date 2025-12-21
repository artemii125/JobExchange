#ifndef APPLICATIONDAO_H
#define APPLICATIONDAO_H

#include <QString>

class ApplicationDao {
public:
    struct ApplicationData {
    int id;
    int applicantId;
    int vacancyId;
    QString status;
    QString applicantName;
    QString vacancyTitle;
};
    bool addApplication(int applicantId, int vacancyId, QString& error);
    bool updateStatus(int applicationId, const QString& status);
    bool removeApplication(int id, QString& error);
};

#endif // APPLICATIONDAO_H