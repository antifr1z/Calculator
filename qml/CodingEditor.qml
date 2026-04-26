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
        
        // Ensure expansion is enabled
        focus: true
        
        // Make items larger for easier interaction
        rowHeightProvider: function(row) {
            return 56
        }
        
        // Make sure items can be expanded
        Component.onCompleted: {
            // Expand the first item if it has children
            if (model && model.rowCount() > 0) {
                var firstIndex = index(0, 0)
                if (model.hasChildren(firstIndex)) {
                    expand(firstIndex)
                }
            }
        }
    }
}
