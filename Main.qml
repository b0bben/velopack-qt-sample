import QtQuick
import QtQuick.Window
import QtQuick.Controls

import VelopackQt 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Velopack Qt c++ example")

    property string currentVersion: AutoUpdater.currentVersion || "1.0.0"

    Column {
        anchors.fill: parent
        anchors.topMargin: 40
        spacing: 10

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            text: "Welcome to v%1 of the Velopack Qt C++ Sample App.".arg(
                      currentVersion)
        }

        Button {
            width: 400
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Check for updates"
        }

        Button {
            width: 400
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Download update"
        }

        Button {
            width: 400
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply update and restart"
        }
    }
}
