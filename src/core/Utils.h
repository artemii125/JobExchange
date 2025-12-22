#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QLineEdit>
#include <QEvent>
#include <QTimer>
#include <QApplication>

class PhoneInputFixer : public QObject {
    Q_OBJECT
public:
    explicit PhoneInputFixer(QLineEdit* parent);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    QLineEdit* field;
};

class Utils {
public:
    static void applyDarkTheme();
    static bool processCommandLine(const QApplication &app);
    static QString formatTitleCase(const QString &text);
};

#endif // UTILS_H