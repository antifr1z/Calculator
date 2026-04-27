import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    id: root
    width: 800
    height: 650
    visible: true
    title: "Coding Calculator"
    color: theme.background

    readonly property string _binaryStr: typeof codingEngine !== "undefined" ? codingEngine.binaryString : "1011 1010 0011 0100"
    readonly property string _hexStr: typeof codingEngine !== "undefined" ? codingEngine.hexString : "BA34"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme.marginLarge
        spacing: theme.spacingLarge

        DeviceSelector {
            Layout.fillWidth: true
        }

        HexInput {
            Layout.fillWidth: true
        }

        CodingEditor {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // Binary & Hex live preview
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: theme.surface
            radius: theme.radiusLarge

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: theme.marginNormal
                spacing: theme.spacingSmall

                Text {
                    text: "Binary: " + root._binaryStr
                    color: theme.textSecondary
                    font.family: theme.monoFont
                    font.pixelSize: theme.fontNormal
                }
                Text {
                    text: "Hex: " + root._hexStr
                    color: theme.textPrimary
                    font.family: theme.monoFont
                    font.pixelSize: theme.fontMedium
                    font.bold: true
                }
            }
        }
    }
}
