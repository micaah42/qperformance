#ifndef FPSMONITOR_H
#define FPSMONITOR_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>

#include "monitor.h"

class Debouncer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
    Q_PROPERTY(double input READ input WRITE setInput NOTIFY inputChanged FINAL)
    Q_PROPERTY(double output READ output NOTIFY outputChanged FINAL)

public:
    explicit Debouncer(QObject *parent = nullptr);
    double input() const;
    void setInput(double newInput);

    int interval() const;
    void setInterval(int newInterval);

    double output() const;

signals:
    void inputChanged();
    void outputChanged();

    void intervalChanged();

protected:
    void setOutput(double newOutput);

private:
    QTimer _timer;
    double _input;
    double _output;
};

class FPSMonitor : public Monitor
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(double fps READ fps WRITE setFps NOTIFY fpsChanged FINAL)
    Q_PROPERTY(double avgFps READ avgFps WRITE setAvgFps NOTIFY avgFpsChanged FINAL)
    Q_PROPERTY(int avgLength READ avgLength WRITE setAvgLength NOTIFY avgLengthChanged FINAL)
    Q_PROPERTY(QQuickWindow *window READ window WRITE setWindow NOTIFY windowChanged FINAL)

public:
    explicit FPSMonitor(QObject *parent = nullptr);

    double fps() const;
    void setFps(double newFps);

    double avgFps() const;
    void setAvgFps(double newAvgFps);

    int avgLength() const;
    void setAvgLength(int newAvgLength);

    QQuickWindow *window() const;
    void setWindow(QQuickWindow *newWindow);

public slots:
    void useApplicationWindow();

signals:
    void fpsChanged();
    void avgFpsChanged();
    void avgLengthChanged();
    void windowChanged();

protected:
    virtual void deactivate() override;
    virtual void activate() override;

    void onFrameSwapped();

private:
    double _fps;
    double _avgFps;
    int _avgLength;
    QElapsedTimer _elapsedTimer;
    QPointer<QQuickWindow> _window;
    QList<double> _fpsAvgQueue;
};

#endif // FPSMONITOR_H
