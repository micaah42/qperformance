#ifndef CPUGRAPH_H
#define CPUGRAPH_H

#include <QImage>
#include <QQuickPaintedItem>

#include "cpumonitor.h"
#include "performancechart.h"

class CPUGraph : public PerformanceChart
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(CPUMonitor *cpu READ cpu WRITE setCpu NOTIFY cpuChanged FINAL)

public:
    CPUGraph();
    ~CPUGraph();

    CPUMonitor *cpu() const;
    void setCpu(CPUMonitor *newCpu);

signals:
    void cpuChanged();

protected:
    void onUsagesChanged();

private:
    CPUMonitor *m_cpu = nullptr;
};

#endif // CPUGRAPH_H
