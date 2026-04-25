import QtQuick

QtObject {
    id: theme

    // Base colors (Catppuccin Mocha)
    readonly property color background: "#1e1e2e"
    readonly property color surface: "#313244"
    readonly property color surfaceAlt: "#2a2a3c"
    readonly property color overlay: "#45475a"
    readonly property color border: "#585b70"

    // Text colors
    readonly property color textPrimary: "#cdd6f4"
    readonly property color textSecondary: "#a6adc8"
    readonly property color textMuted: "#6c7086"

    // Accent colors
    readonly property color accent: "#89b4fa"
    readonly property color accentHover: "#94e2d5"
    readonly property color success: "#a6e3a1"

    // Typography
    readonly property string monoFont: "Consolas"
    readonly property int fontSmall: 11
    readonly property int fontNormal: 13
    readonly property int fontMedium: 14

    // Spacing
    readonly property int radiusSmall: 4
    readonly property int radiusMedium: 6
    readonly property int radiusLarge: 8
    readonly property int marginNormal: 10
    readonly property int marginLarge: 16
    readonly property int spacingSmall: 4
    readonly property int spacingNormal: 8
    readonly property int spacingMedium: 10
    readonly property int spacingLarge: 12
}
