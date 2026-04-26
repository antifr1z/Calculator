import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: theme.surface
    radius: theme.radiusLarge
    clip: true

    TreeView {
        id: treeView
        anchors.fill: parent
        anchors.margins: theme.spacingNormal
        model: typeof treeModel !== "undefined" ? treeModel : null
        selectionModel: ItemSelectionModel {}

        delegate: FieldDelegate {}
    }
}
