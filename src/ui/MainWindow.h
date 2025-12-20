#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTabWidget>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString &role, QWidget *parent = nullptr);

private slots:
    void refreshData();
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onRespondClicked(); // Кнопка "Откликнуться"
    void onTabChanged(int index);

private:
    void setupUi();
    void setupModels();
    void updateButtonsState(int tabIndex);

    QString currentRole;

    QTabWidget *tabWidget;
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QPushButton *btnRespond;
    QPushButton *btnRefresh;
    
    // Модели
    QSqlTableModel *modelApplicants;
    QSqlTableModel *modelCompanies;
    QSqlQueryModel *modelVacancies;    
    QSqlQueryModel *modelApplications; 
    
    QSortFilterProxyModel *proxyModel;
};

#endif // MAINWINDOW_H