// SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UTILITY_H
#define UTILITY_H

#include <QMap>
namespace utility {

QMap<QByteArray, QByteArray> parseMap(const QByteArray &bytes, char seperator = ':', char newLine = '\n');
};

#endif // UTILITY_H
