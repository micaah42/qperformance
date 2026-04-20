#ifndef POLLINGMONITOR_H
#define POLLINGMONITOR_H

#include <QObject>
#include <QTimer>

#include "monitor.h"

class PollingMonitor : public Monitor
{
    Q_OBJECT
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
public:
    explicit PollingMonitor(QObject *parent = nullptr, int interval = 100);

    void setInterval(int newInterval);
    int interval() const;

protected:
    virtual void onTimeout() = 0;

private:
    virtual void deactivate() override;
    virtual void activate() override;

private:
    QTimer _timer;
};

#endif // POLLINGMONITOR_H
