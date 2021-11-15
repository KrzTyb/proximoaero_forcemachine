import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: measurePopup
    visible: true
    width: parent.width
    height: parent.height
    anchors.centerIn: parent
    modal: true
    focus: true
    dim: true
    padding: 200
    closePolicy: Popup.NoAutoClose

    property bool indicatorVisible: false
    property string infoText: ""

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
        ColumnLayout
        {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.centerIn: parent
            spacing: 10
            Text {
                id: calibPopupText
                text: measurePopup.infoText
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.weight: Font.Bold
                font.pointSize: 20
                color: "white"
            }
            BusyIndicator
            {
                running: measurePopup.indicatorVisible
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }
}