import QtQuick
import QtQuick.Controls

Item {
    id: key_root
    property string unpressed_source: "images/KeyDel.png"
    property string pressed_source: "images/KeyDel_Pressed.png"
    width: 94
    height: 77
    state: "Unpressed"
    signal clicked()

    Image {
        id: image
        anchors.fill: parent
        source: parent.unpressed_source
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
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: image
                source: pressed_source
                scale: 0.92
            }
        }
    ]
}
