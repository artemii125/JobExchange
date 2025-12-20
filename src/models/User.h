#ifndef USER_H
#define USER_H
#include <QString>

struct User {
    int id;
    QString login;
    QString role; // "admin" или "user"
};

#endif // USER_H