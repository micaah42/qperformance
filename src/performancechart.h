#ifndef PERFORMANCECHART_H
#define PERFORMANCECHART_H

#include "performancechartbase.h"

#include <deque>

/**
 * @class PerformanceChart
 * @brief A QML-compatible chart widget for displaying long time-series data efficiently.
 *
 * LongTimeChart is a QQuickPaintedItem that visualizes one or more data series over a user-defined duration.
 * It optimizes performance by only redrawing the most recent portion of the data when new values are added.
 * Although it uses some inefficient image copying internally, this tradeoff can improve rendering performance
 * for high-frequency data visualization.
 */
class PerformanceChart : public PerformanceChartBase
{
    Q_OBJECT
    QML_ELEMENT

public:
    PerformanceChart();

public slots:
    virtual void pushValues2(const QList<double> &values, qint64 t) override;
    virtual void reset() override;

protected:
    virtual void paint(QPainter *painter) override;

private:
    std::deque<QList<double>> _values;
    std::deque<qint64> _ts;
};

#endif // PERFORMANCECHART_H
