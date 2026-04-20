import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material

Rectangle {
    id: root
    Layout.fillWidth: true
    property string leftLabelText: duration > 0 ? Style.roundWithPrecision(duration / 1000, 1) + "s" : ''
    property string topLabelText: ''
    property int verticalMarkers: 5
    property int duration: 0

    border.color: Material.frameColor
    color: 'transparent'

    Repeater {
        model: verticalMarkers

        delegate: Rectangle {
            y: modelData * root.height / (verticalMarkers - 1)
            width: root.width
            opacity: 0.42
            height: 0.75
        }
    }

    Label {
        font.pixelSize: Style.fontPxSmall
        font.family: Style.fontFamily
        text: topLabelText
        x: 4
        y: 2
        z: 1
    }
    Label {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        font.pixelSize: Style.fontPxSmall
        font.family: Style.fontFamily
        text: leftLabelText
        x: 4
        z: 1
    }
}
