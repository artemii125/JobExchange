#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getUserRole() const { return userRole; }
    int getUserId() const { return userId; }
    QString getUserType() const { return userType; }
    int getProfileId() const { return profileId; }

private slots:
    void onLogin();
    void onRegister();

private:
    QLineEdit *loginEdit;
    QLineEdit *passEdit;
    QString userRole;
    int userId = -1;
    QString userType;
    int profileId = -1;
};

#endif // LOGINDIALOG_H