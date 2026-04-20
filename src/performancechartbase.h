#ifndef PERFORMANCECHARTBASE_H
#define PERFORMANCECHARTBASE_H

#include <QQuickPaintedItem>
#include <QTimer>

#include "monitor.h"

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

    Q_PROPERTY(QList<double> lineWidths READ lineWidths WRITE setLineWidths NOTIFY lineWidthsChanged FINAL)

    /**
     * @brief Redraws happen at most every maxUpdateInterval milliseconds or when new data is ready
     */
    Q_PROPERTY(int maxUpdateInterval READ maxUpdateInterval WRITE setMaxUpdateInterval NOTIFY maxUpdateIntervalChanged FINAL)

    Q_PROPERTY(double min READ min WRITE setMin NOTIFY minChanged FINAL)
    Q_PROPERTY(double max READ max WRITE setMax NOTIFY maxChanged FINAL)

    Q_PROPERTY(Monitor *monitor READ monitor WRITE setMonitor NOTIFY monitorChanged FINAL)

public:
    PerformanceChartBase();
    ~PerformanceChartBase();

    double duration() const;
    void setDuration(double newDuration);

    const QList<QColor> &colors() const;
    void setColors(const QList<QColor> &newColors);

    int maxUpdateInterval() const;
    void setMaxUpdateInterval(int newMaxUpdateInterval);

    double min() const;
    void setMin(double newMin);

    double max() const;
    void setMax(double newMax);

    QList<double> thickness() const;
    void setThickness(const QList<double> &newThickness);

    QList<double> lineWidths() const;
    void setLineWidths(const QList<double> &newLineWidths);

    Monitor *monitor() const;
    void setMonitor(Monitor *newMonitor);

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
    void maxUpdateIntervalChanged();
    void lineWidthsChanged();
    void minChanged();
    void maxChanged();
    void monitorChanged();

protected:
    qint64 elapsedTime() const;
    double lineWidth(int i) const;
    QColor color(int i) const;

private:
    double _min;
    double _max;

    double _duration;
    QList<QColor> _colors;
    QList<double> _lineWidths;

    QTimer _updateTimer;
    QElapsedTimer _elapsedTimer;
    QPointer<Monitor> _monitor;
};

#endif // PERFORMANCECHARTBASE_H
