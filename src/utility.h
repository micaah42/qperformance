#ifndef UTILITY_H
#define UTILITY_H

#include <QMap>
namespace utility {

QMap<QByteArray, QByteArray> parseMap(const QByteArray &bytes, char seperator = ':', char newLine = '\n');
};

#endif // UTILITY_H
