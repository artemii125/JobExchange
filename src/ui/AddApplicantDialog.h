#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include "../dao/ApplicantDao.h"

class AddApplicantDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddApplicantDialog(QWidget *parent = nullptr);
    void setApplicantData(const ApplicantDao::ApplicantData &data);
    ApplicantDao::ApplicantData getApplicantData() const;
private slots:
    void onSave();
    void onSalarySliderChanged(int value);
    void onSalaryEditChanged();
private:
    QLineEdit *fioEdit;
    QDateEdit *birthEdit;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    QLineEdit *specialtyEdit;
    QSpinBox *expEdit;
    QSlider *expSlider;
    QLineEdit *salaryEdit;
    QSlider *salarySlider;
};