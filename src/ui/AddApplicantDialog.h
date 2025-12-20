#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

class AddApplicantDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddApplicantDialog(QWidget *parent = nullptr);
private slots:
    void onSave();
private:
    QLineEdit *fioEdit;
    QDateEdit *birthEdit;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    QLineEdit *specialtyEdit;
    QSpinBox *expEdit;
    QDoubleSpinBox *salaryEdit;
};