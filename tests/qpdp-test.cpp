#include <QtTest/QTest>

#include <QBuffer>

#include "recorder.h"

class QPDPTest : public QObject
{
    Q_OBJECT

private slots:

    void testRoundTrip()
    {
        Recorder::DataPoint p1;
        p1.timestamp = 1000;
        p1.values = {1.5, 2.5, 3.5};

        Recorder::DataPoint p2;
        p2.timestamp = 2000;
        p2.values = {10.1, 20.2, 30.3};

        Recorder::DataPoints originalData = {p1, p2};

        // 2. Write to buffer
        QByteArray bufferData;
        QBuffer buffer(&bufferData);
        buffer.open(QIODevice::WriteOnly);

        Recorder recorder;
        recorder.writeDataPoints(&buffer, originalData);
        buffer.close();

        // 3. Read from buffer
        buffer.open(QIODevice::ReadOnly);
        Recorder::DataPoints readData = recorder.readDataPoints(&buffer);
        buffer.close();

        // 4. Verification
        QCOMPARE(readData.size(), originalData.size());
        for (int i = 0; i < originalData.size(); ++i) {
            QCOMPARE(readData[i].timestamp, originalData[i].timestamp);
            QCOMPARE(readData[i].values, originalData[i].values);
        }
    }

    void testInvalidMagic()
    {
        QByteArray corruptData = "ThisIsClearlyNotTheRightMagicHeader!!";
        QBuffer buffer(&corruptData);
        buffer.open(QIODevice::ReadOnly);

        Recorder recorder;
        Recorder::DataPoints result = recorder.readDataPoints(&buffer);

        // Should return an empty list because the magic check fails
        QVERIFY(result.isEmpty());
    }

    void testTruncatedFile()
    {
        // Only write the magic number and no compressed data
        QByteArray truncatedData = Recorder::QPerfDataPointsMagic;
        QBuffer buffer(&truncatedData);
        buffer.open(QIODevice::ReadOnly);

        Recorder recorder;
        // qUncompress will fail or the stream will be empty
        Recorder::DataPoints result = recorder.readDataPoints(&buffer);

        QVERIFY(result.isEmpty());
    }

    void testEmptyDataPoints()
    {
        QByteArray bufferData;
        QBuffer buffer(&bufferData);
        buffer.open(QIODevice::ReadWrite);

        Recorder recorder;
        Recorder::DataPoints emptySet;

        recorder.writeDataPoints(&buffer, emptySet);

        // Based on your code: if empty, it returns early without writing anything
        QCOMPARE(buffer.size(), 0);
    }

    void testLargeDataSet()
    {
        // Generate a large dataset with 10000+ dynamically created datapoints
        const int dataPointCount = 15000;
        const int seriesCount = 5;
        
        Recorder::DataPoints originalData;
        originalData.reserve(dataPointCount);

        // Dynamically generate datapoints
        for (int i = 0; i < dataPointCount; ++i) {
            Recorder::DataPoint point;
            point.timestamp = i * 100; // 100ms intervals
            point.values.reserve(seriesCount);
            
            // Generate different values for each series
            for (int j = 0; j < seriesCount; ++j) {
                point.values.append(i * 1.5 + j * 0.5);
            }
            
            originalData.append(point);
        }

        // Write to buffer
        QByteArray bufferData;
        QBuffer buffer(&bufferData);
        buffer.open(QIODevice::WriteOnly);

        Recorder recorder;
        recorder.writeDataPoints(&buffer, originalData);
        buffer.close();

        // Read from buffer
        buffer.open(QIODevice::ReadOnly);
        Recorder::DataPoints readData = recorder.readDataPoints(&buffer);
        buffer.close();

        // Verification
        QCOMPARE(readData.size(), originalData.size());
        QCOMPARE(readData.size(), dataPointCount);
        
        // Spot check: verify first, middle, and last datapoints
        QCOMPARE(readData[0].timestamp, originalData[0].timestamp);
        QCOMPARE(readData[0].values, originalData[0].values);
        
        int midpoint = dataPointCount / 2;
        QCOMPARE(readData[midpoint].timestamp, originalData[midpoint].timestamp);
        QCOMPARE(readData[midpoint].values, originalData[midpoint].values);
        
        QCOMPARE(readData[dataPointCount - 1].timestamp, originalData[dataPointCount - 1].timestamp);
        QCOMPARE(readData[dataPointCount - 1].values, originalData[dataPointCount - 1].values);
    }
};

#include "qpdp-test.moc"

QTEST_MAIN(QPDPTest)
