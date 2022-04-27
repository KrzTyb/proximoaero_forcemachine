import QtQuick
import QtQuick.Controls

Popup {
    id: exportPopup
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

    property string popupText: qsTr("Błąd zapisu pomiarów") //qsTr("Zapisano pomiary")


    contentItem: Rectangle
    {
        color: "#414141"
        border.color: "black"
        border.width: 4
        radius: 8
        Row {
            anchors.centerIn: parent
            spacing: 8
            Text {
                id: exportPopupText
                y: 10
                text: popupText
                font.weight: Font.Bold
                font.pointSize: 20
                color: "white"
            }
            Button {
                id: confirmButton
                width: 153
                height: 54
                font.weight: Font.Bold
                background: Rectangle {
                    opacity: enabled ? 1 : 0.3
                    color: confirmButton.down ? "#DCDCDC" : "#2F4F4F"
                    radius: 8
                    border.color: confirmButton.down ? "#DCDCDC" : "black"
                    border.width: 2
                }
                font.pointSize: 15
                contentItem: Text {
                    opacity: enabled ? 1.0 : 0.3
                    color: confirmButton.down ? "black" : "white"
                    text: qsTr("Potwierdź")
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: confirmButton.font
                }

                onClicked: exportPopup.visible = false
            }
        }
    }
}