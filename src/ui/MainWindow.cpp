#include "MainWindow.h"
//диалоги
#include "AddApplicantDialog.h"
#include "AddCompanyDialog.h"
#include "AddVacancyDialog.h"
#include "LoginDialog.h"
//ядро и DAO
#include "../core/DatabaseManager.h"
#include "../core/DataExporter.h"
#include "../core/QueryLoader.h"
#include "../dao/VacancyDao.h"
#include "../dao/CompanyDao.h"
#include "../dao/ApplicantDao.h"
#include "../dao/ApplicationDao.h"
//Qt
#include <QApplication>
#include <QTimer>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>


MainWindow::MainWindow(const QString &role, int userId, const QString &userType, int profileId, QWidget *parent) 
    : QMainWindow(parent), currentRole(role), currentUserId(userId), currentUserType(userType), currentProfileId(profileId),
      modelApplicants(nullptr), modelCompanies(nullptr), modelVacancies(nullptr), modelApplications(nullptr), proxyModel(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    
    QString title = "Биржа Труда [";
    if (role == "admin") title += "Администратор";
    else if (userType == "company") title += "Компания";
    else title += "Соискатель";
    title += "]";
    
    setWindowTitle(title);
    resize(1200, 700);
    setupModels();
    setupUi();
    onTabChanged(0);
}

MainWindow::~MainWindow() {
    if (proxyModel) {
        proxyModel->setSourceModel(nullptr);
    }
}

