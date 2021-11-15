import QtQuick
import QtQuick.Controls


Item {
    id: configPopup
    anchors.fill: parent

    property int step: 0

    signal next2Clicked(height: int)

    MouseArea
    {
        anchors.fill: parent
    }

    Rectangle
    {
        anchors.fill: parent
        color: "black"
        opacity: 0.8
    }

    Item
    {
        width: parent.width / 1.5
        height: parent.height / 1.5
        anchors.centerIn: parent

        SwipeView
        {
            id: configPopupContent
            currentIndex: configPopup.step
            anchors.fill: parent
            clip: true
            interactive: false
            Rectangle
            {
                color: "#414141"
                border.color: "black"
                border.width: 4

                radius: 8

                Text {
                    id: text1
                    x: 178
                    y: 88
                    color: "#ffffff"
                    text: qsTr("Umieść próbkę, obciążenie\noraz\ngrot na urządzeniu")
                    font.pixelSize: 24
                    horizontalAlignment: Text.AlignHCenter
                    font.weight: Font.Bold
                }

                Button {
                    id: next1
                    x: 265
                    y: 236
                    width: 153
                    height: 54
                    font.weight: Font.Bold
                    background: Rectangle {
                        opacity: enabled ? 1 : 0.3
                        color: next1.down ? "#DCDCDC" : "#2F4F4F"
                        radius: 8
                        border.color: next1.down ? "#DCDCDC" : "black"
                        border.width: 2
                    }
                    font.pointSize: 15
                    contentItem: Text {
                        opacity: enabled ? 1.0 : 0.3
                        color: next1.down ? "black" : "white"
                        text: qsTr("Dalej")
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: next1.font
                    }

                    onClicked: configPopupContent.incrementCurrentIndex()
                }
            }
            Rectangle
            {
                color: "#414141"
                border.color: "black"
                border.width: 4

                radius: 8

                function heightOnKeyClicked(key)
                {
                    if (key === ".")
                    {
                        return
                    }

                    heightInput.insert(heightInput.cursorPosition, key)
                }

                function heightOnDeleteClicked()
                {
                    heightInput.remove(heightInput.cursorPosition - 1, heightInput.cursorPosition)
                    if (heightInput.text.length === 0)
                    {
                        return
                    }
                }

                NumericKeyboard {
                    id: configKeyboard
                    x: 137
                    y: 130
                    width: 409
                    height: 262
                    backgroudEnabled: false

                    onKeyClicked: function onKeyClicked(key)
                    {
                        if (heightInput.focus)
                        {
                            parent.heightOnKeyClicked(key)
                        }
                    }
                    onDeleteClicked: function onDeleteClicked()
                    {
                        if (heightInput.focus)
                        {
                            parent.heightOnDeleteClicked()
                        }
                    }
                }

                Text {
                    id: text2
                    x: 244
                    y: 24
                    color: "#ffffff"
                    text: qsTr("Podaj wysokość (10-100) [cm]")
                    font.pixelSize: 20
                    font.weight: Font.Bold
                }

                Image {
                    id: heightInputBackground
                    x: 180
                    y: 70
                    source: "images/ScaleInputBackground.png"
                    fillMode: Image.PreserveAspectFit

                    TextInput {
                        id: heightInput
                        x: 8
                        y: 0
                        width: 153
                        height: 49
                        text: ""
                        font.pixelSize: 19
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        maximumLength: 12
                        font.weight: Font.Bold
                    }
                }

                function getHeight()
                {
                    var heightStr = heightInput.text
                    var heightInt = parseInt(heightStr, 10)
                    if (heightInt)
                    {
                        if (heightInt > 100)
                        {
                            return 100
                        }
                        else if (heightInt < 10)
                        {
                            return 10
                        }
                        return heightInt
                    }
                    else
                    {
                        return 10
                    }
                }

                Button {
                    id: next2
                    x: 393
                    y: 70
                    width: 153
                    height: 54
                    text: qsTr("Rozpocznij")
                    font.weight: Font.Bold
                    background: Rectangle {
                        opacity: enabled ? 1 : 0.3
                        color: next2.down ? "#DCDCDC" : "#2F4F4F"
                        radius: 8
                        border.color: next2.down ? "#DCDCDC" : "black"
                        border.width: 2
                    }
                    font.pointSize: 15
                    contentItem: Text {
                        opacity: enabled ? 1.0 : 0.3
                        color: next2.down ? "black" : "white"
                        text: next2.text
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: next2.font
                    }

                    onClicked: 
                    {
                        next2Clicked(parent.getHeight())
                        heightInput.text = ""
                    }
                }
            }

        }
    }
}