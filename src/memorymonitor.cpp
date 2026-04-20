#include "memorymonitor.h"

#include <QFile>
#include <QLoggingCategory>

#include "utility.h"

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.memory", QtInfoMsg);
}

MemoryMonitor::MemoryMonitor(QObject *parent)
    : PollingMonitor{parent, 500}
    , _memAvailable{0}
    , _memTotal{0}
    , _swapTotal{0}
    , _swapFree{0}
    , _memUsed{0}
    , _swapUsed{0}
{
    MemoryMonitor::onTimeout();
}

quint64 parseKB(const QByteArray &bytes)
{
    if (!bytes.endsWith(" kB")) {
        return 0;
    }

    auto numberBytes = bytes.chopped(3);

    bool ok;
    auto number = numberBytes.toULongLong(&ok);

    if (!ok) {
        return 0;
    }

    return number;
}

void MemoryMonitor::onTimeout()
{
    /*
        MemTotal:       32739040 kB
        MemFree:        27134804 kB
        MemAvailable:   29227040 kB
        Buffers:          336844 kB
        Cached:          2181636 kB
        SwapCached:            0 kB
        Active:          3427852 kB
        Inactive:         973368 kB
        Active(anon):    2023596 kB
        Inactive(anon):        0 kB
        Active(file):    1404256 kB
        Inactive(file):   973368 kB
        Unevictable:          16 kB
        Mlocked:              16 kB
        SwapTotal:             0 kB
        SwapFree:              0 kB
        Zswap:                 0 kB
        Zswapped:              0 kB
        Dirty:                56 kB
        Writeback:             0 kB
        AnonPages:       1882940 kB
        Mapped:           905352 kB
        Shmem:            140824 kB
        KReclaimable:     156728 kB
        Slab:             488316 kB
        SReclaimable:     156728 kB
        SUnreclaim:       331588 kB
        KernelStack:       16560 kB
        PageTables:        30060 kB
        SecPageTables:         0 kB
        NFS_Unstable:          0 kB
        Bounce:                0 kB
        WritebackTmp:          0 kB
        CommitLimit:    16369520 kB
        Committed_AS:    9562764 kB
        VmallocTotal:   34359738367 kB
        VmallocUsed:      125580 kB
        VmallocChunk:          0 kB
        Percpu:            36864 kB
        HardwareCorrupted:     0 kB
        AnonHugePages:         0 kB
        ShmemHugePages:        0 kB
        ShmemPmdMapped:        0 kB
        FileHugePages:         0 kB
        FilePmdMapped:         0 kB
        Unaccepted:            0 kB
        HugePages_Total:       0
        HugePages_Free:        0
        HugePages_Rsvd:        0
        HugePages_Surp:        0
        Hugepagesize:       2048 kB
        Hugetlb:               0 kB
        DirectMap4k:      416948 kB
        DirectMap2M:     5756928 kB
        DirectMap1G:    27262976 kB
    */

    QFile procMeminfo{"/proc/meminfo"};

    if (!procMeminfo.open(QIODevice::ReadOnly)) {
        qCCritical(self) << "failed to open file:" << procMeminfo.fileName() << procMeminfo.errorString();
        return;
    }

    auto bytes = procMeminfo.readAll();
    auto meminfo = utility::parseMap(bytes);

    this->setMemAvailable(parseKB(meminfo["MemAvailable"]));
    this->setMemTotal(parseKB(meminfo["MemTotal"]));

    this->setSwapTotal(parseKB(meminfo["SwapTotal"]));
    this->setSwapFree(parseKB(meminfo["SwapFree"]));

    this->setMemUsed(_memTotal - _memAvailable);
    double memUsage = _memTotal ? (double) _memUsed / (double) _memTotal : 0;

    this->setSwapUsed(_swapTotal - _swapFree);
    double swapUsage = _swapTotal ? (double) _swapUsed / (double) _swapTotal : 0;

    emit valuesAvailable({memUsage, swapUsage});
}

int MemoryMonitor::memAvailable() const
{
    return _memAvailable;
}

void MemoryMonitor::setMemAvailable(int newMemAvailable)
{
    if (_memAvailable == newMemAvailable)
        return;
    _memAvailable = newMemAvailable;
    emit memAvailableChanged();
}

int MemoryMonitor::memTotal() const
{
    return _memTotal;
}

void MemoryMonitor::setMemTotal(int newMemTotal)
{
    if (_memTotal == newMemTotal)
        return;
    _memTotal = newMemTotal;
    emit memTotalChanged();
}

int MemoryMonitor::swapTotal() const
{
    return _swapTotal;
}

void MemoryMonitor::setSwapTotal(int newSwapTotal)
{
    if (_swapTotal == newSwapTotal)
        return;
    _swapTotal = newSwapTotal;
    emit swapTotalChanged();
}

int MemoryMonitor::swapFree() const
{
    return _swapFree;
}

void MemoryMonitor::setSwapFree(int newSwapFree)
{
    if (_swapFree == newSwapFree)
        return;
    _swapFree = newSwapFree;
    emit swapFreeChanged();
}

int MemoryMonitor::memUsed() const
{
    return _memUsed;
}

void MemoryMonitor::setMemUsed(int newMemUsed)
{
    if (_memUsed == newMemUsed)
        return;
    _memUsed = newMemUsed;
    emit memUsedChanged();
}

int MemoryMonitor::swapUsed() const
{
    return _swapUsed;
}

void MemoryMonitor::setSwapUsed(int newSwapUsed)
{
    if (_swapUsed == newSwapUsed)
        return;

    _swapUsed = newSwapUsed;
    emit swapUsedChanged();
}
