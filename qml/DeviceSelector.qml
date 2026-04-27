import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: theme.surface
    radius: theme.radiusLarge
    implicitHeight: 50

    readonly property var _deviceNames: typeof deviceManager !== "undefined" ? deviceManager.deviceNames : ["BCM", "Instrument Cluster", "Comfort Module"]

    RowLayout {
        anchors.fill: parent
        anchors.margins: theme.marginNormal
        spacing: theme.spacingMedium

        Text {
            text: "Device:"
            color: theme.textPrimary
            font.pixelSize: theme.fontMedium
            font.bold: true
        }

        ComboBox {
            id: deviceCombo
            Layout.fillWidth: true
            model: _deviceNames

            contentItem: Text {
                text: deviceCombo.displayText
                color: theme.textPrimary
                font.pixelSize: theme.fontNormal
                verticalAlignment: Text.AlignVCenter
                leftPadding: 8
            }

            background: Rectangle {
                color: theme.overlay
                radius: theme.radiusMedium
                border.color: deviceCombo.activeFocus ? theme.accent : theme.border
                border.width: 1
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0 && typeof deviceManager !== "undefined") {
                    var path = deviceManager.getConfigPath(currentIndex)
                    codingEngine.loadConfig(path)
                    treeModel.refresh()
                }
            }
        }
    }
}
