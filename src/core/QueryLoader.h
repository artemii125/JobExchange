#ifndef QUERYLOADER_H
#define QUERYLOADER_H

#include <QString>
#include <QMap>

class QueryLoader {
public:
    static QueryLoader& instance();
    bool loadQueries();
    QString getQuery(const QString &name) const;

private:
    QueryLoader() = default;
    bool loadFromFile(const QString &filePath);
    QMap<QString, QString> queries;
};

#endif
