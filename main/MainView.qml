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

        Image {
            id: logo
            x: 51
            y: 54
            width: 315
            height: 235
            source: "images/ProximoAeroLogo.png"
        }

        function scaleOnKeyClicked(key)
        {
            if (scaleInput.text.includes(".") && key === ".")
            {
                return
            }

            scaleInput.insert(scaleInput.cursorPosition, key)
        }

        function scaleOnDeleteClicked()
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

        NumericKeyboard {
            id: numericKeyboard
            x: 0
            y: 330

            onKeyClicked: function onKeyClicked(key)
            {
                if (scaleInput.focus)
                {
                    parent.scaleOnKeyClicked(key)
                }
            }
            onDeleteClicked: function onDeleteClicked()
            {
                if (scaleInput.focus)
                {
                    parent.scaleOnDeleteClicked()
                }
            }
        }

        Text {
            id: scaleInfo
            x: 489
            y: 362
            width: 153
            height: 34
            color: "#ffffff"
            text: qsTr("Waga [kg]")
            font.pixelSize: 19
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.weight: Font.Bold
        }

        Image {
            id: scaleInputBackground
            x: 481
            y: 402
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

        Button {
            id: startButton
            x: 489
            y: 488
            width: 153
            height: 54
            text: qsTr("Start")
            font.weight: Font.Bold

            enabled: true

            background: Rectangle {
                opacity: enabled ? 1 : 0.3
                color: startButton.down ? "#DCDCDC" : "#2F4F4F"
                radius: 8
                border.color: startButton.down ? "#DCDCDC" : "black"
                border.width: 2
            }
            font.pointSize: 15
            contentItem: Text {
                opacity: enabled ? 1.0 : 0.3
                color: startButton.down ? "black" : "white"
                text: startButton.text
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: startButton.font
            }

            onPressed:
            {
                BackendConnector.setScale(scaleInput.text)
                BackendConnector.clickStart()
            }
        }

        Button {
            id: exportButton
            x: 756
            y: 488
            width: 159
            height: 54

            text: qsTr("Eksport danych")
            font.weight: Font.Bold

            enabled: false


            background: Rectangle {
                opacity: enabled ? 1 : 0.3
                color: exportButton.down ? "#DCDCDC" : "#2F4F4F"
                radius: 8
                border.color: exportButton.down ? "#DCDCDC" : "black"
                border.width: 2
            }
            font.pointSize: 12
            contentItem: Text {
                opacity: enabled ? 1.0 : 0.3
                color: exportButton.down ? "black" : "white"
                text: exportButton.text
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: exportButton.font
            }
        }

        ContentPreview {
            id: contentPreview
            x: 406
            y: 0
        }
    }

    DoorPopup
    {
        id: doorPopup
        visible: false
    }

    CalibrationPopup
    {
        id: calibPopup
        visible: false
    }

    ConfigPopup
    {
        id: configPopup
        visible: false
    }

    MeasurePopup
    {
        id: measurePopup
        visible: false
    }

    Connections {
        target: configPopup
        function onNext2Clicked(height)
        {
            BackendConnector.configEndClicked(height)
        }
    }

    Connections {
        target: BackendConnector
        function onBlockStartClick(blocked)
        {
            startButton.enabled = blocked ? false : true
        }

        function onBlockSetHeightClick(blocked)
        {
            heightButton.enabled = blocked ? false : true
        }

        function onBlockExportClick(blocked)
        {
            exportButton.enabled = blocked ? false : true
        }

        function onShowDoorPopup(visible)
        {
            doorPopup.visible = visible
        }

        function onShowCalibrationPopup(visible)
        {
            calibPopup.visible = visible
        }

        function onOpenConfigPopup()
        {
            configPopup.step = 0
            configPopup.visible = true
        }

        function onCloseConfigPopup()
        {
            configPopup.visible = false
        }

        function onSetWaitPopupState(visible)
        {
            measurePopup.indicatorVisible = true
            measurePopup.infoText = qsTr("Proszę czekać")
            measurePopup.visible = visible
        }
    
        function onSetStartPopupState(visible)
        {
            measurePopup.indicatorVisible = false
            measurePopup.infoText = qsTr("Wciśnij START")
            measurePopup.visible = visible
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:1.25;height:600;width:1024}
}
##^##*/

