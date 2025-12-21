#include "LoginDialog.h"
#include "../dao/UserDao.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Вход в систему");
    setFixedSize(400, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 20, 30, 20);
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    loginEdit = new QLineEdit(this);
    formLayout->addRow(new QLabel("Логин:"), loginEdit);
    
    passEdit = new QLineEdit(this);
    passEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow(new QLabel("Пароль:"), passEdit);

    layout->addLayout(formLayout);

    QPushButton *btnLogin = new QPushButton("Войти", this);
    QPushButton *btnReg = new QPushButton("Регистрация", this);

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