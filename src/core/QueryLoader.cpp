#include "QueryLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>

QueryLoader& QueryLoader::instance() {
    static QueryLoader inst;
    return inst;
}

bool QueryLoader::loadQueries() {
    QStringList paths = {
        "/usr/local/share/JobExchange/db/auth_queries.sql",
        "/usr/local/share/JobExchange/db/data_queries.sql",
        "db/auth_queries.sql",
        "db/data_queries.sql"
    };

    bool success = false;
    for (const QString &path : paths) {
        if (loadFromFile(path)) {
            success = true;
        }
    }
    return success;
}

bool QueryLoader::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QString currentName;
    QString currentQuery;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.startsWith("--") && line.contains(":")) {
            if (!currentName.isEmpty() && !currentQuery.isEmpty()) {
                queries[currentName] = currentQuery.trimmed();
            }
            
            int colonPos = line.indexOf(":");
            currentName = line.mid(2, colonPos - 2).trimmed();
            currentQuery.clear();
        } else if (!line.isEmpty() && !line.startsWith("--")) {
            currentQuery += line + " ";
        }
    }
    
    if (!currentName.isEmpty() && !currentQuery.isEmpty()) {
        queries[currentName] = currentQuery.trimmed();
    }
    
    file.close();
    return true;
}

QString QueryLoader::getQuery(const QString &name) const {
    return queries.value(name, "");
}
