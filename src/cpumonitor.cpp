#include "cpumonitor.h"

#include <QFile>
#include <QLoggingCategory>

#ifdef Q_PROCESSOR_X86_64
#define ARCHITECTURE "x86_64"
#elif defined(Q_PROCESSOR_ARM)
#define ARCHITECTURE "arm"
#else
#define ARCHITECTURE "unknown"
#endif

namespace {
Q_LOGGING_CATEGORY(self, "cpu", QtInfoMsg);
}

CPUCore::CPUCore(const QString &name, QObject *parent)
    : QObject{parent}
    , _totalTime{0}
    , _activeTime{0}
    , _load{0}
    , _name{name}
{}

const QString &CPUCore::name() const
{
    return _name;
}

double CPUCore::load() const
{
    return _load;
}

void CPUCore::setLoad(double newLoad)
{
    if (qFuzzyCompare(_load, newLoad))
        return;

    _load = newLoad;
    emit loadChanged();
}

CPUMonitor::CPUMonitor(QObject *parent)
    : Monitor{parent}
    , _temperature{0}
{
    this->setInterval(100);
    CPUMonitor::onTimeout();

    // Init Constant Properties like architecture and model name

    QFile procCpuInfo{"/proc/cpuinfo"};

    if (!procCpuInfo.open(QIODevice::ReadOnly)) {
        qCCritical(self) << "failed to open file:" << procCpuInfo.fileName() << procCpuInfo.errorString();
        return;
    }

    auto lines = procCpuInfo.readAll().split('\n');

    for (auto const &line : std::as_const(lines)) {
        auto split = line.split(':');

        if (split.size() < 2) {
            continue;
        }

        auto key = split[0];
        auto value = split.mid(1).join(':');

        if (key.startsWith("vendor_id"))
            _vendor = value.simplified();
        else if (key.startsWith("model name"))
            _model = value.simplified();
    }

    _architecture = ARCHITECTURE;
    qCInfo(self) << "initialized monitor" << this;
}

void CPUMonitor::onTimeout()
{
    qCDebug(self) << this << "parsing files...";

    /*
    * Determine CPU load by parsing unix system file
    * The contents of Proc Stat Look Somewhat like this:
    * 
    *      User  Nice System Idle Iowait Irq Softirq Steal Guest GuestNice
    * cpu  341115 89402 96203 10267229 5580 0 2037 0 0 0
    * cpu0 30647 9763 8353 850563 586 0 90 0 0 0
    * ...
    * ...
    * cpuN 30647 9763 8353 850563 586 0 90 0 0 0
    * intr ...
    * ...
    * ...
    */

    QFile procStat("/proc/stat");
    if (!procStat.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/stat";
        return;
    }

    bool coresAdded = false;
    auto lines = procStat.readAll().split('\n');

    _usages.clear();
    _usages.reserve(_cores.size());

    for (int i = 0; i < lines.size(); i++) {
        auto line = lines[i];

        if (!line.startsWith("cpu"))
            break;

        auto words = line.simplified().split(' ');

        if (words.size() != 11) {
            qCCritical(self) << "unexpected number of words in line:" << line << words.size();
            continue;
        }

        while (_cores.size() <= i) {
            _cores.append(new CPUCore{words[0], this});
            coresAdded = true;
        }
        auto core = _cores[i];

        quint64 totalTime = 0;
        for (int j = 1; j < 11; ++j) {
            totalTime += words[j].toULongLong();
        }

        quint64 idleTime = words[4].toULongLong();
        quint64 activeTime = totalTime - idleTime;

        double totalDelta = totalTime - core->_totalTime;
        double activeDelta = activeTime - core->_activeTime;

        core->setLoad((100 * activeDelta) / totalDelta);
        core->_totalTime = totalTime;
        core->_activeTime = activeTime;
        _usages.append(core->load());
    }

    emit usagesChanged();

    if (coresAdded) {
        emit coresChanged();
    }

    /*
    * Determine CPU temperature by reading a system file with a simple number
    */

    QFile tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (!tempFile.open(QIODevice::ReadOnly)) {
        // qCWarning(self) << "failed to open thermal zone file:" << tempFile.fileName() << tempFile.errorString();
        return;
    }

    this->setTemperature(tempFile.readAll().toInt() / 1000);
}

QList<CPUCore *> CPUMonitor::cores() const
{
    return _cores;
}

int CPUMonitor::temperature() const
{
    return _temperature;
}

void CPUMonitor::setTemperature(int temperature)
{
    if (_temperature == temperature) {
        return;
    }

    _temperature = temperature;
    emit temperatureChanged();
}

QString CPUMonitor::architecture() const
{
    return _architecture;
}

QString CPUMonitor::model() const
{
    return _model;
}

QString CPUMonitor::vendor() const
{
    return _vendor;
}

CPUCore *CPUMonitor::total() const
{
    return _cores.isEmpty() ? nullptr : _cores.first();
}

QList<double> CPUMonitor::usages() const
{
    return _usages;
}

void CPUMonitor::setUsages(const QList<double> &newUsages)
{
    if (_usages == newUsages)
        return;

    _usages = newUsages;
    emit usagesChanged();
}
