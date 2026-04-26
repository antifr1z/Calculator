import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: theme.surface
    radius: theme.radiusSmall
    implicitHeight: 56
    implicitWidth: 280

    readonly property string fieldName: model.fieldName || ""
    readonly property string fieldType: model.fieldType || ""
    readonly property int bitOffset: model.bitOffset || 0
    readonly property int bitWidth: model.bitWidth || 0
    readonly property int absoluteBitOffset: model.absoluteBitOffset || 0
    readonly property var fieldValue: model.fieldValue || 0
    readonly property var fieldOptions: model.fieldOptions || []
    readonly property bool editable: model.editable || false

    RowLayout {
        anchors.fill: parent
        anchors.margins: theme.spacingNormal
        spacing: theme.spacingMedium

        // Expansion indicator area for nested fields
        Rectangle {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
            Layout.alignment: Qt.AlignVCenter
            color: "transparent"
            
            Text {
                anchors.centerIn: parent
                text: fieldType === appConfig.nestedType ? "▶" : ""
                color: theme.textSecondary
                font.pixelSize: theme.fontSmall
                font.bold: true
            }
        }

        // Field name and bit info column
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Layout.alignment: Qt.AlignVCenter

            Text {
                text: fieldName
                color: fieldType === appConfig.defaultType ? theme.textMuted : theme.textPrimary
                font.pixelSize: theme.fontNormal
                font.bold: fieldType === appConfig.nestedType
                Layout.fillWidth: true
            }

            Text {
                text: "bit " + absoluteBitOffset + ":" + bitWidth
                color: theme.textMuted
                font.pixelSize: theme.fontSmall
                font.family: theme.monoFont
                Layout.leftMargin: 8
                visible: bitWidth > 0
            }
        }

        // Value editor
        Loader {
            Layout.preferredWidth: 180
            Layout.preferredHeight: 32
            Layout.alignment: Qt.AlignVCenter
            sourceComponent: {
                if (fieldType === "bool") return boolEditor
                if (fieldType === "enum") return enumEditor
                if (fieldType === appConfig.defaultType) return reservedLabel
                if (fieldType === appConfig.nestedType) return nestedEditor
                return reservedLabel
            }
        }
    }

    Component {
        id: boolEditor
        Switch {
            checked: fieldValue !== 0
            onToggled: {
                if (typeof treeModel !== "undefined")
                    treeModel.setFieldValue(absoluteBitOffset,
                                            bitWidth,
                                            checked ? 1 : 0)
            }
        }
    }

    Component {
        id: enumEditor
        ComboBox {
            model: fieldOptions
            textRole: "label"
            valueRole: "value"
            currentIndex: indexOfValue(fieldValue)
            
            onActivated: function(currentIndex) {
                if (typeof treeModel !== "undefined")
                    treeModel.setFieldValue(absoluteBitOffset,
                                            bitWidth,
                                            valueAt(currentIndex))
            }
        }
    }

    Component {
        id: nestedEditor
        Text {
            text: "[nested]"
            color: theme.textSecondary
            font.pixelSize: theme.fontNormal
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Component {
        id: reservedLabel
        Text {
            text: "0x" + fieldValue.toString(16).toUpperCase().padStart(2, '0')
            color: theme.textMuted
            font.pixelSize: theme.fontNormal
            font.family: theme.monoFont
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
