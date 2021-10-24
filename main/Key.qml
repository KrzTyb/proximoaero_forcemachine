import QtQuick
import QtQuick.Controls

Item {
    id: key_root
    property string unpressed_source: "images/Key.png"
    property string pressed_source: "images/Key_Pressed.png"
    property string key_text: "0"
    width: 94
    height: 77
    state: "Unpressed"
    signal clicked()

    Image {
        id: image
        anchors.fill: parent
        source: parent.unpressed_source
    }

    Text {
        id: k_text
        color: "#ffffff"
        anchors.centerIn: parent
        text: key_text
        font.weight: Font.Bold
        font.family: "Tahoma"
        anchors.verticalCenterOffset: -3
        anchors.horizontalCenterOffset: 1
        font.pointSize: 20
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed:
        {
            parent.state = "Pressed"
            parent.clicked()
        }
        onReleased: parent.state = "Unpressed"
    }

    states: [
        State {
            name: "Unpressed"
            PropertyChanges {
                target: image
                source: unpressed_source
                scale: 1.0
            }
            PropertyChanges {
                target: k_text
                anchors.verticalCenterOffset: -3
                color: "#ffffff"
            }

        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: image
                source: pressed_source
                scale: 0.92
            }
            PropertyChanges {
                target: k_text
                anchors.verticalCenterOffset: -1
                color: "#000000"
            }
        }
    ]
}
