cmake_minimum_required(VERSION 3.16)
project(JobExchange VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Статическая линковка
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# Поиск Qt6
find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

# Список всех исходных файлов
set(PROJECT_SOURCES
    src/main.cpp
    src/core/DatabaseManager.cpp src/core/DatabaseManager.h
    src/core/DataExporter.cpp src/core/DataExporter.h
    src/core/QueryLoader.cpp src/core/QueryLoader.h
    src/core/Utils.cpp src/core/Utils.h
    src/dao/UserDao.cpp src/dao/UserDao.h
    src/dao/CompanyDao.cpp src/dao/CompanyDao.h
    src/dao/ApplicantDao.cpp src/dao/ApplicantDao.h
    src/dao/VacancyDao.cpp src/dao/VacancyDao.h
    src/dao/ApplicationDao.cpp src/dao/ApplicationDao.h
    src/ui/LoginDialog.cpp src/ui/LoginDialog.h
    src/ui/RegisterDialog.cpp src/ui/RegisterDialog.h
    src/ui/MainWindow.cpp src/ui/MainWindow.h
    src/ui/AddApplicantDialog.cpp src/ui/AddApplicantDialog.h
    src/ui/AddCompanyDialog.cpp src/ui/AddCompanyDialog.h
    src/ui/AddVacancyDialog.cpp src/ui/AddVacancyDialog.h
    resources.qrc
)

add_executable(JobExchange ${PROJECT_SOURCES})
target_link_libraries(JobExchange PRIVATE Qt6::Widgets Qt6::Sql)

# --- Установка по стандартам Linux ---
include(GNUInstallDirs)

# Установка бинарного файла в /usr/local/bin
install(TARGETS JobExchange RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Установка ресурсов в /usr/local/share/JobExchange
install(FILES db/init.sql DESTINATION ${CMAKE_INSTALL_DATADIR}/JobExchange/db)
install(FILES db/auth_queries.sql DESTINATION ${CMAKE_INSTALL_DATADIR}/JobExchange/db)
install(FILES db/data_queries.sql DESTINATION ${CMAKE_INSTALL_DATADIR}/JobExchange/db)
install(FILES docker-compose.yml DESTINATION ${CMAKE_INSTALL_DATADIR}/JobExchange)

# Создание ярлыка (.desktop файла) для меню приложений
set(DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/JobExchange.desktop")
file(WRITE ${DESKTOP_FILE}
    "[Desktop Entry]\n"
    "Type=Application\n"
    "Name=JobExchange\n"
    "Exec=JobExchange\n"
    "Icon=system-run\n"
    "Categories=Office;Development;\n"
    "Terminal=false\n"
)
install(FILES ${DESKTOP_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)