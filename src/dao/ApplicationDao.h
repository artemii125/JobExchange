#ifndef APPLICATIONDAO_H
#define APPLICATIONDAO_H

#include <QString>

class ApplicationDao {
public:
    bool addApplication(int applicantId, int vacancyId, QString& error);
};

#endif // APPLICATIONDAO_H