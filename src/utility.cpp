#include "utility.h"

#include <QLoggingCategory>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.utility", QtWarningMsg)
}

QMap<QByteArray, QByteArray> utility::parseMap(const QByteArray &bytes, char seperator, char newLine)
{
    QMap<QByteArray, QByteArray> map;
    const auto lines = bytes.split(newLine);

    for (const auto &line : lines) {
        auto index = line.indexOf(seperator);
        auto key = line.left(index).simplified();
        auto value = line.mid(index + 1).simplified();
        qCDebug(self) << "parse line (index, key, value, line):" << index << key << value << line;
        map.insert(key, value);
    }

    return map;
}
