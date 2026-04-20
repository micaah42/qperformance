#ifndef LAZYREDRAWCHART_H
#define LAZYREDRAWCHART_H

#include <QImage>

#include "performancechartbase.h"

/**
 * @class LazyRedrawChart
 * @brief A QML-compatible chart widget for displaying long time-series data efficiently.
 *
 * LazyRedrawChart is a QQuickPaintedItem that visualizes one or more data series over a user-defined duration.
 * It optimizes performance by only redrawing the most recent portion of the data when new values are added.
 * Although it uses some inefficient image copying internally, this tradeoff can improve rendering performance
 * for high-frequency data visualization.
 */
class LazyRedrawChart : public PerformanceChartBase
{
    Q_OBJECT
    QML_ELEMENT

public:
    LazyRedrawChart();

public slots:
    virtual void pushValues2(const QList<double> &values, qint64 t) override;
    virtual void reset() override;

protected:
    virtual void paint(QPainter *painter) override;

private:
    QList<double> _values;
    double _t;

    QList<double> _prevValues;
    double _prevT;
    QImage _image;
};

#endif // LAZYREDRAWCHART_H
