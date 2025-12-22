#include <QApplication>
#include <QDebug>
#include "core/DatabaseManager.h"
#include "core/Utils.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("JobExchange");
    app.setApplicationVersion("1.0.0");

    // Обработка флагов (удаление и т.д.)
    if (Utils::processCommandLine(app)) return 0;

    // Применение оформления
    Utils::applyDarkTheme();

    // Подключение к БД
    if (!DatabaseManager::instance().connect()) {
        qCritical() << "Не удалось подключиться к базе данных.";
        return -1;
    }

    // Авторизация
    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        MainWindow w(login.getUserRole());
        w.show();
        return app.exec();
    }

    return 0;
}