#include "RegisterDialog.h"
#include "../core/DatabaseManager.h"
#include "../core/QueryLoader.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QSlider>
#include <QValidator>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
}

void RegisterDialog::setupUi() {
    setWindowTitle("Регистрация");
    setMinimumWidth(400);

    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout;

    loginEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    // Кнопка показа/скрытия пароля
    auto *passwordLayout = new QHBoxLayout;
    auto *showPasswordBtn = new QPushButton("👁");
    showPasswordBtn->setMaximumWidth(30);
    showPasswordBtn->setCheckable(true);
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addWidget(showPasswordBtn);
    
    typeCombo = new QComboBox;
    typeCombo->addItems({"Компания", "Соискатель"});

    formLayout->addRow("Логин:", loginEdit);
    formLayout->addRow("Пароль:", passwordLayout);
    formLayout->addRow("Тип:", typeCombo);
    
    connect(showPasswordBtn, &QPushButton::toggled, [this](bool checked) {
        passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    mainLayout->addLayout(formLayout);

    profileStack = new QStackedWidget;

    // Форма компании
    auto *companyWidget = new QWidget;
    auto *companyLayout = new QFormLayout(companyWidget);
    companyNameEdit = new QLineEdit;
    innEdit = new QLineEdit;
    innEdit->setMaxLength(12);
    innEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,12}"), innEdit));
    addressEdit = new QLineEdit;
    companyPhoneEdit = new QLineEdit;
    companyPhoneEdit->setInputMask("+7(999)999-99-99;_");
    companyPhoneEdit->setCursorPosition(3); // Устанавливаем курсор после +7(
    
    // Обработчик клика для установки курсора после +7(
    connect(companyPhoneEdit, &QLineEdit::selectionChanged, [this]() {
        if (companyPhoneEdit->cursorPosition() < 3) {
            companyPhoneEdit->setCursorPosition(3);
        }
    });
    contactPersonEdit = new QLineEdit;
    companyLayout->addRow("Название*:", companyNameEdit);
    companyLayout->addRow("ИНН*:", innEdit);
    companyLayout->addRow("Адрес:", addressEdit);
    companyLayout->addRow("Телефон*:", companyPhoneEdit);
    companyLayout->addRow("Контактное лицо*:", contactPersonEdit);
    profileStack->addWidget(companyWidget);

    // Форма соискателя
    auto *applicantWidget = new QWidget;
    auto *applicantLayout = new QFormLayout(applicantWidget);
    fullNameEdit = new QLineEdit;
    birthDateEdit = new QDateEdit;
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    birthDateEdit->setDate(QDate::currentDate().addYears(-18));
    applicantPhoneEdit = new QLineEdit;
    applicantPhoneEdit->setInputMask("+7(999)999-99-99;_");
    applicantPhoneEdit->setCursorPosition(3); // Устанавливаем курсор после +7(
    
    // Обработчик клика для установки курсора после +7(
    connect(applicantPhoneEdit, &QLineEdit::selectionChanged, [this]() {
        if (applicantPhoneEdit->cursorPosition() < 3) {
            applicantPhoneEdit->setCursorPosition(3);
        }
    });
    emailEdit = new QLineEdit;
    specialtyEdit = new QLineEdit;
    
    // Опыт работы с ползунком
    auto *expLayout = new QHBoxLayout;
    experienceEdit = new QLineEdit;
    experienceEdit->setText("0");
    experienceEdit->setValidator(new QIntValidator(0, 50, experienceEdit));
    experienceSlider = new QSlider(Qt::Horizontal);
    experienceSlider->setMinimum(0);
    experienceSlider->setMaximum(50);
    experienceSlider->setValue(0);
    experienceSlider->setSingleStep(1);
    expLayout->addWidget(experienceEdit);
    expLayout->addWidget(experienceSlider);
    
    // Зарплата с ползунком
    auto *salaryLayout = new QHBoxLayout;
    salaryEdit = new QLineEdit;
    salaryEdit->setText("30000");
    salaryEdit->setValidator(new QIntValidator(0, 500000, salaryEdit));
    salarySlider = new QSlider(Qt::Horizontal);
    salarySlider->setMinimum(0);
    salarySlider->setMaximum(500000);
    salarySlider->setValue(30000);
    salarySlider->setSingleStep(1000);
    salaryLayout->addWidget(salaryEdit);
    salaryLayout->addWidget(salarySlider);
    applicantLayout->addRow("ФИО*:", fullNameEdit);
    applicantLayout->addRow("Дата рождения:", birthDateEdit);
    applicantLayout->addRow("Телефон*:", applicantPhoneEdit);
    applicantLayout->addRow("Email*:", emailEdit);
    applicantLayout->addRow("Специальность*:", specialtyEdit);
    applicantLayout->addRow("Опыт (лет):", expLayout);
    applicantLayout->addRow("Желаемая ЗП*:", salaryLayout);
    profileStack->addWidget(applicantWidget);

    mainLayout->addWidget(profileStack);

    auto *btnLayout = new QHBoxLayout;
    auto *btnRegister = new QPushButton("Зарегистрироваться");
    auto *btnCancel = new QPushButton("Отмена");
    btnLayout->addWidget(btnRegister);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RegisterDialog::onTypeChanged);
    connect(btnRegister, &QPushButton::clicked, this, &RegisterDialog::onRegister);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    
    // Синхронизация ползунков
    connect(experienceSlider, &QSlider::valueChanged, [this](int value) {
        experienceEdit->setText(QString::number(value));
    });
    connect(experienceEdit, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok;
        int value = text.toInt(&ok);
        if (ok && value >= 0 && value <= 50) {
            experienceSlider->setValue(value);
        }
    });
    
    connect(salarySlider, &QSlider::valueChanged, [this](int value) {
        int roundedValue = (value / 1000) * 1000;
        salaryEdit->setText(QString::number(roundedValue));
    });
    connect(salaryEdit, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok;
        int value = text.toInt(&ok);
        if (ok && value >= 0 && value <= 500000) {
            salarySlider->setValue(value);
        }
    });
}

