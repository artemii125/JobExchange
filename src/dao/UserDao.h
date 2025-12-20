#ifndef USERDAO_H
#define USERDAO_H

#include <QString>

class UserDao {
public:
    struct AuthResult {
        bool success;
        QString role; // "admin" или "user"
        QString error;
    };

    AuthResult login(const QString& login, const QString& password);
    AuthResult registerUser(const QString& login, const QString& password);

private:
    QString hashPassword(const QString& password);
};

#endif // USERDAO_H