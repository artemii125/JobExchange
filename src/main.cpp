#include <QApplication>
#include <QCommandLineParser>
#include <QStyleFactory>
#include <QPalette>
#include "core/DatabaseManager.h"
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
    app.setApplicationVersion("1.0");

    // 1. Применяем стиль
    setDarkTheme(app);

    // 2. Обработка флагов (--help, --uninstall)
    QCommandLineParser parser;
    parser.setApplicationDescription("Биржа Труда");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption uninstallOpt("uninstall", "Удалить данные приложения");
    parser.addOption(uninstallOpt);
    parser.process(app);

    if (parser.isSet(uninstallOpt)) {
        qInfo() << "Запущена процедура очистки...";
        // Логика удаления (имитация)
        return 0;
    }

    // 3. Подключение к БД (с ожиданием)
    if (!DatabaseManager::instance().connect()) {
        return -1;
    }

    // 4. Окно входа
    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        // 5. Главное окно (если вход успешен)
        MainWindow w(login.getUserRole());
        w.show();
        return app.exec();
    }

    return 0;
}