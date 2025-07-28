#ifndef PERFORMANCECHART_H
#define PERFORMANCECHART_H

#include <QColor>
#include <QElapsedTimer>
#include <QImage>
#include <QQuickPaintedItem>
#include <QTimer>

#include <deque>

class PerformanceChartBase : public QQuickPaintedItem
{
    Q_OBJECT
    QML_UNCREATABLE("PerformanceChartBase is Abstract")

    /**
     * @brief Duration (in seconds) of the time window displayed on the chart.
     */
    Q_PROPERTY(double duration READ duration WRITE setDuration NOTIFY durationChanged FINAL)

    /**
     * @brief List of colors used to render multiple data series.
     */
    Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors NOTIFY colorsChanged FINAL)

    Q_PROPERTY(double thickness READ thickness WRITE setThickness NOTIFY thicknessChanged FINAL)

    Q_PROPERTY(int maxUpdateInterval READ maxUpdateInterval WRITE setMaxUpdateInterval NOTIFY maxUpdateIntervalChanged FINAL)

public:
    PerformanceChartBase();

    double duration() const;
    void setDuration(double newDuration);

    const QList<QColor> &colors() const;
    void setColors(const QList<QColor> &newColors);

    double thickness() const;
    void setThickness(double newThickness);

    int maxUpdateInterval() const;
    void setMaxUpdateInterval(int newMaxUpdateInterval);

public slots:
    /**
     * @brief Adds new data values to the chart at the given timestamp.
     *
     * @param values A list of new values to append. Each entry corresponds to a different series.
     * @param t The timestamp associated with the new data (in milliseconds since epoch or arbitrary reference).
     */
    virtual void pushValues2(const QList<double> &values, qint64 t) = 0;
    void pushValues(const QList<double> &values);
    void requestUpdate();

    /**
     * @brief Clears all existing data and resets the chart to an empty state.
     */
    virtual void reset() = 0;

signals:
    void durationChanged();
    void colorsChanged();

    void thicknessChanged();

    void maxUpdateIntervalChanged();

protected:
    qint64 elapsedTime() const;
    QColor color(int i) const;

private:
    double _duration;
    QList<QColor> _colors;
    double _thickness;
    QTimer _updateTimer;
    QElapsedTimer _elapsedTimer;
};

/**
 * @class LongTimeChart
 * @brief A QML-compatible chart widget for displaying long time-series data efficiently.
 *
 * LongTimeChart is a QQuickPaintedItem that visualizes one or more data series over a user-defined duration.
 * It optimizes performance by only redrawing the most recent portion of the data when new values are added.
 * Although it uses some inefficient image copying internally, this tradeoff can improve rendering performance
 * for high-frequency data visualization.
 */
class LongTimeChart : public PerformanceChartBase
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @brief Duration (in seconds) of the time window displayed on the chart.
     */
    Q_PROPERTY(double duration READ duration WRITE setDuration NOTIFY durationChanged FINAL)

    /**
     * @brief List of colors used to render multiple data series.
     */
    Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors NOTIFY colorsChanged FINAL)

public:
    LongTimeChart();

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
