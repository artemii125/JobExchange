#include "MainWindow.h"
// Диалоги
#include "AddApplicantDialog.h"
#include "AddCompanyDialog.h"
#include "AddVacancyDialog.h"
// Ядро и DAO
#include "../core/DatabaseManager.h"
#include "../dao/VacancyDao.h"
#include "../dao/CompanyDao.h"
#include "../dao/ApplicantDao.h"
#include "../dao/ApplicationDao.h"
// Qt
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>


MainWindow::MainWindow(const QString &role, QWidget *parent) 
    : QMainWindow(parent), currentRole(role) 
{
    setWindowTitle("Биржа Труда [Пользователь: " + role + "]");
    resize(1200, 700);
    setupModels();
    setupUi();
    onTabChanged(0);
}

void MainWindow::setupModels() {
    QSqlDatabase db = DatabaseManager::instance().db();

    // 1. Соискатели
    modelApplicants = new QSqlTableModel(this, db);
    modelApplicants->setTable("applicants");
    modelApplicants->setHeaderData(0, Qt::Horizontal, "ID");
    modelApplicants->setHeaderData(1, Qt::Horizontal, "ФИО");
    modelApplicants->setHeaderData(2, Qt::Horizontal, "Дата рождения");
    modelApplicants->setHeaderData(3, Qt::Horizontal, "Телефон");
    modelApplicants->setHeaderData(4, Qt::Horizontal, "Email");
    modelApplicants->setHeaderData(5, Qt::Horizontal, "Специальность");
    modelApplicants->setHeaderData(6, Qt::Horizontal, "Опыт");
    modelApplicants->select();

    // 2. Компании
    modelCompanies = new QSqlTableModel(this, db);
    modelCompanies->setTable("companies");
    modelCompanies->setHeaderData(0, Qt::Horizontal, "ID");
    modelCompanies->setHeaderData(1, Qt::Horizontal, "Название");
    modelCompanies->setHeaderData(2, Qt::Horizontal, "ИНН");
    modelCompanies->setHeaderData(3, Qt::Horizontal, "Адрес");
    modelCompanies->setHeaderData(4, Qt::Horizontal, "Телефон");
    modelCompanies->setHeaderData(5, Qt::Horizontal, "Контактное лицо");
    modelCompanies->select();

    // 3. Вакансии (JOIN)
    modelVacancies = new QSqlQueryModel(this);
    modelVacancies->setQuery(DatabaseManager::instance().getQuery("GetAllVacancies"), db);
    modelVacancies->setHeaderData(0, Qt::Horizontal, "ID");
    modelVacancies->setHeaderData(1, Qt::Horizontal, "Компания");
    modelVacancies->setHeaderData(2, Qt::Horizontal, "Специальность");
    modelVacancies->setHeaderData(3, Qt::Horizontal, "Зарплата");
    modelVacancies->setHeaderData(4, Qt::Horizontal, "Дата");

    // 4. Заявки (JOIN)
    modelApplications = new QSqlQueryModel(this);
    modelApplications->setQuery(DatabaseManager::instance().getQuery("GetAllApplications"), db);
    modelApplications->setHeaderData(0, Qt::Horizontal, "ID");
    modelApplications->setHeaderData(1, Qt::Horizontal, "ID Соискателя");
    modelApplications->setHeaderData(2, Qt::Horizontal, "Соискатель");
    modelApplications->setHeaderData(3, Qt::Horizontal, "Вакансия");
    modelApplications->setHeaderData(4, Qt::Horizontal, "Статус");
    modelApplications->setHeaderData(5, Qt::Horizontal, "Дата");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Вкладки
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget, "Соискатели");
    tabWidget->addTab(new QWidget, "Вакансии");
    tabWidget->addTab(new QWidget, "Компании");
    tabWidget->addTab(new QWidget, "Заявки");
    mainLayout->addWidget(tabWidget, 0);

    // Кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnAdd = new QPushButton("Добавить", this);
    btnDelete = new QPushButton("Удалить", this);
    btnRespond = new QPushButton("Откликнуться", this);
    btnRefresh = new QPushButton("Обновить", this);
    btnUpdateStatus = new QPushButton("Изменить статус", this);
    
    btnLayout->addWidget(btnUpdateStatus);
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnDelete);
    btnLayout->addWidget(btnRespond);
    btnLayout->addWidget(btnRefresh);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Таблица
    tableView = new QTableView(this);
    tableView->setModel(proxyModel);
    tableView->setSortingEnabled(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);
    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setSectionsClickable(true);
    tableView->horizontalHeader()->setSortIndicatorShown(true);
    mainLayout->addWidget(tableView, 1);

    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshData);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(btnRespond, &QPushButton::clicked, this, &MainWindow::onRespondClicked);
    connect(btnUpdateStatus, &QPushButton::clicked, this, &MainWindow::onUpdateStatusClicked);
}

void MainWindow::updateButtonsState(int tabIndex) {
    if (!btnUpdateStatus) return;
    bool isAdmin = (currentRole == "admin");
    btnAdd->setVisible(true);
    btnDelete->setVisible(true);
    btnRespond->setVisible(false);
    btnUpdateStatus->setVisible(false);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (tabIndex == 0) { // Соискатели
        btnAdd->setText("Добавить");
        btnAdd->setEnabled(true); 
        btnDelete->setEnabled(true);
    } 
    else if (tabIndex == 1) { // Вакансии
        btnAdd->setText("Добавить");
        btnRespond->setVisible(true);
        if (!isAdmin) {
            btnAdd->setEnabled(false);
            btnDelete->setEnabled(false);
        } else {
            btnAdd->setEnabled(true);
            btnDelete->setEnabled(true);
        }
    } 
    else if (tabIndex == 2) { // Компании
        btnAdd->setText("Добавить");
        if (!isAdmin) {
            btnAdd->setEnabled(false);
            btnDelete->setEnabled(false);
        } else {
            btnAdd->setEnabled(true);
            btnDelete->setEnabled(true);
        }
    }
    else { // Заявки
        btnAdd->setVisible(false);
        btnDelete->setVisible(true);
        btnDelete->setEnabled(true);
        btnRespond->setVisible(false);
        if (isAdmin) {
            btnUpdateStatus->setVisible(true);
        }
    }
}

