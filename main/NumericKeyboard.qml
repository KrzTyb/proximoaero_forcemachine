import QtQuick
import QtQuick.Controls

Item {
    id: keyboard_root
    width: 417
    height: 270

    property bool backgroudEnabled: true

    Image {
        id: background
        source: "images/KeyboardTile.png"
        anchors.fill: parent
        visible: keyboard_root.backgroudEnabled
    }
    signal keyClicked(key: string)
    signal deleteClicked()

    Key {
        id: key1
        x: 14
        y: 23
        key_text: "1"
        onClicked: parent.keyClicked("1")
    }

    Key {
        id: key2
        x: 108
        y: 23
        key_text: "2"
        onClicked: parent.keyClicked("2")
    }

    Key {
        id: key3
        x: 208
        y: 23
        key_text: "3"
        onClicked: parent.keyClicked("3")
    }

    Key {
        id: key4
        x: 13
        y: 104
        key_text: "4"
        onClicked: parent.keyClicked("4")
    }

    Key {
        id: key5
        x: 108
        y: 104
        key_text: "5"
        onClicked: parent.keyClicked("5")
    }

    Key {
        id: key6
        x: 208
        y: 104
        key_text: "6"
        onClicked: parent.keyClicked("6")
    }

    Key {
        id: key7
        x: 13
        y: 187
        key_text: "7"
        onClicked: parent.keyClicked("7")
    }

    Key {
        id: key8
        x: 108
        y: 187
        key_text: "8"
        onClicked: parent.keyClicked("8")
    }

    Key {
        id: key9
        x: 208
        y: 187
        key_text: "9"
        onClicked: parent.keyClicked("9")
    }

    Key {
        id: key0
        x: 308
        y: 187
        key_text: "0"
        onClicked: parent.keyClicked("0")
    }
    Key {
        id: keyComa
        x: 308
        y: 104
        key_text: "."
        onClicked: parent.keyClicked(".")
    }
    KeyDel {
        x: 308
        y: 23
        onClicked: parent.deleteClicked()
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.5}
}
##^##*/

