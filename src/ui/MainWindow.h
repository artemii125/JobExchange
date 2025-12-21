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
    explicit MainWindow(const QString &role, QWidget *parent = nullptr);

private slots:
    void refreshData();
    void onAddClicked();
    void onDeleteClicked();
    void onRespondClicked(); // Кнопка "Откликнуться"
    void onAcceptApplication();
    void onTabChanged(int index);

private:
    void setupUi();
    void setupModels();
    void updateButtonsState(int tabIndex);

    QString currentRole;

    QTabWidget *tabWidget;
    QTableView *tableView;
    QPushButton *btnAdd;
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