void MainWindow::onTabChanged(int index) {
    proxyModel->setSourceModel(nullptr);
    tableView->setItemDelegateForColumn(2, nullptr);
    tableView->setItemDelegateForColumn(3, nullptr);
    if (!proxyModel || !tableView) return; // Защита от сегфолта
    proxyModel->setSourceModel(nullptr);
    switch(index) {
        case 0: 
            proxyModel->setSourceModel(modelApplicants); 
            tableView->setColumnHidden(0, false);
            tableView->setColumnHidden(7, true);
            break;
        case 1: 
            proxyModel->setSourceModel(modelVacancies); 
            tableView->setColumnHidden(0, true);
            tableView->setItemDelegateForColumn(3, new NumberDelegate(this));
            break;
        case 2: 
            proxyModel->setSourceModel(modelCompanies); 
            tableView->setColumnHidden(0, true);
            tableView->setColumnHidden(6, true);
            break;
        case 3: 
            proxyModel->setSourceModel(modelApplications); 
            tableView->setColumnHidden(0, true);
            tableView->setColumnHidden(1, false);
            tableView->setColumnHidden(5, false);
            break;
    }
    updateButtonsState(index);
}

void MainWindow::refreshData() {
    modelApplicants->select();
    modelCompanies->select();
    modelVacancies->setQuery(modelVacancies->query().lastQuery(), DatabaseManager::instance().db());
    modelApplications->setQuery(modelApplications->query().lastQuery(), DatabaseManager::instance().db());
}

void MainWindow::onAddClicked() {
    int index = tabWidget->currentIndex();
    QDialog* dlg = nullptr;

    if (index == 0) dlg = new AddApplicantDialog(this);
    else if (index == 1) dlg = new AddVacancyDialog(this);
    else if (index == 2) dlg = new AddCompanyDialog(this);

    if (dlg) {
        if (dlg->exec() == QDialog::Accepted) refreshData();
        delete dlg;
    }
}

void MainWindow::onDeleteClicked() {
    QItemSelectionModel *select = tableView->selectionModel();
    if (!select->hasSelection()) return;
    int row = select->selectedRows().first().row();
    
    int id = proxyModel->data(proxyModel->index(row, 0)).toInt();
    int index = tabWidget->currentIndex();
    QString error;
    bool success = false;

    if (index == 0) {
        ApplicantDao dao;
        success = dao.removeApplicant(id, error);
    } else if (index == 1) {
        VacancyDao dao;
        success = dao.removeVacancy(id, error);
    } else if (index == 2) {
        CompanyDao dao;
        success = dao.removeCompany(id, error);
    } else if (index == 3) {
        ApplicationDao dao;
        success = dao.removeApplication(id, error); 
    }

    if (success) {
        refreshData();
    } else {
        QMessageBox::critical(this, "Ошибка удаления", error);
    }
}

void MainWindow::onRespondClicked() {
    QItemSelectionModel *select = tableView->selectionModel();
    if (!select->hasSelection()) {
        QMessageBox::warning(this, "Внимание", "Выберите вакансию.");
        return;
    }
    int row = select->selectedRows().first().row();
    int vacancyId = proxyModel->data(proxyModel->index(row, 0)).toInt();

    bool ok;
    int applicantId = QInputDialog::getInt(this, "Отклик", "Введите ваш ID Соискателя:", 1, 1, 10000, 1, &ok);
    if (!ok) return;

    ApplicationDao dao;
    QString error;
    if (dao.addApplication(applicantId, vacancyId, error)) {
        QMessageBox::information(this, "Успех", "Отклик отправлен!");
    } else {
        QMessageBox::critical(this, "Ошибка", error);
    }
}

void MainWindow::onAcceptApplication() {
    if (this->currentRole != "admin") {
        QMessageBox::warning(this, "Доступ запрещен", "Только администратор может обрабатывать заявки!");
        return;
    }
}

void MainWindow::onUpdateStatusClicked() {
    if (this->currentRole != "admin") {
        QMessageBox::warning(this, "Доступ запрещен", "Только администратор может изменять статус!");
        return;
    }

    QItemSelectionModel *select = tableView->selectionModel();
    if (!select->hasSelection() || tabWidget->currentIndex() != 3) {
        QMessageBox::information(this, "Внимание", "Выберите заявку в таблице 'Заявки'");
        return;
    }

    int row = select->selectedRows().first().row();

    int appId = proxyModel->data(proxyModel->index(row, 0)).toInt();

    QStringList statuses = {"Ожидание", "Одобрено", "Неодобрено"};
    bool ok;
    QString res = QInputDialog::getItem(this, "Статус", "Изменить статус заявки №" + QString::number(appId), 
                                        statuses, 0, false, &ok);
    
    if (ok && !res.isEmpty()) {
        ApplicationDao dao;
        if (dao.updateStatus(appId, res)) {
            refreshData();
            QMessageBox::information(this, "Успех", "Статус обновлен");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить статус");
        }
    }
}