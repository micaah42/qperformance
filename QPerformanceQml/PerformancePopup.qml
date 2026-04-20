import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    id: root

    property color backgroundColor: Qt.rgba(0.24, 0.24, 0.24, 0.84)
    property alias dialog: dialog
    height: dialog.height
    width: 280

    Popup {
        id: dialog

        Material.roundedScale: Material.SmallScale
        Material.background: root.backgroundColor
        closePolicy: Popup.NoAutoClose
        width: root.width
        parent: root
        visible: true
        dim: false
        padding: 8
        z: 10

        contentItem: Item {
            implicitHeight: column.implicitHeight

            ColumnLayout {
                id: column
                width: parent.width

                Label {
                    Layout.fillWidth: true
                    text: 'Performance (FPS, CPU, Memory)'
                    font.pixelSize: Style.fontPxLarge
                    font.family: Style.fontFamily
                }

                Loader {
                    Layout.fillWidth: true
                    Layout.preferredHeight: item?.implicitHeight || 0
                    active: root.visible

                    sourceComponent: PerformanceDisplay {
                        id: performanceDisplay
                        showHeaders: false
                        chartHeight: 96
                        spacing: 16
                    }
                }
            }

            MouseArea {
                id: mover
                anchors.fill: parent
                drag.target: root
            }
        }
    }
}
