cmake_minimum_required(VERSION 3.16)
project(JobExchange VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    # Core
    src/core/DatabaseManager.cpp src/core/DatabaseManager.h
    src/core/Utils.h
    # DAO (Слой работы с БД)
    src/dao/UserDao.cpp src/dao/UserDao.h
    src/dao/CompanyDao.cpp src/dao/CompanyDao.h
    src/dao/ApplicantDao.cpp src/dao/ApplicantDao.h
    src/dao/VacancyDao.cpp src/dao/VacancyDao.h
    src/dao/ApplicationDao.cpp src/dao/ApplicationDao.h
    # UI (Интерфейс)
    src/ui/LoginDialog.cpp src/ui/LoginDialog.h
    src/ui/MainWindow.cpp src/ui/MainWindow.h
    # Диалоги добавления
    src/ui/AddApplicantDialog.cpp src/ui/AddApplicantDialog.h
    src/ui/AddCompanyDialog.cpp src/ui/AddCompanyDialog.h
    src/ui/AddVacancyDialog.cpp src/ui/AddVacancyDialog.h
)

add_executable(JobExchange ${PROJECT_SOURCES})
target_link_libraries(JobExchange PRIVATE Qt6::Widgets Qt6::Sql)

# --- Установка и Инсталлятор ---
include(GNUInstallDirs)
install(TARGETS JobExchange DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES db/init.sql DESTINATION share/JobExchange/db)
install(FILES docker-compose.yml DESTINATION share/JobExchange)

include(CPack)