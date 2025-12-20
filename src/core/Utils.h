#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>

class Utils {
public:
    // Функция делает первую букву заглавной, остальные маленькими
    // Для ФИО разбивает по пробелам и делает это для каждого слова
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

#endif // UTILS_H