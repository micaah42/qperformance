#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <QObject>
#include <QTimer>

#include "monitor.h"

class CPUCore;

class CPUMonitor : public Monitor
{
    Q_OBJECT

    Q_PROPERTY(QString architecture READ architecture CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString vendor READ vendor CONSTANT)

    Q_PROPERTY(QList<double> usages READ usages NOTIFY usagesChanged FINAL)
    Q_PROPERTY(QList<CPUCore *> cores READ cores NOTIFY coresChanged)
    Q_PROPERTY(CPUCore *total READ total NOTIFY totalChanged FINAL)
    Q_PROPERTY(int temperature READ temperature NOTIFY temperatureChanged)

public:
    explicit CPUMonitor(QObject *parent = nullptr);

    QList<CPUCore *> cores() const;
    int temperature() const;
    QString architecture() const;
    QString model() const;
    QString vendor() const;

    CPUCore *total() const;

    QList<double> usages() const;
    void setUsages(const QList<double> &newUsages);

signals:
    void coresChanged();
    void temperatureChanged();
    void totalChanged();
    void usagesChanged();

private slots:
    void setTemperature(int temperature);
    void onTimeout();

private:
    int _temperature;
    QList<CPUCore *> _cores;
    QString _architecture;
    QString _model;
    QString _vendor;
    QList<double> _usages;
};

class CPUCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(double load READ load WRITE setLoad NOTIFY loadChanged)

public:
    explicit CPUCore(const QString &name = "", QObject *parent = nullptr);

    const QString &name() const;
    double load() const;

signals:
    void loadChanged();

protected:
    quint64 _totalTime;
    quint64 _activeTime;

protected slots:
    void setLoad(double newLoad);

private:
    double _load;
    QString _name;

    friend class CPUMonitor;
};
#endif // CPUMONITOR_H
