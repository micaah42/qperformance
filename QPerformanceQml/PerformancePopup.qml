import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    id: root

    property color backgroundColor: Qt.rgba(0.24, 0.24, 0.24, 0.84)
    property alias dialog: dialog
    signal closeRequested
    signal dragStopped
    height: dialog.height
    width: 280

    Popup {
        id: dialog

        Material.roundedScale: Material.SmallScale
        Material.background: root.backgroundColor
        closePolicy: Popup.NoAutoClose
        width: root.width
        parent: root
        dim: false
        padding: 8
        z: 10

        contentItem: Item {
            implicitHeight: column.implicitHeight
            MouseArea {
                id: mover
                anchors.fill: parent
                drag.target: root
                onReleased: root.dragStopped()
            }
            ColumnLayout {
                id: column
                width: parent.width

                RowLayout {
                    Label {
                        id: label
                        Layout.fillWidth: true
                        text: 'Performance'
                        font.pixelSize: Style.fontPxLarge
                        font.family: Style.fontFamily
                    }
                    Button {
                        Layout.preferredHeight: label.height
                        font.pixelSize: Style.fontPxSmall
                        font.family: Style.fontFamily
                        bottomPadding: 0
                        bottomInset: 0
                        topPadding: 0
                        topInset: 0

                        onClicked: {
                            root.closeRequested()
                            dialog.close()
                        }
                        text: 'Close'
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    Layout.preferredHeight: item?.implicitHeight || 0
                    active: root.visible

                    sourceComponent: PerformanceDisplay {
                        id: performanceDisplay
                        chartHeight: 96
                        spacing: 16
                    }
                }
            }
        }
    }
}
