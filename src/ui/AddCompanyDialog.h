#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

class AddCompanyDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddCompanyDialog(QWidget *parent = nullptr);
private slots:
    void onSave();
private:
    QLineEdit *nameEdit;
    QLineEdit *innEdit;
    QLineEdit *phoneEdit;
    QLineEdit *contactEdit;
    QTextEdit *addressEdit;
};