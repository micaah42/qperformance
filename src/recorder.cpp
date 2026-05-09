#include "recorder.h"

#include <QIODevice>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrent>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.recorder")
}

const unsigned char QPerfDataPointsMagicData[]{
    0xB2,
    0x28,
    0x09,
    0x5B,
    0x30,
    0x52,
    0x45,
    0x39,
    0x83,
    0xB3,
    0x84,
    0x76,
    0x1C,
    0x9E,
    0x02,
    0x59,
};

const QByteArray Recorder::QPerfDataPointsMagic{
    reinterpret_cast<const char *>(QPerfDataPointsMagicData),
    sizeof(QPerfDataPointsMagic),
};

Recorder::DataPoints Recorder::readDataPoints(QIODevice *file)
{
    auto bytes = file->read(QPerfDataPointsMagic.size());
    if (bytes != QPerfDataPointsMagic) {
        qCCritical(self) << "invalid magic, got" << bytes.toHex('.') << "expected" << QPerfDataPointsMagic.toHex('.');
        return {};
    }

    bytes = file->readAll();
    bytes = qUncompress(bytes);

    QDataStream stream{bytes};

    qint64 nDataPoints;
    stream >> nDataPoints;
    qCDebug(self) << "n data points:" << nDataPoints;

    qint64 nValuesPerPoint;
    stream >> nValuesPerPoint;
    qCDebug(self) << "values per point:" << nValuesPerPoint;

    DataPoints dataPoints;
    dataPoints.reserve(nDataPoints);

    for (int i = 0; i < nDataPoints; i++) {
        DataPoint dataPoint;
        dataPoint.values.reserve(nValuesPerPoint);

        stream >> dataPoint.timestamp;

        for (int j = 0; j < nValuesPerPoint; j++) {
            double value;
            stream >> value;
            dataPoint.values.append(value);
        }

        dataPoints.append(std::move(dataPoint));
    }

    return dataPoints;
}

void Recorder::writeDataPoints(QIODevice *file, const DataPoints &dataPoints)
{
    if (dataPoints.empty())
        return;

    QByteArray bytes;

    {
        QDataStream stream{&bytes, QIODevice::WriteOnly};

        stream << static_cast<qint64>(dataPoints.size());
        stream << static_cast<qint64>(dataPoints.first().values.size());

        for (int i = 0; i < dataPoints.size(); i++) {
            auto const &dataPoint = dataPoints[i];
            stream << dataPoint.timestamp;

            for (int j = 0; j < dataPoint.values.size(); j++)
                stream << dataPoint.values[j];
        }
    }

    bytes = qCompress(bytes);
    bytes = QPerfDataPointsMagic + bytes;
    file->write(bytes);
}

Recorder::Recorder(QObject *parent)
    : QObject{parent}
    , _monitor{nullptr}
    , _flushInterval{15 * 1000}
    , _flushing{false}
{
    _flushTimer.callOnTimeout(this, &Recorder::flush);
    _flushTimer.setInterval(_flushInterval);
    _flushTimer.setTimerType(Qt::CoarseTimer);
    _flushTimer.start();
}

QString Recorder::fileName() const
{
    return _fileName;
}

void Recorder::setFileName(const QString &newFileName)
{
    if (_fileName == newFileName)
        return;
    _fileName = newFileName;
    emit fileNameChanged();
}

Monitor *Recorder::monitor() const
{
    return _monitor;
}

void Recorder::setMonitor(Monitor *newMonitor)
{
    if (_monitor == newMonitor)
        return;

    if (_monitor) {
        disconnect(_monitor, &Monitor::valuesAvailable, this, &Recorder::pushValues);
        _monitor->unsubscribe(this);
    }

    _monitor = newMonitor;
    emit monitorChanged();

    if (_monitor) {
        connect(_monitor, &Monitor::valuesAvailable, this, &Recorder::pushValues);
        _monitor->subscribe(this);
    }
}

void Recorder::pushValues2(const QList<double> &values, qint64 t)
{
    _dataPoints.append({values, t});
}

void Recorder::pushValues(const QList<double> &values)
{
    _dataPoints.append({values, QDateTime::currentMSecsSinceEpoch()});
}

void Recorder::flush()
{
    this->setFileName(QString{"./recording%1.qpdp"}.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")));

    if (_flushing)
        return;

    if (_fileName.isEmpty() || _dataPoints.isEmpty())
        return;

    this->setFlushing(true);
    _flushTimer.start();

    DataPoints dataToWrite = std::move(_dataPoints);
    _dataPoints.clear();

    const QString fileName = _fileName;

    auto future = QtConcurrent::run([this, dataToWrite = std::move(dataToWrite), fileName]() {
        QFile file(fileName);

        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            writeDataPoints(&file, dataToWrite);
            file.close();
        } else
            qCCritical(self) << "failed to open file for writing:" << fileName;

        if (file.open(QIODevice::ReadOnly)) {
            auto data = readDataPoints(&file);
            file.close();

            if (data != dataToWrite) {
                qCWarning(self) << "recode error!";
                qCInfo(self) << dataToWrite;
                qCInfo(self) << data;
            }
        }
        QMetaObject::invokeMethod(this, [this]() { this->setFlushing(false); }, Qt::QueuedConnection);
    });

    Q_UNUSED(future);
}
int Recorder::flushInterval() const
{
    return _flushInterval;
}

void Recorder::setFlushInterval(int newFlushInterval)
{
    if (_flushInterval == newFlushInterval)
        return;
    _flushInterval = newFlushInterval;
    emit flushIntervalChanged();
}

bool Recorder::flushing() const
{
    return _flushing;
}

void Recorder::setFlushing(bool newFlushing)
{
    if (_flushing == newFlushing)
        return;

    qCInfo(self) << this << "flushing" << newFlushing;
    _flushing = newFlushing;
    emit flushingChanged();
}

QDebug operator<<(QDebug dbg, const Recorder::DataPoint &dp)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace();

    dbg << "DataPoint(timestamp=" << dp.timestamp << ", values=[";

    for (int i = 0; i < dp.values.size(); ++i) {
        if (i > 0)
            dbg << ", ";
        dbg << dp.values[i];
    }

    dbg << "])";

    return dbg;
}
