#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QStackedWidget>
#include <QSlider>
#include <QValidator>
#include <QRegularExpressionValidator>

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    int getUserId() const { return userId; }
    QString getUserType() const { return userType; }
    int getProfileId() const { return profileId; }

private slots:
    void onTypeChanged(int index);
    void onRegister();

private:
    void setupUi();
    bool registerUser();
    bool validateCompanyData();
    bool validateApplicantData();

    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QComboBox *typeCombo;
    QStackedWidget *profileStack;
    
    //поля компании
    QLineEdit *companyNameEdit;
    QLineEdit *innEdit;
    QLineEdit *addressEdit;
    QLineEdit *companyPhoneEdit;
    QLineEdit *contactPersonEdit;
    
    //поля соискателя
    QLineEdit *fullNameEdit;
    QDateEdit *birthDateEdit;
    QLineEdit *applicantPhoneEdit;
    QLineEdit *emailEdit;
    QLineEdit *specialtyEdit;
    QLineEdit *experienceEdit;
    QSlider *experienceSlider;
    QLineEdit *salaryEdit;
    QSlider *salarySlider;

    int userId = -1;
    QString userType;
    int profileId = -1;
};

#endif
