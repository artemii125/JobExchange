#include "Utils.h"
#include <QStyleFactory>
#include <QPalette>
#include <QCommandLineParser>
#include <iostream>

PhoneInputFixer::PhoneInputFixer(QLineEdit* parent) : QObject(parent), field(parent) {
    field->installEventFilter(this);
}

bool PhoneInputFixer::eventFilter(QObject* watched, QEvent* event) {
    if (watched == field && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)) {
        QTimer::singleShot(0, this, [this]() {
            QString currentText = field->displayText();
            int firstPlaceholder = currentText.indexOf('_');
            if (firstPlaceholder != -1) field->setCursorPosition(firstPlaceholder);
        });
    }
    return false;
}

void Utils::applyDarkTheme() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
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
    qApp->setPalette(p);
}

bool Utils::processCommandLine(const QApplication &app) {
    QCommandLineParser parser;
    parser.setApplicationDescription("JobExchange — Система управления биржей труда");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption uninstallOpt({"u", "uninstall"}, "Инструкция по полному удалению приложения.");
    parser.addOption(uninstallOpt);
    parser.process(app);

    if (parser.isSet(uninstallOpt)) {
        std::cout << "\n=== ИНСТРУКЦИЯ ПО УДАЛЕНИЮ JobExchange ===\n"
                  << "1. Удаление бинарного файла: sudo rm /usr/local/bin/JobExchange\n"
                  << "2. Удаление базы данных: sudo docker-compose down\n"
                  << "==========================================\n";
        return true; // Программу нужно завершить
    }
    return false; // Продолжаем запуск
}

QString Utils::formatTitleCase(const QString &text) {
    if (text.isEmpty()) return text;
    QString result = text.toLower();
    result[0] = result[0].toUpper();
    return result;
}