#include "LoginDialog.h"
#include "../dao/UserDao.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Вход в систему");
    setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Логин");
    
    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("Пароль");
    passEdit->setEchoMode(QLineEdit::Password);

    QPushButton *btnLogin = new QPushButton("Войти", this);
    QPushButton *btnReg = new QPushButton("Регистрация", this);

    layout->addWidget(new QLabel("Добро пожаловать"));
    layout->addWidget(loginEdit);
    layout->addWidget(passEdit);
    layout->addWidget(btnLogin);
    layout->addWidget(btnReg);

    // Связываем кнопки с функциями
    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnReg, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

void LoginDialog::onLogin() {
    UserDao dao;
    auto res = dao.login(loginEdit->text(), passEdit->text());
    if (res.success) {
        userRole = res.role;
        accept(); // Закрыть диалог с успехом
    } else {
        QMessageBox::warning(this, "Ошибка", res.error);
    }
}

void LoginDialog::onRegister() {
    UserDao dao;
    auto res = dao.registerUser(loginEdit->text(), passEdit->text());
    if (res.success) {
        QMessageBox::information(this, "Успех", "Аккаунт создан! Теперь войдите.");
    } else {
        QMessageBox::critical(this, "Ошибка", res.error);
    }
}