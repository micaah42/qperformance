import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    property alias header: header
    property string subtitle
    property string title
    spacing: 8

    RowLayout {
        id: header
        Layout.fillWidth: true
        Layout.bottomMargin: 4

        Label {
            text: title
            font.pixelSize: Style.fontPxLarge
            font.family: Style.fontFamily
        }
        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignRight
            font.pixelSize: Style.fontPxSmall
            font.family: Style.fontFamily
            opacity: 0.72
            text: subtitle
        }
    }
}
