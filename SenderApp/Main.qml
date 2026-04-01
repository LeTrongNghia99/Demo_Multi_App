import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Sender 1.0

ApplicationWindow {
    visible: true
    width: 500
    height: 600
    title: "UDP Multicast Sender"

    MulticastSender {
        id: sender
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        GroupBox {
            title: "Network Config"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 8
                    Label { text: "Receiver 1 IP:" }
                    TextField { id: ip1; width: 120; placeholderText: "224.0.0.1" }
                    Label { text: "Port:" }
                    TextField { id: port1; width: 70; inputMethodHints: Qt.ImhDigitsOnly; placeholderText: "5001" }
                    Button {
                        text: "Connect Receiver 1"
                        onClicked: sender.addTarget(ip1.text, parseInt(port1.text))
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Receiver 2 IP:" }
                    TextField { id: ip2; width: 120; placeholderText: "224.0.0.2" }
                    Label { text: "Port:" }
                    TextField { id: port2; width: 70; inputMethodHints: Qt.ImhDigitsOnly; placeholderText: "5002" }
                    Button {
                        text: "Connect Receiver 2"
                        onClicked: sender.addTarget(ip2.text, parseInt(port2.text))
                    }
                }
            }
        }

        GroupBox {
            title: "MS1 (Continuous)"
            Layout.fillWidth: true
            RowLayout {
                spacing: 8
                Label { text: "Content:" }
                TextField { id: ms1Content; width: 120 }
                Label { text: "Interval (ms):" }
                SpinBox { id: ms1Interval; from: 100; to: 10000; value: 1000; stepSize: 100; width: 80 }
                Button {
                    text: "Start Timer"
                    onClicked: sender.startMs1(ms1Content.text, ms1Interval.value)
                }
                Button {
                    text: "Stop Timer"
                    onClicked: sender.stopMs1()
                }
            }
        }

        GroupBox {
            title: "MS2 (Continuous)"
            Layout.fillWidth: true
            RowLayout {
                spacing: 8
                Label { text: "Content:" }
                TextField { id: ms2Content; width: 120 }
                Label { text: "Interval (ms):" }
                SpinBox { id: ms2Interval; from: 100; to: 10000; value: 2000; stepSize: 100; width: 80 }
                Button {
                    text: "Start Timer"
                    onClicked: sender.startMs2(ms2Content.text, ms2Interval.value)
                }
                Button {
                    text: "Stop Timer"
                    onClicked: sender.stopMs2()
                }
            }
        }

        GroupBox {
            title: "MS3 (One-time)"
            Layout.fillWidth: true
            RowLayout {
                spacing: 8
                Label { text: "Content:" }
                TextField { id: ms3Content; width: 120 }
                Button {
                    text: "Send Now"
                    onClicked: sender.sendMs3(ms3Content.text)
                }
            }
        }
        Item { Layout.fillHeight: true } // Spacer
    }
}
