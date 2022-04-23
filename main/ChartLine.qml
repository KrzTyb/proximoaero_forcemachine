import QtQuick

import Main.ChartJS 1.0

Item {
    id: rootChartLine

    function getxMax(measurements)
    {
        let maxX = 0.0
        for (let value of measurements)
        {
            if (value.x > maxX)
            {
                maxX = value.x
            }
        }
        return maxX
    }
    function getyMax(measurements)
    {
        let maxY = 0.0
        for (let value of measurements)
        {
            if (value.y > maxY)
            {
                maxY = value.y
            }
        }
        return maxY
    }
    function getyMin(measurements)
    {
        if (measurements.length <= 0)
        {
            return 0.0
        }

        let minY = measurements[0].y
        for (let value of measurements)
        {
            if (value.y < minY)
            {
                minY = value.y
            }
        }
        return minY
    }

    function setChartData(measurements)
    {
        chartLine.chartDataList = measurements

        chartLine.chartOptions.scales.xAxes[0].ticks.max = Math.ceil((getxMax(measurements)) * 1000.0) / 1000.0
        chartLine.chartOptions.scales.yAxes[0].ticks.max = Math.round((getyMax(measurements) + 200.0) / 100.0) * 100.0
        chartLine.chartOptions.scales.yAxes[0].ticks.min = Math.round((getyMin(measurements) - 200.0) / 100.0) * 100.0
        chartLine.requestPaint()
    }

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    ChartView {
        id: chartLine
        anchors.fill: parent
        chartType: 'line'

        property var chartDataList

        chartData: {
            return {
                datasets: [{
                        backgroundColor: 'rgba(0,0,0,0)',
                        borderColor: '#990000',
                        data: chartDataList
                    }]
            }
        }

        chartOptions: {
            return {
                animation: false,
                scales: {
                    xAxes: [{
                            display: true,
                            type: 'linear',
                            scaleLabel: {
                                display: true,
                                labelString: 'Przemieszczenie [m]',
                                fontColor: "black"
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                // max: 0.09,
                                max: 0.0,
                                fontColor: "black"
                            },
                            gridLines: {
                                display: true,
                                color: "black"
                            }
                        }],
                    yAxes: [{
                            display: true,
                            type: 'linear',
                            scaleLabel: {
                                display: true,
                                labelString: 'Siła [N]',
                                fontColor: "black"
                            },
                            ticks: {
                                beginAtZero: true,
                                min: -100.0,
                                // max: 1200.0,
                                fontColor: "black"
                            },
                            gridLines: {
                                display: true,
                                color: "black"
                            }
                        }]
                },
                legend: {
                    display: false
                },
                elements: {
                    point: {
                        radius: 0
                    }
                },
                decimation: {
                    enabled: true
                }
            }
        }
    }
}
