#include "monitor.h"

Monitor::Monitor(QObject *parent)
    : QObject{parent}
    , _active{false}
{}

bool Monitor::active() const
{
    return !_subscribers.empty();
}

void Monitor::unsubscribe(QObject *subscriber)
{
    _subscribers.removeOne(subscriber);
    this->updateActive();
}

void Monitor::subscribe(QObject *subscriber)
{
    connect(subscriber, &QObject::destroyed, this, [this, subscriber]() {
        _subscribers.removeAll(subscriber);
        this->updateActive();
    });

    _subscribers.append(subscriber);
    this->updateActive();
}

void Monitor::updateActive()
{
    auto newActive = !_subscribers.empty();

    if (newActive == _active)
        return;

    _active = newActive;
    _active ? this->activate() : this->deactivate();
}
