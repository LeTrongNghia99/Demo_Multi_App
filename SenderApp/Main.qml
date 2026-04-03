import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Sender 1.0

ApplicationWindow {

    property bool receiver1Connected: false
    property bool receiver2Connected: false
    visible: true
    width: 700
    height: 600
    title: "UDP Multicast Sender"

    Rectangle {
        anchors.fill: parent
        color: "white" // Đặt màu nền là trắng
    }

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
                    TextField {
                        id: ip1
                        width: 120
                        placeholderText: "224.0.0.1"
                        enabled: !receiver1Connected
                    }
                    Label { text: "Port:" }
                    TextField {
                        id: port1
                        width: 70
                        inputMethodHints: Qt.ImhDigitsOnly
                        placeholderText: "5001"
                        enabled: !receiver1Connected
                    }
                    Button {
                        text: receiver1Connected ? "Disconnect Receiver 1" : "Connect Receiver 1"
                        onClicked: {
                            if (!receiver1Connected) {
                                sender.addTarget(ip1.text, parseInt(port1.text));
                                receiver1Connected = true;
                            } else {
                                sender.removeTarget(ip1.text, parseInt(port1.text));
                                receiver1Connected = false;
                            }
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Receiver 2 IP:" }
                    TextField {
                        id: ip2
                        width: 120
                        placeholderText: "224.0.0.2"
                        enabled: !receiver2Connected
                    }
                    Label { text: "Port:" }
                    TextField {
                        id: port2
                        width: 70
                        inputMethodHints: Qt.ImhDigitsOnly
                        placeholderText: "5002"
                        enabled: !receiver2Connected
                    }
                    Button {
                        text: receiver2Connected ? "Disconnect Receiver 2" : "Connect Receiver 2"
                        onClicked: {
                            if (!receiver2Connected) {
                                sender.addTarget(ip2.text, parseInt(port2.text));
                                receiver2Connected = true;
                            } else {
                                sender.removeTarget(ip2.text, parseInt(port2.text));
                                receiver2Connected = false;
                            }
                        }
                    }
                }
            }
        }

        Button{
            id: addMsgBtn
            Layout.alignment: Qt.AlignHCenter
            contentItem: Text {
                text: "Add Message"
                color: "black"
                font: control.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }
            onClicked: {
                MessageController.addMessage()
            }  
        }


        GroupBox {
            anchors{
                top:addMsgBtn.bottom
                topMargin: 50
            }

            title: "Messages"
            Layout.fillWidth: true
            ListView {
                id: messageList
                anchors.left: parent.left
                anchors.right: parent.right
                height: 300
                model: MessageController
                delegate: Rectangle {
                    property bool isClickedFirsTime : false
                    width: messageList.width
                    height: 50
                    color: index % 2 === 0 ? "#f5f5f5" : "#e0e0e0"
                    RowLayout {
                        anchors.fill: parent
                        spacing: 8
                        Label { text: "ID: " + msgId; width: 50 }
                        Label { text: "Content:"; width: 60 }
                        TextField {
                            text: content
                            width: 120
                            // Có thể thêm onTextChanged để cập nhật model nếu muốn\
                            onTextChanged: {
                                MessageController.updateContent(msgId, text)
                            }
                        }
                        Label { text: "Interval (ms):"; width: 90 }
                        SpinBox {
                            value: intervalMs
                            from: 0; to: 10000; stepSize: 100; width: 80
                            // Có thể thêm onValueChanged để cập nhật model nếu muốn
                            onValueChanged: {
                                MessageController.updateInterval(msgId, value)
                            }
                        }
                        Button {
                            text: "Start Send"
                            onClicked: {
                                MessageController.startSendMessage(msgId,sender)
                            }
                        }
                        Button{
                            text: "Stop Send"
                            onClicked: MessageController.stopSendMessage(msgId,sender)
                        }
                        Button {
                            text: "Delete Message"
                            onClicked: MessageController.deleteMessage(msgId)
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true } // Spacer
    }
}
