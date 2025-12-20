#include "AddApplicantDialog.h"
#include "../dao/ApplicantDao.h" // DAO
#include "../core/Utils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

AddApplicantDialog::AddApplicantDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Добавить соискателя");
    resize(400, 600);
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("ФИО*:"));
    fioEdit = new QLineEdit; layout->addWidget(fioEdit);

    layout->addWidget(new QLabel("Дата рождения:"));
    birthEdit = new QDateEdit; 
    birthEdit->setCalendarPopup(true);
    birthEdit->setDate(QDate::currentDate().addYears(-18));
    layout->addWidget(birthEdit);

    layout->addWidget(new QLabel("Телефон:"));
    phoneEdit = new QLineEdit; 
    phoneEdit->setInputMask("+7(999)999-99-99;_");
    layout->addWidget(phoneEdit);

    layout->addWidget(new QLabel("Email:"));
    emailEdit = new QLineEdit; layout->addWidget(emailEdit);

    layout->addWidget(new QLabel("Специальность:"));
    specialtyEdit = new QLineEdit; layout->addWidget(specialtyEdit);

    layout->addWidget(new QLabel("Опыт работы (лет):"));
    expEdit = new QSpinBox; layout->addWidget(expEdit);

    layout->addWidget(new QLabel("Желаемая зарплата:"));
    salaryEdit = new QDoubleSpinBox; 
    salaryEdit->setMaximum(1000000);
    layout->addWidget(salaryEdit);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddApplicantDialog::onSave);
}

void AddApplicantDialog::onSave() {
    if(fioEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ФИО!");
        return;
    }

    // Заполнение структуры данных
    ApplicantDao::ApplicantData data;
    data.fullName = Utils::formatTitleCase(fioEdit->text());
    data.birthDate = birthEdit->date();
    data.phone = phoneEdit->text();
    data.email = emailEdit->text();
    data.specialty = Utils::formatTitleCase(specialtyEdit->text());
    data.experience = expEdit->value();
    data.salary = salaryEdit->value();

    ApplicantDao dao;
    QString error;
    if(dao.addApplicant(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}