void MainWindow::setupModels() {
    QSqlDatabase db = DatabaseManager::instance().db();
    
    //проверка готовности БД
    if (!db.isOpen() || !db.isValid()) {
        qWarning() << "База данных не готова";
        return;
    }

    // 1. Соискатели
    qDebug() << "Инициализация модели...";
    modelApplicants = new QSqlTableModel(this);
    modelApplicants->setTable("applicants");
    modelApplicants->setHeaderData(0, Qt::Horizontal, "ID");
    modelApplicants->setHeaderData(1, Qt::Horizontal, "User ID");
    modelApplicants->setHeaderData(2, Qt::Horizontal, "ФИО");
    modelApplicants->setHeaderData(3, Qt::Horizontal, "Дата рождения");
    modelApplicants->setHeaderData(4, Qt::Horizontal, "Телефон");
    modelApplicants->setHeaderData(5, Qt::Horizontal, "Email");
    modelApplicants->setHeaderData(6, Qt::Horizontal, "Специальность");
    modelApplicants->setHeaderData(7, Qt::Horizontal, "Опыт");
    modelApplicants->setHeaderData(8, Qt::Horizontal, "Желаемая зарплата");
    
    if (currentRole == "user" && currentUserType == "applicant") {
        modelApplicants->setFilter(QString("user_id = %1").arg(currentUserId));
    }
    modelApplicants->select();

    // 2. Компании
    modelCompanies = new QSqlTableModel(this);
    modelCompanies->setTable("companies");
    modelCompanies->setHeaderData(0, Qt::Horizontal, "ID");
    modelCompanies->setHeaderData(1, Qt::Horizontal, "User ID");
    modelCompanies->setHeaderData(2, Qt::Horizontal, "Название");
    modelCompanies->setHeaderData(3, Qt::Horizontal, "ИНН");
    modelCompanies->setHeaderData(4, Qt::Horizontal, "Адрес");
    modelCompanies->setHeaderData(5, Qt::Horizontal, "Телефон");
    modelCompanies->setHeaderData(6, Qt::Horizontal, "Контактное лицо");
    
    if (currentRole == "user" && currentUserType == "company") {
        modelCompanies->setFilter(QString("user_id = %1").arg(currentUserId));
    }
    modelCompanies->select();

    // 3. Вакансии
    modelVacancies = new QSqlQueryModel(this);
    QString vacancyQuery;
    if (currentRole == "user" && currentUserType == "company") {
        vacancyQuery = QueryLoader::instance().getQuery("GetVacanciesByCompany");
        vacancyQuery.replace("?", QString::number(currentProfileId));
    } else {
        vacancyQuery = QueryLoader::instance().getQuery("GetAllVacancies");
    }
    modelVacancies->setQuery(vacancyQuery);
    modelVacancies->setHeaderData(0, Qt::Horizontal, "ID");
    modelVacancies->setHeaderData(1, Qt::Horizontal, "Компания");
    modelVacancies->setHeaderData(2, Qt::Horizontal, "Специальность");
    modelVacancies->setHeaderData(3, Qt::Horizontal, "Зарплата");
    modelVacancies->setHeaderData(4, Qt::Horizontal, "Дата");
    modelVacancies->setHeaderData(5, Qt::Horizontal, "Статус");

    // 4. Заявки
    modelApplications = new QSqlQueryModel(this);
    QString appQuery;
    
    if (currentRole == "user" && currentUserType == "applicant") {
        appQuery = QueryLoader::instance().getQuery("GetApplicationsByApplicant");
        appQuery.replace("?", QString::number(currentProfileId));
    } else if (currentRole == "user" && currentUserType == "company") {
        appQuery = QueryLoader::instance().getQuery("GetApplicationsByCompany");
        appQuery.replace("?", QString::number(currentProfileId));
    } else {
        appQuery = QueryLoader::instance().getQuery("GetAllApplications");
    }
    
    modelApplications->setQuery(appQuery);
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

    //вкладки
    tabWidget = new QTabWidget(this);
    
    if (currentRole == "admin" || currentUserType == "applicant") {
        tabWidget->addTab(new QWidget, "Соискатели");
    }
    tabWidget->addTab(new QWidget, "Вакансии");
    if (currentRole == "admin" || currentUserType == "company") {
        tabWidget->addTab(new QWidget, "Компании");
    }
    tabWidget->addTab(new QWidget, "Заявки");
    
    mainLayout->addWidget(tabWidget, 0);

    //кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnAdd = new QPushButton("Добавить", this);
    btnDelete = new QPushButton("Удалить", this);
    btnRespond = new QPushButton("Откликнуться", this);
    btnRefresh = new QPushButton("Обновить", this);
    btnUpdateStatus = new QPushButton("Изменить статус", this);
    btnLogout = new QPushButton("Выйти", this);
    
    btnLayout->addWidget(btnUpdateStatus);
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnDelete);
    btnLayout->addWidget(btnRespond);
    btnLayout->addWidget(btnRefresh);
    
    if (currentRole == "admin") {
        btnExport = new QPushButton("Экспорт", this);
        btnImport = new QPushButton("Импорт", this);
        btnLayout->addWidget(btnExport);
        btnLayout->addWidget(btnImport);
        connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportData);
        connect(btnImport, &QPushButton::clicked, this, &MainWindow::onImportData);
    }
    
    btnLayout->addStretch();
    btnLayout->addWidget(btnLogout);
    mainLayout->addLayout(btnLayout);

    //таблица
    tableView = new QTableView(this);
    tableView->setModel(proxyModel);
    tableView->setSortingEnabled(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);
    mainLayout->addWidget(tableView, 1);

    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshData);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(btnRespond, &QPushButton::clicked, this, &MainWindow::onRespondClicked);
    connect(btnUpdateStatus, &QPushButton::clicked, this, &MainWindow::onUpdateStatusClicked);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogout);
}

void MainWindow::updateButtonsState(int tabIndex) {
    if (!btnUpdateStatus) return;
    
    bool isAdmin = (currentRole == "admin");
    bool isCompany = (currentUserType == "company");
    bool isApplicant = (currentUserType == "applicant");
    
    btnAdd->setVisible(true);
    btnDelete->setVisible(true);
    btnRespond->setVisible(false);
    btnUpdateStatus->setVisible(false);

    QString tabName = tabWidget->tabText(tabIndex);
    
    if (tabName == "Соискатели") {
        btnAdd->setEnabled(isAdmin);
        btnDelete->setEnabled(isAdmin);
        if (isApplicant) {
            btnAdd->setVisible(false);
            btnDelete->setVisible(false);
        }
    } 
    else if (tabName == "Вакансии") {
        btnAdd->setEnabled(isAdmin || isCompany);
        btnDelete->setEnabled(isAdmin || isCompany);
        btnRespond->setVisible(isApplicant);
        if (isApplicant) {
            btnAdd->setVisible(false);
            btnDelete->setVisible(false);
        }
    } 
    else if (tabName == "Компании") {
        btnAdd->setEnabled(isAdmin);
        btnDelete->setEnabled(isAdmin);
        if (isCompany) {
            btnAdd->setVisible(false);
            btnDelete->setVisible(false);
        }
    }
    else if (tabName == "Заявки") {
        btnAdd->setVisible(false);
        btnDelete->setEnabled(isAdmin);
        btnUpdateStatus->setVisible(isAdmin || isCompany);
        if (isApplicant) {
            btnDelete->setVisible(false);
        }
    }
}

