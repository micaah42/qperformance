// SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QElapsedTimer>
#include <QObject>
#include <QQmlEngine>

#include "pollingmonitor.h"

class NetworkInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(double rxSpeed READ rxSpeed WRITE setRxSpeed NOTIFY rxSpeedChanged)
    Q_PROPERTY(double txSpeed READ txSpeed WRITE setTxSpeed NOTIFY txSpeedChanged)
    Q_PROPERTY(quint64 rxBytes READ rxBytes NOTIFY bytesChanged)
    Q_PROPERTY(quint64 txBytes READ txBytes NOTIFY bytesChanged)

public:
    explicit NetworkInterface(const QString &name = "", QObject *parent = nullptr);

    const QString &name() const;
    double rxSpeed() const;
    double txSpeed() const;
    quint64 rxBytes() const;
    quint64 txBytes() const;

signals:
    void rxSpeedChanged();
    void txSpeedChanged();
    void bytesChanged();

protected slots:
    void setRxSpeed(double newRxSpeed);
    void setTxSpeed(double newTxSpeed);

protected:
    quint64 _rxBytes;
    quint64 _txBytes;

private:
    QString _name;
    double _rxSpeed;
    double _txSpeed;

    friend class NetworkMonitor;
};

class NetworkMonitor : public PollingMonitor
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName NOTIFY interfaceNameChanged)
    Q_PROPERTY(double rxSpeed READ rxSpeed NOTIFY speedsChanged)
    Q_PROPERTY(double txSpeed READ txSpeed NOTIFY speedsChanged)
    Q_PROPERTY(quint64 rxBytes READ rxBytes NOTIFY bytesChanged)
    Q_PROPERTY(quint64 txBytes READ txBytes NOTIFY bytesChanged)
    Q_PROPERTY(QList<NetworkInterface *> interfaces READ interfaces NOTIFY interfacesChanged)

public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    QString interfaceName() const;
    void setInterfaceName(const QString &name);

    double rxSpeed() const;
    double txSpeed() const;
    quint64 rxBytes() const;
    quint64 txBytes() const;

    QList<NetworkInterface *> interfaces() const;
    NetworkInterface *interface(const QString &name) const;

signals:
    void interfaceNameChanged();
    void speedsChanged();
    void bytesChanged();
    void interfacesChanged();

protected:
    virtual void onTimeout() override;

private:
    QString _interfaceName;
    double _rxSpeed;
    double _txSpeed;
    quint64 _rxBytes;
    quint64 _txBytes;
    bool _initialized;
    QElapsedTimer _elapsedTimer;
    QList<NetworkInterface *> _interfaces;
};

class NetworkMonitorSingleton : public NetworkMonitor
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit NetworkMonitorSingleton(QObject *parent = nullptr)
        : NetworkMonitor{parent} {};
};

#endif // NETWORKMONITOR_H
