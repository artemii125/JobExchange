#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QStyleFactory>
#include <QPalette>
#include <QDebug>
#include "core/DatabaseManager.h"
#include "core/QueryLoader.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

// Функция включения темной темы (Fusion)
void setDarkTheme(QApplication &app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p.setColor(QPalette::Window, QColor(53, 53, 53));
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Base, QColor(25, 25, 25));
    p.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    p.setColor(QPalette::ToolTipBase, Qt::white);
    p.setColor(QPalette::ToolTipText, Qt::white);
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Button, QColor(53, 53, 53));
    p.setColor(QPalette::ButtonText, Qt::white);
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, QColor(42, 130, 218));
    p.setColor(QPalette::Highlight, QColor(42, 130, 218));
    p.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(p);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("JobExchange");
    app.setApplicationVersion("1.0.0");

    // 1. Настройка парсера аргументов командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("JobExchange — Система управления биржей труда");
    
    // Добавляем стандартные опции -h/--help и -v/--version
    parser.addHelpOption();
    parser.addVersionOption();

    // Добавляем опцию удаления
    QCommandLineOption uninstallOpt(QStringList() << "u" << "uninstall", "Инструкция по полному удалению приложения.");
    parser.addOption(uninstallOpt);

    // Парсим аргументы
    parser.process(app);

    // 2. Обработка флагов
    if (parser.isSet(uninstallOpt)) {
        std::cout << "\n==============================================\n"
                  << "ИНСТРУКЦИЯ ПО УДАЛЕНИЮ JobExchange:\n"
                  << "==============================================\n"
                  << "1. Удаление исполняемого файла:\n"
                  << "   sudo rm /usr/local/bin/JobExchange\n\n"
                  << "2. Удаление ярлыка меню:\n"
                  << "   sudo rm /usr/local/share/applications/JobExchange.desktop\n\n"
                  << "3. Удаление ресурсов (скрипты и конфиги):\n"
                  << "   sudo rm -rf /usr/local/share/JobExchange\n\n"
                  << "4. Остановка и удаление базы данных (Docker):\n"
                  << "   Перейдите в папку с проектом и выполните:\n"
                  << "   sudo docker-compose down\n"
                  << "==============================================\n";
        return 0;
    }

    // 3. Если флагов нет — запускаем графический интерфейс
    setDarkTheme(app);
    app.setQuitOnLastWindowClosed(false);

    // 4. Подключение к БД
    if (!DatabaseManager::instance().connect()) {
        qCritical() << "Не удалось подключиться к базе данных. Проверьте запущен ли Docker-контейнер.";
        return -1;
    }

    // 5. Загрузка SQL-запросов
    if (!QueryLoader::instance().loadQueries()) {
        qWarning() << "Не удалось загрузить SQL-запросы из файлов";
    }

    // 5. Окно входа
    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        // 6. Главное окно (передаем данные пользователя)
        MainWindow *w = new MainWindow(login.getUserRole(), login.getUserId(), login.getUserType(), login.getProfileId());
        w->show();
        return app.exec();
    }

    return 0;
}