void MainWindow::onTabChanged(int index) {
    proxyModel->setSourceModel(nullptr);
    tableView->setItemDelegateForColumn(2, nullptr);
    tableView->setItemDelegateForColumn(3, nullptr);
    
    QString tabName = tabWidget->tabText(index);
    
    if (tabName == "Соискатели") {
        proxyModel->setSourceModel(modelApplicants);
        tableView->setColumnHidden(0, true);
        tableView->setColumnHidden(1, true);
    } 
    else if (tabName == "Вакансии") {
        proxyModel->setSourceModel(modelVacancies);
        tableView->setColumnHidden(0, true);
        tableView->setItemDelegateForColumn(3, new NumberDelegate(this));
    } 
    else if (tabName == "Компании") {
        proxyModel->setSourceModel(modelCompanies);
        tableView->setColumnHidden(0, true);
        tableView->setColumnHidden(1, true);
    } 
    else if (tabName == "Заявки") {
        proxyModel->setSourceModel(modelApplications);
        tableView->setColumnHidden(0, true);
        tableView->setColumnHidden(1, true);
    }
    
    updateButtonsState(index);
}

void MainWindow::refreshData() {
    modelApplicants->select();
    modelCompanies->select();
    modelVacancies->setQuery(modelVacancies->query().lastQuery());
    modelApplications->setQuery(modelApplications->query().lastQuery());
}

