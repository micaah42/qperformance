#include "monitor.h"

Monitor::Monitor(QObject *parent)
    : QObject{parent}
    , m_subscribers{0}
{
    connect(&_timer, &QTimer::timeout, this, &Monitor::onTimeout);
    _timer.setTimerType(Qt::PreciseTimer);
    _timer.setInterval(100);
}

int Monitor::interval() const
{
    return _timer.interval();
}

void Monitor::setInterval(int newInterval)
{
    if (_timer.interval() == newInterval)
        return;

    _timer.setInterval(newInterval);
    emit intervalChanged();
}

bool Monitor::active() const
{
    return _timer.isActive();
}

void Monitor::unsubscribe(QObject *subscriber)
{
    m_subscribers--;

    if (m_subscribers == 0)
        this->setActive(false);
}

void Monitor::subscribe(QObject *subscriber)
{
    m_subscribers++;

    if (m_subscribers == 1)
        this->setActive(true);
}

void Monitor::setActive(bool newactive)
{
    if (_timer.isActive() == newactive)
        return;

    if (newactive) {
        this->onTimeout();
        _timer.start();
    }

    else {
        _timer.stop();
    }

    emit activeChanged();
}

int Monitor::subscribers() const
{
    return m_subscribers;
}

void Monitor::setSubscribers(int newSubscribers)
{
    if (m_subscribers == newSubscribers)
        return;

    m_subscribers = newSubscribers;
    emit subscribersChanged();
}
