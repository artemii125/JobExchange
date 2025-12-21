#include "VacancyDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>

VacancyDialog::VacancyDialog(Vacancy* vacancy,
                             CompanyService* companyService,
                             SpecialtyRepository* specialtyRepo,
                             QWidget *parent)
    : QDialog(parent),
      vacancy_(vacancy),
      companyService_(companyService),
      specialtyRepo_(specialtyRepo),
      isEditMode_(vacancy != nullptr) {
    
    setWindowTitle(isEditMode_ ? "Edit Vacancy" : "Add Vacancy");
    setModal(true);
    resize(600, 650);
    
    setupUi();
    loadCompanies();
    loadSpecialties();
    
    if (isEditMode_) {
        loadData();
    }
}

void VacancyDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Основная информация
    QGroupBox* mainGroup = new QGroupBox("Vacancy Information");
    QFormLayout* mainForm = new QFormLayout(mainGroup);
    
    companyCombo_ = new QComboBox();
    mainForm->addRow("Company*:", companyCombo_);
    
    positionEdit_ = new QLineEdit();
    positionEdit_->setPlaceholderText("e.g., Senior C++ Developer");
    mainForm->addRow("Position*:", positionEdit_);
    
    specialtyCombo_ = new QComboBox();
    specialtyCombo_->addItem(" Не выбрано ", 0);
    mainForm->addRow("Specialty:", specialtyCombo_);
    
    salarySpinBox_ = new QDoubleSpinBox();
    salarySpinBox_->setRange(0, 10000000);
    salarySpinBox_->setDecimals(2);
    salarySpinBox_->setSuffix(" ₽");
    salarySpinBox_->setSingleStep(1000);
    mainForm->addRow("Salary:", salarySpinBox_);
    
    statusCombo_ = new QComboBox();
    statusCombo_->addItem("Active", "active");
    statusCombo_->addItem("Closed", "closed");
    statusCombo_->addItem("Filled", "filled");
    mainForm->addRow("Status:", statusCombo_);
    
    postedDateEdit_ = new QDateEdit();
    postedDateEdit_->setCalendarPopup(true);
    postedDateEdit_->setDate(QDate::currentDate());
    postedDateEdit_->setDisplayFormat("dd.MM.yyyy");
    mainForm->addRow("Posted Date:", postedDateEdit_);
    
    mainLayout->addWidget(mainGroup);
    
    // Требования
    QGroupBox* reqGroup = new QGroupBox("Requirements");
    QVBoxLayout* reqLayout = new QVBoxLayout(reqGroup);
    
    requirementsEdit_ = new QTextEdit();
    requirementsEdit_->setPlaceholderText("Experience, skills, qualifications...");
    requirementsEdit_->setMaximumHeight(120);
    reqLayout->addWidget(requirementsEdit_);
    
    mainLayout->addWidget(reqGroup);
    
    // Описание
    QGroupBox* descGroup = new QGroupBox("Description");
    QVBoxLayout* descLayout = new QVBoxLayout(descGroup);
    
    descriptionEdit_ = new QTextEdit();
    descriptionEdit_->setPlaceholderText("Job description, responsibilities...");
    descriptionEdit_->setMaximumHeight(120);
    descLayout->addWidget(descriptionEdit_);
    
    mainLayout->addWidget(descGroup);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* saveButton = new QPushButton("Save");
    saveButton->setDefault(true);
    saveButton->setMinimumWidth(100);
    
    QPushButton* cancelButton = new QPushButton("Cancel");
    cancelButton->setMinimumWidth(100);
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    
    mainLayout->addLayout(buttonLayout);
    
    connect(saveButton, &QPushButton::clicked, this, &VacancyDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &VacancyDialog::onCancel);
}

void VacancyDialog::loadCompanies() {
    auto companies = companyService_->getAllCompanies();
    
    for (const auto& company : companies) {
        companyCombo_->addItem(company.name(), company.id());
    }
}

void VacancyDialog::loadSpecialties() {
    auto specialties = specialtyRepo_->findAll();
    
    for (const auto& specialty : specialties) {
        specialtyCombo_->addItem(specialty.name(), specialty.id());
    }
}

void VacancyDialog::loadData() {
    if (!vacancy_) return;
    
    int companyIndex = companyCombo_->findData(vacancy_->companyId());
    if (companyIndex >= 0) {
        companyCombo_->setCurrentIndex(companyIndex);
    }
    
    positionEdit_->setText(vacancy_->position());
    
    int specialtyIndex = specialtyCombo_->findData(vacancy_->specialtyId());
    if (specialtyIndex >= 0) {
        specialtyCombo_->setCurrentIndex(specialtyIndex);
    }
    
    salarySpinBox_->setValue(vacancy_->salary());
    
    int statusIndex = statusCombo_->findData(vacancy_->status());
    if (statusIndex >= 0) {
        statusCombo_->setCurrentIndex(statusIndex);
    }
    
    postedDateEdit_->setDate(vacancy_->postedDate());
    requirementsEdit_->setPlainText(vacancy_->requirements());
    descriptionEdit_->setPlainText(vacancy_->description());
}

Vacancy VacancyDialog::getVacancy() const {
    Vacancy vacancy;
    
    if (isEditMode_ && vacancy_) {
        vacancy.setId(vacancy_->id());
        vacancy.setCreatedAt(vacancy_->createdAt());
    }
    
    vacancy.setCompanyId(companyCombo_->currentData().toInt());
    vacancy.setPosition(positionEdit_->text().trimmed());
    vacancy.setSpecialtyId(specialtyCombo_->currentData().toInt());
    vacancy.setSalary(salarySpinBox_->value());
    vacancy.setStatus(statusCombo_->currentData().toString());
    vacancy.setPostedDate(postedDateEdit_->date());
    vacancy.setRequirements(requirementsEdit_->toPlainText().trimmed());
    vacancy.setDescription(descriptionEdit_->toPlainText().trimmed());
    vacancy.setUpdatedAt(QDateTime::currentDateTime());
    
    return vacancy;
}

void VacancyDialog::onSave() {
    if (companyCombo_->currentData().toInt() == 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a company");
        companyCombo_->setFocus();
        return;
    }
    
    QString position = positionEdit_->text().trimmed();
    if (position.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Position is required");
        positionEdit_->setFocus();
        return;
    }
    
    accept();
}

void VacancyDialog::onCancel() {
    reject();
}
