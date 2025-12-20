#include "AddVacancyDialog.h"
#include "../dao/VacancyDao.h" // DAO
#include "../core/DatabaseManager.h"
#include "../core/Utils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery> 
#include <QMessageBox>

AddVacancyDialog::AddVacancyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Добавить вакансию");
    resize(400, 500);
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Должность*:"));
    positionEdit = new QLineEdit; layout->addWidget(positionEdit);

    layout->addWidget(new QLabel("Компания*:"));
    companyCombo = new QComboBox;
    // Заполняем комбобокс данными для выбора (это SELECT, он безопасен здесь)
    QSqlQuery q(DatabaseManager::instance().db());
    q.exec("SELECT id, name FROM companies");
    while(q.next()) {
        companyCombo->addItem(q.value("name").toString(), q.value("id"));
    }
    layout->addWidget(companyCombo);

    layout->addWidget(new QLabel("Специальность:"));
    specialtyEdit = new QLineEdit; layout->addWidget(specialtyEdit);

    layout->addWidget(new QLabel("Зарплата:"));
    salaryEdit = new QDoubleSpinBox; 
    salaryEdit->setMaximum(1000000);
    layout->addWidget(salaryEdit);
    
    layout->addWidget(new QLabel("Статус:"));
    statusCombo = new QComboBox;
    statusCombo->addItems({"Активна", "Закрыта"});
    layout->addWidget(statusCombo);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddVacancyDialog::onSave);
}

void AddVacancyDialog::onSave() {
    if(positionEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите должность!");
        return;
    }

    VacancyDao::VacancyData data;
    data.companyId = companyCombo->currentData().toInt();
    data.position = Utils::formatTitleCase(positionEdit->text());
    data.specialty = Utils::formatTitleCase(specialtyEdit->text());
    data.salary = salaryEdit->value();
    data.status = statusCombo->currentText();

    VacancyDao dao;
    QString error;
    if(dao.addVacancy(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}