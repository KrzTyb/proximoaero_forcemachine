import QtQuick
import QtQuick.Controls

Popup {
    id: doorPopup
    visible: true
    width: parent.width
    height: parent.height
    anchors.centerIn: parent
    modal: true
    focus: true
    dim: true
    padding: 200
    closePolicy: Popup.NoAutoClose
    background: Rectangle
    {
        width: parent.width
        height: parent.height
        color: "black"
        opacity: 0.8
    }

    contentItem: Rectangle
    {
        color: "#414141"
        border.color: "black"
        border.width: 4
        radius: 8
        Text {
            id: doorPopupText
            text: qsTr("Zamknij drzwi!")
            font.weight: Font.Bold
            font.pointSize: 20
            anchors.centerIn: parent
            color: "red"

            Timer {
                running: doorPopup.visible ? true : false
                repeat: true
                interval: 1000
                onTriggered: doorPopupText.visible = !doorPopupText.visible
            }
        }
    }
}