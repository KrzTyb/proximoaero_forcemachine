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
                                // labelString: 'Przemieszczenie [mm]',
                                labelString: 'Czas [s]',
                                fontColor: "black"
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                // max: 35.0,
                                max: 2.0,
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
                                // labelString: 'Siła [N]',
                                labelString: 'Napięcie [V]',
                                fontColor: "black"
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                // max: 1000.0,
                                max: 3.3,
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
