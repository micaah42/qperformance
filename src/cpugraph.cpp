#include "cpugraph.h"

#include <QDateTime>
#include <QLoggingCategory>

namespace {
Q_LOGGING_CATEGORY(self, "cpugraph")
}

CPUGraph::CPUGraph() {}

CPUGraph::~CPUGraph()
{
    if (m_cpu)
        m_cpu->unsubscribe();
}

CPUMonitor *CPUGraph::cpu() const
{
    return m_cpu;
}

void CPUGraph::setCpu(CPUMonitor *newCpu)
{
    if (m_cpu == newCpu)
        return;

    if (m_cpu) {
        disconnect(m_cpu, &CPUMonitor::usagesChanged, this, &CPUGraph::onUsagesChanged);
        m_cpu->unsubscribe();
    }

    qCInfo(self) << "change cpu" << m_cpu;
    m_cpu = newCpu;
    emit cpuChanged();

    if (m_cpu) {
        connect(m_cpu, &CPUMonitor::usagesChanged, this, &CPUGraph::onUsagesChanged);
        m_cpu->subscribe();
    }
}

void CPUGraph::onUsagesChanged()
{
    this->pushValues2(m_cpu->usages().mid(1), QDateTime::currentMSecsSinceEpoch());
}
