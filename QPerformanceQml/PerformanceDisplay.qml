import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import QPerformanceQml

ColumnLayout {
    id: root
    spacing: 32

    property var colors: ["#7cb5ec", "#434348", "#90ed7d", "#f7a35c", "#8085e9", "#f15c80", "#e4d354", "#2b908f", "#f45b5b", "#91e8e1"]
    property bool showHeaders: true
    property real chartHeight: 120

    Panel {
        Layout.fillWidth: true

        title: 'FPS Monitor'
        header.visible: showHeaders
        header.data: [
            Label {
                Layout.preferredWidth: sizer.implicitWidth
                font.pixelSize: Style.fontPxSmall
                font.family: Style.fontFamily
                text: `FPS: ${fpsDebouncer.output.toFixed(0)}`
                opacity: 0.72

                Debouncer {
                    id: fpsDebouncer
                    input: FPSMonitor.fps
                }
            },
            Label {
                Layout.preferredWidth: sizer.implicitWidth
                font.pixelSize: Style.fontPxSmall
                font.family: Style.fontFamily
                text: `Avg. FPS: ${avgFpsDebouncer.output.toFixed(0)}`
                opacity: 0.72

                Debouncer {
                    id: avgFpsDebouncer
                    input: FPSMonitor.avgFps
                }
            },
            Label {
                id: sizer
                Layout.preferredWidth: 120
                font.pixelSize: Style.fontPxSmall
                font.family: Style.fontFamily
                text: `Avg. FPS: 999`
                visible: false
            }
        ]

        ChartBackground {
            Layout.preferredHeight: chartHeight
            duration: fpsGraph.duration
            topLabelText: '120'

            PerformanceChart {
                id: fpsGraph
                anchors.fill: parent
                monitor: FPSMonitor
                colors: root.colors
                max: 120
            }
        }
    }

    Panel {
        header.visible: showHeaders
        Layout.fillWidth: true
        title: 'CPU Usage'
        subtitle: infos.filter(x => x.length).join(' \u2022 ')
        readonly property var infos: [CPUMonitor.model, CPUMonitor.vendor, CPUMonitor.architecture]

        ChartBackground {
            Layout.preferredHeight: chartHeight
            duration: cpuGraph.duration
            topLabelText: '100%'

            PerformanceChart {
                id: cpuGraph
                anchors.fill: parent
                monitor: CPUMonitor
                colors: root.colors
            }
        }
    }
    Panel {
        header.visible: showHeaders
        Layout.fillWidth: true

        title: 'Memory Usage'
        subtitle: [//@
            'Mem: ' + Style.formatKBFraction(MemoryMonitor.memUsed, MemoryMonitor.memTotal), //@
            'Swap: ' + Style.formatKBFraction(MemoryMonitor.swapUsed, MemoryMonitor.swapTotal) //@
        ].join(' \u2022 ')

        ChartBackground {
            Layout.preferredHeight: chartHeight
            duration: memoryGraph.duration
            topLabelText: '100%'

            PerformanceChart {
                id: memoryGraph
                anchors.fill: parent
                monitor: MemoryMonitor
                colors: root.colors
                max: 1
            }
        }
    }
}
