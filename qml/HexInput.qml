import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    color: theme.surface
    radius: theme.radiusLarge
    implicitHeight: 50

    // Designer-safe fallbacks
    readonly property string _hexStr: typeof codingEngine !== "undefined" ? codingEngine.hexString : "BA34"
    readonly property int _maxLen: typeof codingEngine !== "undefined" ? codingEngine.codingBytes * 2 : 8

    RowLayout {
        anchors.fill: parent
        anchors.margins: theme.marginNormal
        spacing: theme.spacingMedium

        Text {
            text: "Coding:"
            color: theme.textPrimary
            font.pixelSize: theme.fontMedium
            font.bold: true
        }

        TextField {
            id: hexField
            Layout.fillWidth: true
            placeholderText: "Enter hex string (e.g. BA34)"
            text: _hexStr
            font.family: theme.monoFont
            font.pixelSize: theme.fontMedium
            color: theme.textPrimary
            maximumLength: _maxLen

            background: Rectangle {
                color: theme.overlay
                radius: theme.radiusMedium
                border.color: hexField.activeFocus ? theme.accent : theme.border
                border.width: 1
            }

            validator: RegularExpressionValidator {
                regularExpression: /[0-9A-Fa-f]*/
            }

            onTextEdited: {
                if (typeof codingEngine !== "undefined")
                    codingEngine.hexString = text
            }

            Connections {
                target: typeof codingEngine !== "undefined" ? codingEngine : null
                function onConfigurationChanged() {
                    if (!hexField.activeFocus)
                        hexField.text = codingEngine.hexString
                }
            }
        }

        Button {
            text: "Load"
            onClicked: loadDialog.open()

            contentItem: Text {
                text: parent.text
                color: theme.background
                font.pixelSize: theme.fontSmall
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: parent.hovered ? theme.accentHover : theme.accent
                radius: theme.radiusMedium
            }
        }

        Button {
            text: "Save"
            onClicked: saveDialog.open()

            contentItem: Text {
                text: parent.text
                color: theme.background
                font.pixelSize: theme.fontSmall
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: parent.hovered ? theme.accentHover : theme.success
                radius: theme.radiusMedium
            }
        }
    }

    FileDialog {
        id: loadDialog
        title: "Load Coding"
        nameFilters: ["JSON files (*.json)", "Binary files (*.bin)", "All files (*)"]
        onAccepted: {
            var path = selectedFile.toString().replace("file:///", "")
            if (typeof fileIO !== "undefined") {
                if (path.endsWith(".bin")) {
                    fileIO.loadBinaryFromFile(path, codingEngine)
                } else {
                    fileIO.loadHexFromFile(path, codingEngine)
                }
            }
        }
    }

    FileDialog {
        id: saveDialog
        title: "Save Coding"
        fileMode: FileDialog.SaveFile
        nameFilters: ["JSON files (*.json)", "Binary files (*.bin)"]
        onAccepted: {
            var path = selectedFile.toString().replace("file:///", "")
            if (typeof fileIO !== "undefined") {
                if (path.endsWith(".bin")) {
                    fileIO.saveBinaryToFile(path, codingEngine)
                } else {
                    fileIO.saveHexToFile(path, codingEngine)
                }
            }
        }
    }
}
