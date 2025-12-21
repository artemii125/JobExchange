#ifndef COMPANYDIALOG_H
#define COMPANYDIALOG_H

#include <QDialog>
#include "Company.h"

class QLineEdit;
class QTextEdit;

class CompanyDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CompanyDialog(Company* company, QWidget *parent = nullptr);
    
    Company getCompany() const;
    
private slots:
    void onSave();
    void onCancel();
    
private:
    void setupUi();
    void loadData();
    
    Company* company_;
    
    QLineEdit* nameEdit_;
    QLineEdit* innEdit_;
    QTextEdit* addressEdit_;
    QLineEdit* phoneEdit_;
    QLineEdit* contactPersonEdit_;
    
    bool isEditMode_;
};

#endif // COMPANYDIALOG_H
