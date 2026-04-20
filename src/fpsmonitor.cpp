#include "fpsmonitor.h"

#include <QGuiApplication>

FPSMonitor::FPSMonitor(QObject *parent)
    : Monitor{parent}
    , _fps{0}
    , _avgFps{0}
    , _avgLength{15}
{
    this->useApplicationWindow();
}

double FPSMonitor::fps() const
{
    return _fps;
}

void FPSMonitor::setFps(double newFps)
{
    if (qFuzzyCompare(_fps, newFps))
        return;

    _fps = newFps;
    emit fpsChanged();
}

double FPSMonitor::avgFps() const
{
    return _avgFps;
}

void FPSMonitor::setAvgFps(double newAvgFps)
{
    if (qFuzzyCompare(_avgFps, newAvgFps))
        return;

    _avgFps = newAvgFps;
    emit avgFpsChanged();
}

int FPSMonitor::avgLength() const
{
    return _avgLength;
}

void FPSMonitor::setAvgLength(int newAvgLength)
{
    if (_avgLength == newAvgLength)
        return;

    _avgLength = newAvgLength;
    emit avgLengthChanged();
}

QQuickWindow *FPSMonitor::window() const
{
    return _window;
}

void FPSMonitor::setWindow(QQuickWindow *newWindow)
{
    if (_window == newWindow)
        return;

    if (_window)
        disconnect(_window, &QQuickWindow::frameSwapped, this, &FPSMonitor::onFrameSwapped);

    _window = newWindow;
    emit windowChanged();

    if (_window && this->active())
        connect(_window, &QQuickWindow::frameSwapped, this, &FPSMonitor::onFrameSwapped);
}

void FPSMonitor::useApplicationWindow()
{
    const auto windows = QGuiApplication::allWindows();

    for (auto const window : windows) {
        auto quickWindow = qobject_cast<QQuickWindow *>(window);
        if (quickWindow) {
            this->setWindow(quickWindow);
            return;
        }
    }
}

void FPSMonitor::deactivate()
{
    disconnect(_window, &QQuickWindow::frameSwapped, this, &FPSMonitor::onFrameSwapped);
}

void FPSMonitor::activate()
{
    connect(_window, &QQuickWindow::frameSwapped, this, &FPSMonitor::onFrameSwapped);
}

void FPSMonitor::onFrameSwapped()
{
    double deltaMs = _elapsedTimer.restart();

    if (deltaMs == 0)
        deltaMs = 0.1;

    this->setFps(1000. / deltaMs);

    double dequeueValue = 0;

    while (_fpsAvgQueue.size() > _avgLength)
        dequeueValue += _fpsAvgQueue.takeFirst();

    double queueValue = _fps / _avgLength;
    _fpsAvgQueue.append(queueValue);


    this->setAvgFps(_avgFps + queueValue - dequeueValue);

    emit valuesAvailable({_fps, _avgFps});
}

Debouncer::Debouncer(QObject *parent)
    : QObject{parent}
    , _input{0}
    , _output{0}
{
    _timer.callOnTimeout([this]() { this->setOutput(_input); });
    _timer.setTimerType(Qt::PreciseTimer);
    _timer.setSingleShot(true);
    _timer.setInterval(100);
}

double Debouncer::input() const
{
    return _input;
}

void Debouncer::setInput(double newInput)
{
    if (qFuzzyCompare(_input, newInput))
        return;

    _input = newInput;
    emit inputChanged();

    if (!_timer.isActive())
        _timer.start();
}

double Debouncer::output() const
{
    return _output;
}

void Debouncer::setOutput(double newOutput)
{
    if (qFuzzyCompare(_output, newOutput))
        return;
    _output = newOutput;
    emit outputChanged();
}

int Debouncer::interval() const
{
    return _timer.interval();
}

void Debouncer::setInterval(int newInterval)
{
    if (_timer.interval() == newInterval)
        return;
    _timer.setInterval(newInterval);
    emit intervalChanged();
}
