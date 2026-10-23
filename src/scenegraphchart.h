// SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SCENEGRAPHCHART_H
#define SCENEGRAPHCHART_H

#include <QColor>
#include <QElapsedTimer>
#include <QPointer>
#include <QQuickItem>
#include <QTimer>

#include <deque>

#include "monitor.h"

/**
 * @class SceneGraphChart
 * @brief Time-series chart rendered directly through the Qt Quick scene graph (GPU geometry, no QPainter).
 *
 * Offers the same properties as PerformanceChart. Each series is drawn as one triangle strip
 * with a flat color material.
 */
class SceneGraphChart : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double duration READ duration WRITE setDuration NOTIFY durationChanged FINAL)
    Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors NOTIFY colorsChanged FINAL)
    Q_PROPERTY(QList<double> lineWidths READ lineWidths WRITE setLineWidths NOTIFY lineWidthsChanged FINAL)
    Q_PROPERTY(int maxUpdateInterval READ maxUpdateInterval WRITE setMaxUpdateInterval NOTIFY maxUpdateIntervalChanged FINAL)
    Q_PROPERTY(double min READ min WRITE setMin NOTIFY minChanged FINAL)
    Q_PROPERTY(double max READ max WRITE setMax NOTIFY maxChanged FINAL)
    Q_PROPERTY(double minValue READ minValue NOTIFY minValueChanged FINAL)
    Q_PROPERTY(double maxValue READ maxValue NOTIFY maxValueChanged FINAL)
    Q_PROPERTY(Monitor *monitor READ monitor WRITE setMonitor NOTIFY monitorChanged FINAL)

public:
    SceneGraphChart();
    ~SceneGraphChart() override;

    double duration() const { return _duration; }
    void setDuration(double v);
    const QList<QColor> &colors() const { return _colors; }
    void setColors(const QList<QColor> &v);
    QList<double> lineWidths() const { return _lineWidths; }
    void setLineWidths(const QList<double> &v);
    int maxUpdateInterval() const { return _updateTimer.interval(); }
    void setMaxUpdateInterval(int v);
    double min() const { return _min; }
    void setMin(double v);
    double max() const { return _max; }
    void setMax(double v);
    double minValue() const { return _minValue; }
    double maxValue() const { return _maxValue; }
    Monitor *monitor() const { return _monitor; }
    void setMonitor(Monitor *m);

public slots:
    void pushValues(const QList<double> &values);
    void pushValues2(const QList<double> &values, qint64 t);
    void reset();

signals:
    void durationChanged();
    void colorsChanged();
    void lineWidthsChanged();
    void maxUpdateIntervalChanged();
    void minChanged();
    void maxChanged();
    void minValueChanged();
    void maxValueChanged();
    void monitorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    void requestUpdate();
    void updateRange();

    double _min = 0;
    double _max = 100;
    double _duration = 5000;
    QList<QColor> _colors{Qt::white};
    QList<double> _lineWidths{1.};
    double _minValue = std::numeric_limits<double>::infinity();
    double _maxValue = -std::numeric_limits<double>::infinity();

    QTimer _updateTimer;
    QElapsedTimer _elapsedTimer;
    QPointer<Monitor> _monitor;

    std::deque<QList<double>> _values;
    std::deque<qint64> _ts;
};

#endif // SCENEGRAPHCHART_H
