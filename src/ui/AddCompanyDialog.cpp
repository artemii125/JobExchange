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
    phoneEdit = new QLineEdit;
    phoneEdit->setInputMask("+7(999)999-99-99;_");
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
    if(nameEdit->text().isEmpty() || phoneEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните обязательные поля!");
        return;
    }

    CompanyDao::CompanyData data;
    data.name = Utils::formatTitleCase(nameEdit->text());
    data.inn = innEdit->text();
    data.phone = phoneEdit->text();
    data.contact = Utils::formatTitleCase(contactEdit->text());
    data.address = addressEdit->toPlainText();

    CompanyDao dao;
    QString error;
    if(dao.addCompany(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}