void MainWindow::onAddClicked() {
    QString tabName = tabWidget->tabText(tabWidget->currentIndex());
    QDialog* dlg = nullptr;

    if (tabName == "Соискатели") {
        dlg = new AddApplicantDialog(this);
    }
    else if (tabName == "Вакансии") {
        if (currentUserType == "company") {
            //создаем вакансию только для текущей компании
            dlg = new AddVacancyDialog(currentProfileId, this);
        } else {
            //администратор может выбрать любую компанию
            dlg = new AddVacancyDialog(this);
        }
    }
    else if (tabName == "Компании") {
        dlg = new AddCompanyDialog(this);
    }

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
    QString tabName = tabWidget->tabText(tabWidget->currentIndex());
    QString error;
    bool success = false;

    if (tabName == "Соискатели") {
        ApplicantDao dao;
        success = dao.removeApplicant(id, error);
    } else if (tabName == "Вакансии") {
        VacancyDao dao;
        success = dao.removeVacancy(id, error);
    } else if (tabName == "Компании") {
        CompanyDao dao;
        success = dao.removeCompany(id, error);
    } else if (tabName == "Заявки") {
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
    if (currentUserType != "applicant") {
        QMessageBox::warning(this, "Ошибка", "Только соискатели могут откликаться на вакансии");
        return;
    }
    
    QItemSelectionModel *select = tableView->selectionModel();
    if (!select->hasSelection()) {
        QMessageBox::warning(this, "Внимание", "Выберите вакансию.");
        return;
    }
    int row = select->selectedRows().first().row();
    int vacancyId = proxyModel->data(proxyModel->index(row, 0)).toInt();

    ApplicationDao dao;
    QString error;
    if (dao.addApplication(currentProfileId, vacancyId, error)) {
        QMessageBox::information(this, "Успех", "Отклик отправлен!");
        refreshData();
    } else {
        QMessageBox::critical(this, "Ошибка", error);
    }
}

void MainWindow::onUpdateStatusClicked() {
    if (currentRole != "admin" && currentUserType != "company") {
        QMessageBox::warning(this, "Доступ запрещен", "Только администратор или компания могут изменять статус!");
        return;
    }

    QItemSelectionModel *select = tableView->selectionModel();
    if (!select->hasSelection()) {
        QMessageBox::information(this, "Внимание", "Выберите заявку");
        return;
    }

    int row = select->selectedRows().first().row();
    int appId = proxyModel->data(proxyModel->index(row, 0)).toInt();

    QStringList statuses = {"На рассмотрении", "Одобрено", "Отклонено"};
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

void MainWindow::onExportData() {
    QStringList options = {"Экспорт всей БД (SQL)", "Экспорт таблицы (CSV)"};
    bool ok;
    QString choice = QInputDialog::getItem(this, "Экспорт", "Выберите тип экспорта:", options, 0, false, &ok);
    
    if (!ok) return;
    
    QString error;
    
    if (choice == options[0]) {
        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить SQL", "", "SQL Files (*.sql)");
        if (filePath.isEmpty()) return;
        
        if (DataExporter::exportDatabaseToSQL(filePath, error)) {
            QMessageBox::information(this, "Успех", "База данных экспортирована в " + filePath);
        } else {
            QMessageBox::critical(this, "Ошибка", error);
        }
    } else {
        QStringList tableNames = {"Соискатели", "Компании", "Вакансии", "Заявки"};
        QStringList tableIds = {"applicants", "companies", "vacancies", "applications"};
        QString tableName = QInputDialog::getItem(this, "Экспорт CSV", "Выберите таблицу:", tableNames, 0, false, &ok);
        if (!ok) return;
        
        int idx = tableNames.indexOf(tableName);
        QString table = tableIds[idx];
        
        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить CSV", "", "CSV Files (*.csv)");
        if (filePath.isEmpty()) return;
        
        if (DataExporter::exportToCSV(table, filePath, error)) {
            QMessageBox::information(this, "Успех", "Таблица экспортирована в " + filePath);
        } else {
            QMessageBox::critical(this, "Ошибка", error);
        }
    }
}

void MainWindow::onImportData() {
    QStringList options = {"Импорт SQL", "Импорт CSV"};
    bool ok;
    QString choice = QInputDialog::getItem(this, "Импорт", "Выберите тип импорта:", options, 0, false, &ok);
    
    if (!ok) return;
    
    QString error;
    
    if (choice == options[0]) {
        QString filePath = QFileDialog::getOpenFileName(this, "Открыть SQL", "", "SQL Files (*.sql)");
        if (filePath.isEmpty()) return;
        
        if (DataExporter::importDatabaseFromSQL(filePath, error)) {
            QMessageBox::information(this, "Успех", "База данных импортирована");
            refreshData();
        } else {
            QMessageBox::critical(this, "Ошибка", error);
        }
    } else {
        QStringList tableNames = {"Соискатели", "Компании", "Вакансии", "Заявки"};
        QStringList tableIds = {"applicants", "companies", "vacancies", "applications"};
        QString tableName = QInputDialog::getItem(this, "Импорт CSV", "Выберите таблицу:", tableNames, 0, false, &ok);
        if (!ok) return;
        
        int idx = tableNames.indexOf(tableName);
        QString table = tableIds[idx];
        
        QString filePath = QFileDialog::getOpenFileName(this, "Открыть CSV", "", "CSV Files (*.csv)");
        if (filePath.isEmpty()) return;
        
        if (DataExporter::importFromCSV(table, filePath, error)) {
            QMessageBox::information(this, "Успех", "Таблица импортирована");
            refreshData();
        } else {
            QMessageBox::critical(this, "Ошибка", error);
        }
    }
}

void MainWindow::onLogout() {
    isLoggingOut = true;
    close();
    QTimer::singleShot(0, []() {
        LoginDialog *login = new LoginDialog(nullptr);
        int result = login->exec();
        if (result == QDialog::Accepted) {
            MainWindow *w = new MainWindow(login->getUserRole(), login->getUserId(), 
                                          login->getUserType(), login->getProfileId());
            w->show();
        } else {
            QApplication::quit();
        }
        login->deleteLater();
    });
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!isLoggingOut) {
        QApplication::quit();
    }
    event->accept();
}
