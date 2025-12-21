#ifndef COMPANYDAO_H
#define COMPANYDAO_H

#include <QString>

class CompanyDao {
public:
    struct CompanyData {
        int id;
        QString name;
        QString inn;
        QString phone;
        QString contact;
        QString address;
    };

    bool addCompany(const CompanyData& data, QString& error);
    bool removeCompany(int id, QString& error);
    bool exists(const CompanyData& data);
};

#endif // COMPANYDAO_H