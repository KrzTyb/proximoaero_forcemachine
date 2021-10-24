import QtQuick

import ChartJS

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

    Chart {
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
                                labelString: 'Przemieszczenie'
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                max: 200.0
                            }
                        }],
                    yAxes: [{
                            display: true,
                            type: 'linear',
                            scaleLabel: {
                                display: true,
                                labelString: 'Siła'
                            },
                            ticks: {
                                beginAtZero: true,
                                min: 0.0,
                                max: 100.0
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
