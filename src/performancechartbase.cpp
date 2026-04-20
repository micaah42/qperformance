#include "performancechartbase.h"

#include <QLoggingCategory>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.chartbase")
}

PerformanceChartBase::PerformanceChartBase()
    : _min{0}
    , _max{100}
    , _duration{5000}
    , _colors{Qt::white}
    , _lineWidths{1.}
{
    connect(this, &QQuickItem::heightChanged, this, &PerformanceChartBase::reset);
    connect(this, &QQuickItem::widthChanged, this, &PerformanceChartBase::reset);

    _updateTimer.callOnTimeout(this, [this]() { this->update(); });
    _updateTimer.setTimerType(Qt::PreciseTimer);
    _updateTimer.setSingleShot(true);
    _updateTimer.setInterval(150);
}

PerformanceChartBase::~PerformanceChartBase()
{
    if (_monitor)
        _monitor->unsubscribe(this);
};

QColor PerformanceChartBase::color(int i) const
{
    return _colors.empty() ? QColor{0, 0, 0} : _colors[i % _colors.size()];
}

double PerformanceChartBase::duration() const
{
    return _duration;
}

void PerformanceChartBase::setDuration(double newDuration)
{
    if (_duration == newDuration)
        return;

    _duration = newDuration;
    emit durationChanged();
}

const QList<QColor> &PerformanceChartBase::colors() const
{
    return _colors;
}

void PerformanceChartBase::setColors(const QList<QColor> &newColors)
{
    if (_colors == newColors)
        return;

    _colors = newColors;
    emit colorsChanged();
}

int PerformanceChartBase::maxUpdateInterval() const
{
    return _updateTimer.interval();
}

void PerformanceChartBase::setMaxUpdateInterval(int newMaxUpdateInterval)
{
    if (_updateTimer.interval() == newMaxUpdateInterval)
        return;

    qCDebug(self) << "set max update interval:" << newMaxUpdateInterval;
    _updateTimer.setInterval(newMaxUpdateInterval);
    emit maxUpdateIntervalChanged();
}

void PerformanceChartBase::pushValues(const QList<double> &values)
{
    this->pushValues2(values, _elapsedTimer.elapsed());
}

void PerformanceChartBase::requestUpdate()
{
    if (!_updateTimer.isActive())
        _updateTimer.start();
}

qint64 PerformanceChartBase::elapsedTime() const
{
    return _elapsedTimer.elapsed();
}

double PerformanceChartBase::lineWidth(int i) const
{
    return _lineWidths.empty() ? 1 : _lineWidths[i % _lineWidths.size()];
}

double PerformanceChartBase::min() const
{
    return _min;
}

void PerformanceChartBase::setMin(double newMin)
{
    if (qFuzzyCompare(_min, newMin))
        return;

    _min = newMin;
    emit minChanged();
}

double PerformanceChartBase::max() const
{
    return _max;
}

void PerformanceChartBase::setMax(double newMax)
{
    if (qFuzzyCompare(_max, newMax))
        return;

    _max = newMax;
    emit maxChanged();
}

QList<double> PerformanceChartBase::lineWidths() const
{
    return _lineWidths;
}

void PerformanceChartBase::setLineWidths(const QList<double> &newLineWidths)
{
    if (_lineWidths == newLineWidths)
        return;

    _lineWidths = newLineWidths;
    emit lineWidthsChanged();
}

Monitor *PerformanceChartBase::monitor() const
{
    return _monitor;
}

void PerformanceChartBase::setMonitor(Monitor *newMonitor)
{
    if (_monitor == newMonitor)
        return;

    if (_monitor) {
        disconnect(_monitor, &Monitor::valuesAvailable, this, &PerformanceChartBase::pushValues);
        _monitor->unsubscribe(this);
    }

    _monitor = newMonitor;
    emit monitorChanged();

    if (_monitor) {
        connect(_monitor, &Monitor::valuesAvailable, this, &PerformanceChartBase::pushValues);
        _monitor->subscribe(this);
    }
}
