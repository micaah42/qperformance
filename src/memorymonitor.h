#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include <QObject>
#include <QQmlEngine>

#include "pollingmonitor.h"

class MemoryMonitor : public PollingMonitor
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int memAvailable READ memAvailable WRITE setMemAvailable NOTIFY memAvailableChanged FINAL)
    Q_PROPERTY(int memTotal READ memTotal WRITE setMemTotal NOTIFY memTotalChanged FINAL)
    Q_PROPERTY(int memUsed READ memUsed WRITE setMemUsed NOTIFY memUsedChanged FINAL)
    Q_PROPERTY(int swapTotal READ swapTotal WRITE setSwapTotal NOTIFY swapTotalChanged FINAL)
    Q_PROPERTY(int swapFree READ swapFree WRITE setSwapFree NOTIFY swapFreeChanged FINAL)
    Q_PROPERTY(int swapUsed READ swapUsed WRITE setSwapUsed NOTIFY swapUsedChanged FINAL)
public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    virtual void onTimeout() override;
    int memAvailable() const;
    void setMemAvailable(int newMemAvailable);

    int memTotal() const;
    void setMemTotal(int newMemTotal);

    int swapTotal() const;
    void setSwapTotal(int newSwapTotal);

    int swapFree() const;
    void setSwapFree(int newSwapFree);

    int memUsed() const;
    void setMemUsed(int newMemUsed);

    int swapUsed() const;
    void setSwapUsed(int newSwapUsed);

signals:

    void memAvailableChanged();
    void memTotalChanged();
    void swapTotalChanged();
    void swapFreeChanged();
    void memUsedChanged();
    void swapUsedChanged();

private:
    int _memAvailable;
    int _memTotal;
    int _swapTotal;
    int _swapFree;
    int _memUsed;
    int _swapUsed;
};

#endif // MEMORYMONITOR_H
