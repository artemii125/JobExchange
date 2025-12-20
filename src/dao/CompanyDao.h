#ifndef COMPANYDAO_H
#define COMPANYDAO_H

#include <QString>

class CompanyDao {
public:
    struct CompanyData {
        QString name;
        QString inn;
        QString phone;
        QString contact;
        QString address;
    };

    bool addCompany(const CompanyData& data, QString& error);
    bool removeCompany(int id, QString& error);
};

#endif // COMPANYDAO_H