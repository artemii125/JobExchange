#include "AddApplicantDialog.h"
#include "../dao/ApplicantDao.h"
#include "../core/Utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSlider>

AddApplicantDialog::AddApplicantDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Добавить соискателя");
    resize(400, 650);
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("ФИО*:"));
    fioEdit = new QLineEdit; layout->addWidget(fioEdit);

    layout->addWidget(new QLabel("Дата рождения:"));
    birthEdit = new QDateEdit(this); 
    birthEdit->setDisplayFormat("dd.MM.yyyy");
    birthEdit->setCalendarPopup(true);
    birthEdit->setDate(QDate::currentDate().addYears(-18));
    layout->addWidget(birthEdit);

    layout->addWidget(new QLabel("Телефон*:"));
    phoneEdit = new QLineEdit(this); 
    phoneEdit->setInputMask("+7(999)999-99-99;_");
    new PhoneInputFixer(phoneEdit);
    layout->addWidget(phoneEdit);

    layout->addWidget(new QLabel("Email*:"));
    emailEdit = new QLineEdit; layout->addWidget(emailEdit);

    layout->addWidget(new QLabel("Специальность*:"));
    specialtyEdit = new QLineEdit; layout->addWidget(specialtyEdit);

    layout->addWidget(new QLabel("Опыт работы (лет):"));
    
    //поле ввода опыта с ползунком
    QHBoxLayout *expLayout = new QHBoxLayout;
    expEdit = new QSpinBox; 
    expEdit->setMinimum(0);
    expEdit->setMaximum(50);
    expEdit->setValue(0);
    expSlider = new QSlider(Qt::Horizontal);
    expSlider->setMinimum(0);
    expSlider->setMaximum(50);
    expSlider->setValue(0);
    expSlider->setSingleStep(1);
    expLayout->addWidget(expEdit);
    expLayout->addWidget(expSlider);
    layout->addLayout(expLayout);

    layout->addWidget(new QLabel("Желаемая зарплата (руб.):"));
    
    //поле ввода зарплаты с ползунком
    QHBoxLayout *salaryLayout = new QHBoxLayout;
    salaryEdit = new QLineEdit;
    salaryEdit->setText("30000");
    salaryEdit->setValidator(new QIntValidator(0, 500000, salaryEdit));
    salarySlider = new QSlider(Qt::Horizontal);
    salarySlider->setMinimum(0);
    salarySlider->setMaximum(500000);
    salarySlider->setValue(30000);
    salarySlider->setSingleStep(1000);
    salarySlider->setPageStep(5000);
    salaryLayout->addWidget(salaryEdit);
    salaryLayout->addWidget(salarySlider);
    layout->addLayout(salaryLayout);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddApplicantDialog::onSave);
    connect(salarySlider, &QSlider::valueChanged, this, &AddApplicantDialog::onSalarySliderChanged);
    connect(salaryEdit, &QLineEdit::textChanged, this, &AddApplicantDialog::onSalaryEditChanged);
    
    //синхронизация ползунка опыта
    connect(expSlider, &QSlider::valueChanged, expEdit, QOverload<int>::of(&QSpinBox::setValue));
    connect(expEdit, QOverload<int>::of(&QSpinBox::valueChanged), expSlider, &QSlider::setValue);
}

void AddApplicantDialog::onSalarySliderChanged(int value) {
    //округляем до ближайшей тысячи
    int roundedValue = (value / 1000) * 1000;
    salaryEdit->setText(QString::number(roundedValue));
}

void AddApplicantDialog::onSalaryEditChanged() {
    bool ok;
    int value = salaryEdit->text().toInt(&ok);
    if (ok && value >= salarySlider->minimum() && value <= salarySlider->maximum()) {
        salarySlider->setValue(value);
    }
}

void AddApplicantDialog::onSave() {
    
    ApplicantDao::ApplicantData data = getApplicantData();


    if(data.fullName.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ФИО!");
        return;
    }

    QDate currentDate = QDate::currentDate();
    int age = data.birthDate.daysTo(currentDate) / 365; 
    if (age < 16) {
        QMessageBox::warning(this, "Ошибка", "По ТК РФ работать можно только с 16 лет!");
        return;
    }

    if (data.phone.isEmpty() || data.phone.contains('_') || data.phone.length() < 16) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер телефона!");
        return;
    }

    if (data.email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Email' является обязательным!");
        return;
    }

    if (data.specialty.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Специальность' является обязательным!");
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
    expSlider->setValue(data.experience);
    salaryEdit->setText(QString::number((int)data.salary));
    salarySlider->setValue((int)data.salary);
}

ApplicantDao::ApplicantData AddApplicantDialog::getApplicantData() const {
    ApplicantDao::ApplicantData data;
    data.fullName = fioEdit->text();
    data.birthDate = birthEdit->date();
    data.phone = phoneEdit->text();
    data.email = emailEdit->text();
    data.specialty = specialtyEdit->text();
    data.experience = expEdit->value();
    data.salary = salaryEdit->text().toDouble();
    return data;
}