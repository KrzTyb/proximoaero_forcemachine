import QtQuick

import Main.ChartJS 1.0

Item {
    id: rootChartLine

    function setChartData(measurements)
    {
        chartLine.chartDataList = measurements
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
                                labelString: 'Czas [ms]',
                                fontColor: "black"
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                max: 3500.0,
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
                                min: 0.0,
                                max: 120.0,
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