void RegisterDialog::onTypeChanged(int index) {
    profileStack->setCurrentIndex(index);
}

void RegisterDialog::onRegister() {
    // Базовая валидация
    if (loginEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин!");
        return;
    }
    
    if (passwordEdit->text().length() < 4) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 4 символа!");
        return;
    }

    // Валидация в зависимости от типа
    if (typeCombo->currentIndex() == 0) { // Компания
        if (!validateCompanyData()) return;
    } else { // Соискатель
        if (!validateApplicantData()) return;
    }

    if (registerUser()) {
        QMessageBox::information(this, "Успех", "Регистрация завершена");
        accept();
    }
}

bool RegisterDialog::validateCompanyData() {
    if (companyNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название компании!");
        return false;
    }
    
    QString inn = innEdit->text().trimmed();
    if (inn.isEmpty() || inn.length() < 10 || inn.length() > 12) {
        QMessageBox::warning(this, "Ошибка", "ИНН должен содержать 10-12 цифр!");
        return false;
    }
    
    // Проверка что ИНН содержит только цифры
    for (QChar c : inn) {
        if (!c.isDigit()) {
            QMessageBox::warning(this, "Ошибка", "ИНН должен содержать только цифры!");
            return false;
        }
    }
    
    if (companyPhoneEdit->text().contains('_') || companyPhoneEdit->text().length() < 16) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер телефона!");
        return false;
    }
    
    if (contactPersonEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите контактное лицо!");
        return false;
    }
    
    return true;
}

bool RegisterDialog::validateApplicantData() {
    if (fullNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ФИО!");
        return false;
    }
    
    // Проверка возраста
    QDate currentDate = QDate::currentDate();
    int age = birthDateEdit->date().daysTo(currentDate) / 365;
    if (age < 16) {
        QMessageBox::warning(this, "Ошибка", "По ТК РФ работать можно только с 16 лет!");
        return false;
    }
    
    if (applicantPhoneEdit->text().contains('_') || applicantPhoneEdit->text().length() < 16) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер телефона!");
        return false;
    }
    
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите email!");
        return false;
    }
    
    // Простая проверка email
    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный email!");
        return false;
    }
    
    if (specialtyEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите специальность!");
        return false;
    }
    
    // Проверка зарплаты
    bool ok;
    double salary = salaryEdit->text().toDouble(&ok);
    if (!ok || salary < 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректную желаемую зарплату!");
        return false;
    }
    
    return true;
}

bool RegisterDialog::registerUser() {
    QSqlDatabase db = DatabaseManager::instance().db();
    db.transaction();

    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString passwordHash = hash.toHex();
    
    userType = (typeCombo->currentIndex() == 0) ? "company" : "applicant";

    // Создаем пользователя
    QSqlQuery query(db);
    QString sql = QueryLoader::instance().getQuery("InsertUser");
    query.prepare(sql);
    query.addBindValue(login);
    query.addBindValue(passwordHash);
    query.addBindValue(userType);
    
    if (!query.exec() || !query.next()) {
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось создать пользователя: " + query.lastError().text());
        return false;
    }
    
    userId = query.value(0).toInt();

    // Создаем профиль
    if (userType == "company") {
        sql = QueryLoader::instance().getQuery("InsertCompany");
        query.prepare(sql);
        query.addBindValue(userId);
        query.addBindValue(companyNameEdit->text());
        query.addBindValue(innEdit->text());
        query.addBindValue(addressEdit->text());
        query.addBindValue(companyPhoneEdit->text());
        query.addBindValue(contactPersonEdit->text());
    } else {
        sql = QueryLoader::instance().getQuery("InsertApplicant");
        query.prepare(sql);
        query.addBindValue(userId);
        query.addBindValue(fullNameEdit->text());
        query.addBindValue(birthDateEdit->date());
        query.addBindValue(applicantPhoneEdit->text());
        query.addBindValue(emailEdit->text());
        query.addBindValue(specialtyEdit->text());
        query.addBindValue(experienceEdit->text().toInt());
        query.addBindValue(salaryEdit->text().toDouble());
    }

    if (!query.exec() || !query.next()) {
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось создать профиль: " + query.lastError().text());
        return false;
    }

    profileId = query.value(0).toInt();

    // Обновляем profile_id в users
    sql = QueryLoader::instance().getQuery("UpdateUserProfile");
    query.prepare(sql);
    query.addBindValue(profileId);
    query.addBindValue(userId);
    
    if (!query.exec()) {
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось обновить профиль: " + query.lastError().text());
        return false;
    }

    db.commit();
    return true;
}
