#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTabWidget>
#include <QPushButton>
#include <QStyledItemDelegate>

class NumberDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit NumberDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    QString displayText(const QVariant &value, const QLocale &locale) const override {
        return QString::number(value.toDouble(), 'f', 0);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString &role, int userId, const QString &userType, int profileId, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshData();
    void onAddClicked();
    void onDeleteClicked();
    void onRespondClicked();
    void onUpdateStatusClicked();
    void onTabChanged(int index);
    void onExportData();
    void onImportData();
    void onLogout();

private:
    void setupUi();
    void setupModels();
    void updateButtonsState(int tabIndex);

    QString currentRole;
    int currentUserId;
    QString currentUserType;
    int currentProfileId;
    bool isLoggingOut = false;

    //кнопки
    QTabWidget *tabWidget;
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QPushButton *btnRespond;
    QPushButton *btnRefresh;
    QPushButton *btnUpdateStatus;
    QPushButton *btnExport;
    QPushButton *btnImport;
    QPushButton *btnLogout;
    
    //модели
    QSqlTableModel *modelApplicants;
    QSqlTableModel *modelCompanies;
    QSqlQueryModel *modelVacancies;    
    QSqlQueryModel *modelApplications; 
    
    QSortFilterProxyModel *proxyModel;
};

#endif // MAINWINDOW_H