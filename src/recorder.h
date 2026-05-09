#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QQmlEngine>
#include <QTimer>

#include "monitor.h"

class Recorder : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int flushInterval READ flushInterval WRITE setFlushInterval NOTIFY flushIntervalChanged FINAL)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(Monitor *monitor READ monitor WRITE setMonitor NOTIFY monitorChanged FINAL)
    Q_PROPERTY(bool flushing READ flushing NOTIFY flushingChanged FINAL)

public:
    struct DataPoint
    {
        QList<double> values;
        qint64 timestamp;

        bool operator==(const DataPoint &other) const { return timestamp == other.timestamp && values == other.values; }
    };

    using DataPoints = QList<DataPoint>;

    static const QByteArray QPerfDataPointsMagic;
    static DataPoints readDataPoints(QIODevice *file);
    static void writeDataPoints(QIODevice *file, const DataPoints &dataPoints);

    explicit Recorder(QObject *parent = nullptr);

    QString fileName() const;
    void setFileName(const QString &newFileName);

    Monitor *monitor() const;
    void setMonitor(Monitor *newMonitor);

    int flushInterval() const;
    void setFlushInterval(int newFlushInterval);

    bool flushing() const;

public slots:
    /**
     * @brief Adds new data values to the chart at the given timestamp.
     *
     * @param values A list of new values to append. Each entry corresponds to a different series.
     * @param t The timestamp associated with the new data (in milliseconds since epoch or arbitrary reference).
     */
    void pushValues2(const QList<double> &values, qint64 t);
    void pushValues(const QList<double> &values);

    void flush();

signals:
    void fileNameChanged();
    void monitorChanged();
    void flushIntervalChanged();
    void flushingChanged();

protected:
    void setFlushing(bool newFlushing);

private:
    DataPoints _dataPoints;
    QString _fileName;
    Monitor *_monitor;
    QTimer _flushTimer;
    int _flushInterval;
    bool _flushing;
};

#include <QDebug>

QDebug operator<<(QDebug dbg, const Recorder::DataPoint &dp);

#endif // RECORDER_H
