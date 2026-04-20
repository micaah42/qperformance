#include "pollingmonitor.h"

PollingMonitor::PollingMonitor(QObject *parent, int interval)
{
    connect(&_timer, &QTimer::timeout, this, &PollingMonitor::onTimeout);
    _timer.setTimerType(Qt::PreciseTimer);
    _timer.setInterval(interval);
}

void PollingMonitor::setInterval(int newInterval)
{
    if (_timer.interval() == newInterval)
        return;

    _timer.setInterval(newInterval);
    emit intervalChanged();
}

int PollingMonitor::interval() const
{
    return _timer.interval();
}
void PollingMonitor::deactivate()
{
    _timer.stop();
}

void PollingMonitor::activate()
{
    this->onTimeout();
    _timer.start();
}
