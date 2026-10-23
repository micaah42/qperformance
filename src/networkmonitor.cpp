// SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "networkmonitor.h"

#include <QFile>
#include <QLoggingCategory>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.network", QtInfoMsg);
}

NetworkInterface::NetworkInterface(const QString &name, QObject *parent)
    : QObject{parent}
    , _rxBytes{0}
    , _txBytes{0}
    , _name{name}
    , _rxSpeed{0}
    , _txSpeed{0}
{}

const QString &NetworkInterface::name() const
{
    return _name;
}

double NetworkInterface::rxSpeed() const
{
    return _rxSpeed;
}

double NetworkInterface::txSpeed() const
{
    return _txSpeed;
}

quint64 NetworkInterface::rxBytes() const
{
    return _rxBytes;
}

quint64 NetworkInterface::txBytes() const
{
    return _txBytes;
}

void NetworkInterface::setRxSpeed(double newRxSpeed)
{
    if (qFuzzyCompare(_rxSpeed, newRxSpeed))
        return;

    _rxSpeed = newRxSpeed;
    emit rxSpeedChanged();
}

void NetworkInterface::setTxSpeed(double newTxSpeed)
{
    if (qFuzzyCompare(_txSpeed, newTxSpeed))
        return;

    _txSpeed = newTxSpeed;
    emit txSpeedChanged();
}

NetworkMonitor::NetworkMonitor(QObject *parent)
    : PollingMonitor{parent, 1000}
    , _rxSpeed{0}
    , _txSpeed{0}
    , _rxBytes{0}
    , _txBytes{0}
    , _initialized{false}
{
    _elapsedTimer.start();
    NetworkMonitor::onTimeout();
}

QString NetworkMonitor::interfaceName() const
{
    return _interfaceName;
}

void NetworkMonitor::setInterfaceName(const QString &name)
{
    if (_interfaceName == name)
        return;

    _interfaceName = name;
    _rxBytes = 0;
    _txBytes = 0;
    _rxSpeed = 0;
    _txSpeed = 0;
    _initialized = false;
    _elapsedTimer.restart();

    emit interfaceNameChanged();
    emit speedsChanged();
    emit bytesChanged();

    onTimeout();
}

double NetworkMonitor::rxSpeed() const
{
    return _rxSpeed;
}

double NetworkMonitor::txSpeed() const
{
    return _txSpeed;
}

quint64 NetworkMonitor::rxBytes() const
{
    return _rxBytes;
}

quint64 NetworkMonitor::txBytes() const
{
    return _txBytes;
}

QList<NetworkInterface *> NetworkMonitor::interfaces() const
{
    return _interfaces;
}

NetworkInterface *NetworkMonitor::interface(const QString &name) const
{
    for (auto iface : _interfaces) {
        if (iface->name() == name)
            return iface;
    }
    return nullptr;
}

static quint64 readStatFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return 0;

    return file.readAll().trimmed().toULongLong();
}

void NetworkMonitor::onTimeout()
{
    qCDebug(self) << this << "polling network statistics...";
    qint64 elapsedMs = _elapsedTimer.restart();
    double elapsedSec = elapsedMs > 0 ? (elapsedMs / 1000.0) : 1.0;

    /*
     * If an interface is specified, read directly from sysfs statistics.
     * Otherwise or additionally, /proc/net/dev can also be parsed.
     */
    if (!_interfaceName.isEmpty()) {
        QString basePath = QStringLiteral("/sys/class/net/") + _interfaceName + QStringLiteral("/statistics/");
        quint64 currentRx = readStatFile(basePath + QStringLiteral("rx_bytes"));
        quint64 currentTx = readStatFile(basePath + QStringLiteral("tx_bytes"));

        if (_initialized && elapsedSec > 0) {
            double rxDelta = (currentRx >= _rxBytes) ? static_cast<double>(currentRx - _rxBytes) : 0.0;
            double txDelta = (currentTx >= _txBytes) ? static_cast<double>(currentTx - _txBytes) : 0.0;

            _rxSpeed = rxDelta / elapsedSec;
            _txSpeed = txDelta / elapsedSec;
        } else {
            _rxSpeed = 0.0;
            _txSpeed = 0.0;
            _initialized = true;
        }

        _rxBytes = currentRx;
        _txBytes = currentTx;

        emit speedsChanged();
        emit bytesChanged();
        emit valuesAvailable({_rxSpeed, _txSpeed});
    }

    /*
     * Parse /proc/net/dev to update all interfaces
     */
    QFile procNetDev(QStringLiteral("/proc/net/dev"));
    if (procNetDev.open(QIODevice::ReadOnly)) {
        auto lines = procNetDev.readAll().split('\n');
        bool interfacesAdded = false;

        for (const auto &line : lines) {
            int colonIdx = line.indexOf(':');
            if (colonIdx == -1)
                continue;

            QString ifName = QString::fromUtf8(line.left(colonIdx).trimmed());
            auto parts = line.mid(colonIdx + 1).simplified().split(' ');

            if (parts.size() < 9)
                continue;

            quint64 rx = parts[0].toULongLong();
            quint64 tx = parts[8].toULongLong();

            NetworkInterface *iface = nullptr;
            for (auto item : _interfaces) {
                if (item->name() == ifName) {
                    iface = item;
                    break;
                }
            }

            if (!iface) {
                iface = new NetworkInterface(ifName, this);
                iface->_rxBytes = rx;
                iface->_txBytes = tx;
                _interfaces.append(iface);
                interfacesAdded = true;
            } else {
                if (elapsedSec > 0) {
                    double rxDelta = (rx >= iface->_rxBytes) ? static_cast<double>(rx - iface->_rxBytes) : 0.0;
                    double txDelta = (tx >= iface->_txBytes) ? static_cast<double>(tx - iface->_txBytes) : 0.0;
                    iface->setRxSpeed(rxDelta / elapsedSec);
                    iface->setTxSpeed(txDelta / elapsedSec);
                }
                iface->_rxBytes = rx;
                iface->_txBytes = tx;
                emit iface->bytesChanged();
            }
        }

        if (interfacesAdded)
            emit interfacesChanged();
    }
}
