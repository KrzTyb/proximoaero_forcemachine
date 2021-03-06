import QtQuick
import QtQuick.Controls
import QtMultimedia



Item {
    id: root_contentpreview
    width: 618
    height: 342

    Image {
        id: background
        anchors.fill: parent
        source: "images/Tile.png"
    }
    Item {
        id: static_images
        anchors.fill: parent

        Image {
            id: camera_image
            x: 113
            y: 87
            source: "images/Camera.png"
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: chart
            x: 330
            y: 83
            source: "images/Chart.png"
            fillMode: Image.PreserveAspectFit
        }
    }

    Item {
        id: camera_view
        visible: false
        anchors.fill: parent

        CaptureSession {
            id: capture_session
            videoOutput: camera_output
            camera: Camera {

            }
        }

        VideoOutput {
            id: camera_output
            width: parent.width - 8
            height: parent.height - 8
            x: 8
            fillMode: VideoOutput.PreserveAspectCrop
        }
    }


    Item {
        id: chart_view
        visible: false
        width: parent.width - 8
        height: parent.height - 8
        x: 8

        ChartLine {
            id: chartline
            anchors.fill: parent
        }

    }

    Connections {
        target: BackendConnector
        function onSetCameraVisible(visible)
        {
            camera_view.visible = visible
        }

        function onSetChartVisible(visible)
        {
            if (visible)
            {
                static_images.visible = false
                chart_view.visible = true
            }
            else
            {
                static_images.visible = true
                chart_view.visible = false
            }
        }

        function onMeasurementsReceived(measurements)
        {
            chartline.setChartData(measurements)
        }
    }
}
