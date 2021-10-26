import QtQuick
import QtQuick.Controls


Item {
    id: root

    Component.onCompleted:
    {
        scaleInput.forceActiveFocus()
    }

    Rectangle {
        id: background
        color: "#414141"
        anchors.fill: parent

        NumericKeyboard {
            id: numericKeyboard
            x: 0
            y: 330

            onKeyClicked: function onKeyClicked(key)
            {
                if (scaleInput.text.includes(".") && key === ".")
                {
                    return
                }

                scaleInput.insert(scaleInput.cursorPosition, key)
            }
            onDeleteClicked: function onDeleteClicked()
            {
                scaleInput.remove(scaleInput.cursorPosition - 1, scaleInput.cursorPosition)
                if (scaleInput.text.length === 0)
                {
                    return
                }

                if (scaleInput.text.indexOf(".") === scaleInput.text.length-1)
                {
                    scaleInput.insert(scaleInput.text.length, "0")
                    scaleInput.cursorPosition -= 1
                }
                if (scaleInput.text.indexOf(".") === 0)
                {
                    scaleInput.insert(0, "0")
                    scaleInput.cursorPosition -= 1
                }
            }
        }

        Text {
            id: scaleInfo
            x: 136
            y: 199
            width: 153
            height: 49
            color: "#ffffff"
            text: qsTr("Waga [kg]")
            font.pixelSize: 19
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.weight: Font.Bold
        }

        Image {
            id: scaleInputBackground
            x: 128
            y: 254
            source: "images/ScaleInputBackground.png"
            fillMode: Image.PreserveAspectFit

            TextInput {
                id: scaleInput
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

        Image {
            id: start_Button
            x: 153
            y: 142
            source: "images/Start_Button.png"
            fillMode: Image.PreserveAspectFit

            Text {
                id: start_text
                x: 40
                y: 8
                color: "#FFFFFF"
                text: qsTr("Start")
                font.pixelSize: 17
                font.weight: Font.Bold
            }

            function setPressed()
            {
                start_Button.source = "images/Start_Button_Pressed.png"
                start_text.color = "#000000"
                start_Button.x = 161
                start_Button.y = 146
                start_text.x = 32
            }

            function setReleased()
            {
                start_Button.source = "images/Start_Button.png"
                start_text.color = "#FFFFFF"
                start_Button.x = 153
                start_text.x = 40
                start_Button.y = 142
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    BackendConnector.setScale(scaleInput.text)
                    BackendConnector.clickStart()
                }
            }
        }

        Image {
                    id: logo
                    x: 136
                    y: 8
                    width: 160
                    height: 128
                    source: "images/ProximoAeroLogoWithShadow.png"
                    fillMode: Image.PreserveAspectFit

        }



        ContentPreview {
            id: contentPreview
            x: 406
            y: 0
        }
    }


    Connections {
        target: BackendConnector
        function onBlockStartClicked(blocked)
        {
            if (blocked)
            {
                start_Button.setPressed()
                start_Button.enabled = false
                console.log("Blocked")
            }
            else
            {
                start_Button.enabled = true
                start_Button.setReleased()
                console.log("Unblocked")
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:1.25;height:600;width:1024}
}
##^##*/

