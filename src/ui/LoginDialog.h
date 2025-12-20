#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getUserRole() const { return userRole; }

private slots:
    void onLogin();
    void onRegister();

private:
    QLineEdit *loginEdit;
    QLineEdit *passEdit;
    QString userRole;
};

#endif // LOGINDIALOG_H