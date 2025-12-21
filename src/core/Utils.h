#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>
#include <QLineEdit>
#include <QEvent>
#include <QTimer>
#include <QString>
#include <QStringList>

class Utils {
public:
    // Функция делает первую букву заглавной, остальные маленькими
    static QString formatTitleCase(const QString &input) {
        QStringList parts = input.toLower().split(' ', Qt::SkipEmptyParts);
        for (QString &part : parts) {
            if (!part.isEmpty()) {
                part[0] = part[0].toUpper();
            }
        }
        return parts.join(' ');
    }
};

class PhoneInputFixer : public QObject {
    Q_OBJECT
public:
    explicit PhoneInputFixer(QLineEdit* parent) : QObject(parent), field(parent) {
        field->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {  
        if (watched == field && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)) {
            QTimer::singleShot(0, this, [this]() {
                QString currentText = field->displayText();
                int firstPlaceholder = currentText.indexOf('_');
                
                if (firstPlaceholder != -1) {
                    field->setCursorPosition(firstPlaceholder);
                }
            });
        }
        return false;
    }

private:
    QLineEdit* field;
};

#endif // UTILS_H