#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "../dao/CompanyDao.h"

class AddCompanyDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddCompanyDialog(QWidget *parent = nullptr);
    void setCompanyData(const CompanyDao::CompanyData &data);
    CompanyDao::CompanyData getCompanyData() const;
private slots:
    void onSave();
private:
    QLineEdit *nameEdit;
    QLineEdit *innEdit;
    QLineEdit *phoneEdit;
    QLineEdit *contactEdit;
    QTextEdit *addressEdit;
};