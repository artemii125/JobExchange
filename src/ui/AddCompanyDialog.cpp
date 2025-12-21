#include "AddCompanyDialog.h"
#include "../dao/CompanyDao.h" // DAO
#include "../core/Utils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpressionValidator>

AddCompanyDialog::AddCompanyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Добавить компанию");
    resize(400, 500);
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Название*:"));
    nameEdit = new QLineEdit; layout->addWidget(nameEdit);

    layout->addWidget(new QLabel("ИНН (10-12 цифр):"));
    innEdit = new QLineEdit; 
    innEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\d{10,12}"), this));
    layout->addWidget(innEdit);

    layout->addWidget(new QLabel("Телефон*:"));
    phoneEdit = new QLineEdit(this);
    phoneEdit->setInputMask("+7(999)999-99-99;_");
    new PhoneInputFixer(phoneEdit);
    layout->addWidget(phoneEdit);

    layout->addWidget(new QLabel("Контактное лицо:"));
    contactEdit = new QLineEdit; layout->addWidget(contactEdit);

    layout->addWidget(new QLabel("Адрес:"));
    addressEdit = new QTextEdit; layout->addWidget(addressEdit);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddCompanyDialog::onSave);
}

void AddCompanyDialog::onSave() {
    CompanyDao::CompanyData data;
    data.name = Utils::formatTitleCase(nameEdit->text().trimmed());
    data.inn = innEdit->text().trimmed();
    data.phone = phoneEdit->text();
    data.contact = Utils::formatTitleCase(contactEdit->text().trimmed());
    data.address = addressEdit->toPlainText().trimmed();
    if(data.name.isEmpty() || data.phone.contains('_')) {
        QMessageBox::warning(this, "Ошибка", "Заполните обязательные поля (Название и телефон)!");
        return;
    }

    CompanyDao dao;
    if (dao.exists(data)) {
        QMessageBox::warning(this, "Ошибка", "Компания с такими данными (Название, ИНН, адрес, телефон или контактное лицо) уже существует!");
        return;
    }

    QString error;
    if(dao.addCompany(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}

void AddCompanyDialog::setCompanyData(const CompanyDao::CompanyData &data) {
    nameEdit->setText(data.name);
    innEdit->setText(data.inn);
    addressEdit->setPlainText(data.address);
    phoneEdit->setText(data.phone);
    contactEdit->setText(data.contact);
}

CompanyDao::CompanyData AddCompanyDialog::getCompanyData() const {
    CompanyDao::CompanyData data;
    data.name = nameEdit->text();
    data.inn = innEdit->text();
    data.address = addressEdit->toPlainText();
    data.phone = phoneEdit->text();
    data.contact = contactEdit->text();
    return data;
}