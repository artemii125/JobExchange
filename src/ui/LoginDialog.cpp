#include "LoginDialog.h"
#include "RegisterDialog.h"
#include "../dao/UserDao.h"
#include "../core/DatabaseManager.h"
#include "../core/QueryLoader.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

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
    
    //кнопка показа/скрытия пароля
    auto *passwordLayout = new QHBoxLayout;
    auto *showPasswordBtn = new QPushButton("👁");
    showPasswordBtn->setMaximumWidth(30);
    showPasswordBtn->setCheckable(true);
    showPasswordBtn->setStyleSheet("QPushButton { color: gray; font-size: 16px; } QPushButton:checked { color: white; }");
    passwordLayout->addWidget(passEdit);
    passwordLayout->addWidget(showPasswordBtn);
    
    formLayout->addRow(new QLabel("Пароль:"), passwordLayout);
    
    connect(showPasswordBtn, &QPushButton::toggled, [this](bool checked) {
        passEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    layout->addLayout(formLayout);

    QPushButton *btnLogin = new QPushButton("Войти", this);
    QPushButton *btnReg = new QPushButton("Регистрация", this);

    layout->addWidget(btnLogin);
    layout->addWidget(btnReg);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnReg, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

void LoginDialog::onLogin() {
    QString login = loginEdit->text();
    QString password = passEdit->text();
    
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString passwordHash = hash.toHex();

    QSqlQuery query(DatabaseManager::instance().db());
    QString sql = QueryLoader::instance().getQuery("LoginUser");
    query.prepare(sql);
    query.addBindValue(login);
    query.addBindValue(passwordHash);

    if (query.exec() && query.next()) {
        userId = query.value(0).toInt();
        userRole = query.value(1).toString();
        userType = query.value(2).toString();
        profileId = query.value(3).toInt();
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

void LoginDialog::onRegister() {
    RegisterDialog dialog(this);
    dialog.exec();
}