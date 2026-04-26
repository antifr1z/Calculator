import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: fieldRoot
    implicitWidth: ListView.view ? ListView.view.width : 400
    implicitHeight: contentRow.implicitHeight + 8

    // Required TreeView delegate properties
    required property int depth
    required property bool isTreeNode
    required property bool expanded
    required property bool hasChildren
    required property int row

    // Data model properties
    required property string fieldName
    required property string fieldType
    required property int bitOffset
    required property int bitWidth
    required property int absoluteBitOffset
    required property int fieldValue
    required property string displayValue
    required property var fieldOptions
    required property bool editable

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        color: fieldRoot.row % 2 === 0 ? theme.surface : theme.surfaceAlt
        radius: theme.radiusSmall

        RowLayout {
            id: contentRow
            anchors.fill: parent
            anchors.leftMargin: 12 + fieldRoot.depth * 24
            anchors.rightMargin: 12
            spacing: theme.spacingNormal

            // Expand indicator
            Text {
                text: fieldRoot.hasChildren ? (fieldRoot.expanded ? "\u25BC" : "\u25B6") : "  "
                color: theme.textMuted
                font.pixelSize: 10
                Layout.preferredWidth: 16

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (typeof treeView !== "undefined")
                            treeView.toggleExpanded(fieldRoot.row)
                    }
                }
            }

            // Field name and bit info column
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: fieldRoot.fieldName
                    color: fieldRoot.fieldType === "reserved" ? theme.textMuted : theme.textPrimary
                    font.pixelSize: theme.fontNormal
                    font.bold: fieldRoot.hasChildren
                    Layout.fillWidth: true
                }

                Text {
                    text: "bit " + fieldRoot.absoluteBitOffset + ":" + fieldRoot.bitWidth
                    color: theme.textMuted
                    font.pixelSize: theme.fontSmall
                    font.family: theme.monoFont
                    Layout.leftMargin: 8
                }
            }

            // Value editor
            Loader {
                Layout.preferredWidth: 180
                Layout.preferredHeight: 28
                sourceComponent: {
                    if (fieldRoot.fieldType === "bool") return boolEditor
                    if (fieldRoot.fieldType === "enum") return enumEditor
                    if (fieldRoot.fieldType === "reserved") return reservedLabel
                    if (fieldRoot.fieldType === "nested") return null
                    return reservedLabel
                }
            }
        }
    }

    Component {
        id: boolEditor
        Switch {
            checked: fieldRoot.fieldValue !== 0
            onToggled: {
                if (typeof treeModel !== "undefined")
                    treeModel.setFieldValue(fieldRoot.absoluteBitOffset,
                                            fieldRoot.bitWidth,
                                            checked ? 1 : 0)
            }
        }
    }

    Component {
        id: enumEditor
        ComboBox {
            id: combo
            model: fieldRoot.fieldOptions
            textRole: "label"
            valueRole: "value"
            currentIndex: {
                for (var i = 0; i < count; i++) {
                    if (model[i].value === fieldRoot.fieldValue)
                        return i
                }
                return -1
            }

            contentItem: Text {
                text: combo.displayText
                color: theme.textPrimary
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
                leftPadding: 6
            }

            background: Rectangle {
                color: theme.overlay
                radius: theme.radiusSmall
                border.color: combo.activeFocus ? theme.accent : theme.border
            }

            onActivated: {
                if (typeof treeModel !== "undefined")
                    treeModel.setFieldValue(fieldRoot.absoluteBitOffset,
                                            fieldRoot.bitWidth,
                                            currentValue)
            }
        }
    }

    Component {
        id: reservedLabel
        Text {
            text: "0x" + fieldRoot.fieldValue.toString(16).toUpperCase()
            color: theme.textMuted
            font.family: theme.monoFont
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
        }
    }
}
