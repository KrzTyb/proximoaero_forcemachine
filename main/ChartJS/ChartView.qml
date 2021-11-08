/*!
 * Elypson's Chart.qml adaptor to Chart.js
 * (c) 2020 ChartJs2QML contributors (starting with Elypson, Michael A. Voelkel, https://github.com/Elypson)
 * Released under the MIT License
 */

import QtQuick
import "Chart.js" as Chart

Canvas {
    id: root

    property var jsChart: undefined
    property string chartType
    property var chartData
    property var chartOptions
    property double chartAnimationProgress: 0.1
    property var animationEasingType: Easing.InOutExpo
    property double animationDuration: 500
    property var memorizedContext
    property var memorizedData
    property var memorizedOptions
    property alias animationRunning: chartAnimator.running
    
    signal animationFinished()

    function animateToNewData()
    {
        chartAnimationProgress = 0.1;
        jsChart.update();
        chartAnimator.restart();
    }

    PropertyAnimation {
        id: chartAnimator
        target: root
        property: "chartAnimationProgress"
        alwaysRunToEnd: true
        to: 1
        duration: root.animationDuration
        easing.type: root.animationEasingType
        onFinished: {
            root.animationFinished();
        }
    }

    onChartAnimationProgressChanged: {
        root.requestPaint();
    }

    onPaint: {
        if(root.getContext('2d') != null && memorizedContext != root.getContext('2d') || memorizedData != root.chartData || memorizedOptions != root.chartOptions) {
            var ctx = root.getContext('2d');

            jsChart = new Chart.build(ctx, {
                type: root.chartType,
                data: root.chartData,
                options: root.chartOptions
                });

            memorizedData = root.chartData ;
            memorizedContext = root.getContext('2d');
            memorizedOptions = root.chartOptions;

            chartAnimator.start();
        }

        jsChart.draw(chartAnimationProgress);
    }

    onWidthChanged: {
        if(jsChart) {
            jsChart.resize();
        }
    }

    onHeightChanged: {
        if(jsChart) {
            jsChart.resize();
        }
    }
}
