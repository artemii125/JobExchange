#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "../dao/VacancyDao.h"

class AddVacancyDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddVacancyDialog(QWidget *parent = nullptr);
    void setVacancyData(const VacancyDao::VacancyData &data);
    VacancyDao::VacancyData getVacancyData() const;
private slots:
    void onSave();
private:
    QLineEdit *positionEdit;
    QComboBox *companyCombo;
    QLineEdit *specialtyEdit;
    QDoubleSpinBox *salaryEdit;
    QComboBox *statusCombo;
};