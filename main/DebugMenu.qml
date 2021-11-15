import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    width: 1024
    height: 600
    visible: true
    title: qsTr("Force Machine Debug Menu")

    Switch {
        id: lowerLimit
        x: 64
        y: 88
        width: 57
        height: 53
        text: qsTr("Switch")
        scale: 1.6

        onToggled:
        {
            DebugInput.debug_lowerLimitState(lowerLimit.position)
        }
    }

    Switch {
        id: upperLimit
        x: 189
        y: 88
        width: 57
        height: 53
        text: qsTr("Switch")
        scale: 1.6

        onToggled:
        {
            DebugInput.debug_upperLimitState(upperLimit.position)
        }
    }

    Switch {
        id: door
        x: 303
        y: 88
        width: 57
        height: 53
        text: qsTr("Switch")
        scale: 1.6

        onToggled:
        {
            DebugInput.debug_doorState(door.position)
        }
    }

    Button {
        id: button
        x: 414
        y: 90
        width: 134
        height: 49
        text: qsTr("START")

        onPressed: {
            DebugInput.debug_startButtonState(true)
        }
        onReleased: {
            DebugInput.debug_startButtonState(false)
        }
    }

    Text {
        id: text1
        x: 58
        y: 39
        width: 70
        height: 37
        text: qsTr("Lower")
        font.pixelSize: 20
    }

    Text {
        id: text2
        x: 183
        y: 39
        width: 70
        height: 37
        text: qsTr("Upper")
        font.pixelSize: 20
    }

    Text {
        id: text3
        x: 297
        y: 39
        width: 70
        height: 37
        text: qsTr("Door")
        font.pixelSize: 20
    }

    Text {
        id: text4
        x: 395
        y: 39
        width: 173
        height: 37
        text: qsTr("Mechanical Start")
        font.pixelSize: 20
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.9;height:600;width:1024}
}
##^##*/
