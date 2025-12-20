#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

class AddVacancyDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddVacancyDialog(QWidget *parent = nullptr);
private slots:
    void onSave();
private:
    QLineEdit *positionEdit;
    QComboBox *companyCombo;
    QLineEdit *specialtyEdit;
    QDoubleSpinBox *salaryEdit;
    QComboBox *statusCombo;
};