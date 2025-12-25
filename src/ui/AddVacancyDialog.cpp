#include "AddVacancyDialog.h"
#include "../dao/VacancyDao.h" // DAO
#include "../core/DatabaseManager.h"
#include "../core/QueryLoader.h"
#include "../core/Utils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery> 
#include <QMessageBox>

AddVacancyDialog::AddVacancyDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
}

AddVacancyDialog::AddVacancyDialog(int companyId, QWidget *parent) : QDialog(parent) {
    setupUi(companyId);
}

void AddVacancyDialog::setupUi(int companyId) {
    setWindowTitle("Добавить вакансию");
    resize(400, 500);
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Показываем поле компании только если companyId не задан (для админов/сотрудников)
    if (companyId == -1) {
        layout->addWidget(new QLabel("Компания*:"));
        companyCombo = new QComboBox;
        
        QString sql = QueryLoader::instance().getQuery("GetAllCompaniesForCombo");
        QSqlQuery q(DatabaseManager::instance().db());
        q.prepare(sql);
        q.exec();
        
        while(q.next()) {
            int id = q.value("id").toInt();
            QString name = q.value("name").toString();
            companyCombo->addItem(name, id);
        }
        
        layout->addWidget(companyCombo);
    } else {
        // Для пользователей-компаний создаем скрытый комбобокс с их компанией
        companyCombo = new QComboBox;
        companyCombo->addItem("", companyId);
        companyCombo->setVisible(false);
    }

    layout->addWidget(new QLabel("Специальность*:"));
    specialtyEdit = new QLineEdit; layout->addWidget(specialtyEdit);

    layout->addWidget(new QLabel("Зарплата:"));
    salaryEdit = new QDoubleSpinBox; 
    salaryEdit->setMaximum(1000000);
    salaryEdit->setSingleStep(1000.0);
    layout->addWidget(salaryEdit);

    QPushButton *btnSave = new QPushButton("Добавить");
    btnSave->setStyleSheet("background-color: #2ecc71; color: white;");
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &AddVacancyDialog::onSave);
}

void AddVacancyDialog::onSave() {
    if(companyCombo->currentData().isNull()) {
        QMessageBox::warning(this, "Ошибка", "Выберите компанию!");
        return;
    }
    
    VacancyDao::VacancyData data;
    data.companyId = companyCombo->currentData().toInt();
    data.specialty = Utils::formatTitleCase(specialtyEdit->text());
    data.salary = salaryEdit->value();
    data.status = "Активна";

    if(data.specialty.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите специальность!");
        return;
    }
    VacancyDao dao;
    QString error;
    if(dao.addVacancy(data, error)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка БД", error);
    }
}

void AddVacancyDialog::setVacancyData(const VacancyDao::VacancyData &data) {
    specialtyEdit->setText(data.specialty);
    salaryEdit->setValue(data.salary);
}

VacancyDao::VacancyData AddVacancyDialog::getVacancyData() const {
    VacancyDao::VacancyData data;
    data.companyId = companyCombo->currentData().isNull() ? -1 : companyCombo->currentData().toInt();
    data.specialty = specialtyEdit->text();
    data.salary = salaryEdit->value();
    data.status = "Активна";
    return data;
}