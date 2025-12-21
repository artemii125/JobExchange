#include "AddApplicantDialog.h"
#include "../dao/ApplicantDao.h"
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
    birthEdit = new QDateEdit(this); 
    birthEdit->setDisplayFormat("dd.MM.yyyy");
    birthEdit->setCalendarPopup(true);
    birthEdit->setDate(QDate::currentDate().addYears(-18));
    layout->addWidget(birthEdit);

    layout->addWidget(new QLabel("Телефон:"));
    //phoneEdit = new QLineEdit;
    phoneEdit = new QLineEdit(this); 
    phoneEdit->setInputMask("+7(999)999-99-99;_");
    new PhoneInputFixer(phoneEdit);
    layout->addWidget(phoneEdit);

    layout->addWidget(new QLabel("Email:"));
    emailEdit = new QLineEdit; layout->addWidget(emailEdit);

    layout->addWidget(new QLabel("Специальность:"));
    specialtyEdit = new QLineEdit; layout->addWidget(specialtyEdit);

    layout->addWidget(new QLabel("Опыт работы (лет):"));
    expEdit = new QSpinBox; layout->addWidget(expEdit);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddApplicantDialog::onSave);
}

void AddApplicantDialog::onSave() {
    
    ApplicantDao::ApplicantData data;

    QString fio = fioEdit->text().trimmed();
    QDate birthDate = birthEdit->date();

    if(fioEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ФИО!");
        return;
    }

    QDate currentDate = QDate::currentDate();
    int age = birthDate.daysTo(currentDate) / 365; 
    if (age < 16) {
        QMessageBox::warning(this, "Ошибка", "По ТК РФ работать можно только с 16 лет!");
        return;
    }

    if (phoneEdit->text().contains('_')) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер телефона!");
        return;
    }

    ApplicantDao dao;

    if (dao.exists(data)) {
        QMessageBox::warning(this, "Ошибка", "Соискатель с такими данными (ФИО, Телефон или Email) уже существует!");
        return;
    }
    
    QString error;
    if(dao.addApplicant(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}

void AddApplicantDialog::setApplicantData(const ApplicantDao::ApplicantData &data) {
    fioEdit->setText(data.fullName);
    birthEdit->setDate(data.birthDate);
    phoneEdit->setText(data.phone);
    emailEdit->setText(data.email);
    specialtyEdit->setText(data.specialty);
    expEdit->setValue(data.experience);
}

ApplicantDao::ApplicantData AddApplicantDialog::getApplicantData() const {
    ApplicantDao::ApplicantData data;
    data.fullName = fioEdit->text();
    data.birthDate = birthEdit->date();
    data.phone = phoneEdit->text();
    data.email = emailEdit->text();
    data.specialty = specialtyEdit->text();
    data.experience = expEdit->value();
    return data;
}