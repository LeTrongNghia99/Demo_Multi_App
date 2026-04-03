import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Sender 1.0

ApplicationWindow {

    property bool receiver1Connected: false
    property bool receiver2Connected: false
    visible: true
    width: 800
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

        // Button{
        //     id: addMsgBtn
        //     Layout.alignment: Qt.AlignHCenter
        //     contentItem: Text {
        //         text: "Add Message"
        //         color: "black"
        //         font: control.font
        //         horizontalAlignment: Text.AlignHCenter
        //         verticalAlignment: Text.AlignVCenter
        //         anchors.fill: parent
        //     }
        //     onClicked: {
        //         MessageController.addMessage()
        //     }
        // }

        // GroupBox {
        //     anchors{
        //         top:addMsgBtn.bottom
        //         topMargin: 50
        //     }

        //     title: "Messages"
        //     Layout.fillWidth: true


            // ListView {
            //     id: messageList
            //     anchors.left: parent.left
            //     anchors.right: parent.right
            //     height: 300
            //     model: MessageController
            //     delegate: Rectangle {
            //         property bool isClickedFirsTime : false
            //         width: messageList.width
            //         height: 50
            //         color: index % 2 === 0 ? "#f5f5f5" : "#e0e0e0"
            //         RowLayout {
            //             anchors.fill: parent
            //             spacing: 8
            //             Label { text: "ID: " + msgId; width: 50 }
            //             Label { text: "Content:"; width: 60 }
            //             TextField {
            //                 text: content
            //                 width: 120
            //                 // Có thể thêm onTextChanged để cập nhật model nếu muốn\
            //                 onTextChanged: {
            //                     MessageController.updateContent(msgId, text)
            //                 }
            //             }
            //             Label { text: "Interval (ms):"; width: 90 }
            //             SpinBox {
            //                 value: intervalMs
            //                 from: 0; to: 10000; stepSize: 100; width: 80
            //                 // Có thể thêm onValueChanged để cập nhật model nếu muốn
            //                 onValueChanged: {
            //                     MessageController.updateInterval(msgId, value)
            //                 }
            //             }
            //             Button {
            //                 text: "Start Send"
            //                 onClicked: {
            //                     MessageController.startSendMessage(msgId,sender)
            //                 }
            //             }
            //             Button{
            //                 text: "Stop Send"
            //                 onClicked: MessageController.stopSendMessage(msgId,sender)
            //             }
            //             Button {
            //                 text: "Delete Message"
            //                 onClicked: MessageController.deleteMessage(msgId)
            //             }
            //         }
            //     }
            // }
        // }

        GroupBox {
            title: "Messages"
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout {
                anchors.fill: parent
                spacing: 5

                // Phần 1: Message 1
                Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: "#cccccc"
                    Layout.preferredWidth:  parent.width / 4
                    Layout.fillHeight: true
                    ColumnLayout {

                        spacing: 15
                        Label { text: "Message 1"}
                        RowLayout{
                            Label { text: "Content:"; }
                            TextField { id: msg1Content; width: 200; placeholderText: "" }
                        }
                        RowLayout{
                            Label { text: "Interval (ms):"; }
                            SpinBox { id: msg1Interval; from: 0; to: 10000; value: 1000; stepSize: 100; width: 100 }
                        }

                        Button { text: "Add Field"; onClicked: {/* TODO: logic thêm field cho message 1 */} }
                    }
                }

                // Phần 2: Message 2
                Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: "#cccccc"
                    Layout.preferredWidth: parent.width / 4
                    Layout.fillHeight: true
                    ColumnLayout {

                        spacing: 15
                        Label { text: "Message 2"}
                        RowLayout{
                            Label { text: "Content:"; }
                            TextField { id: msg2Content; width: 200; placeholderText: "" }
                        }
                        RowLayout{
                            Label { text: "Interval (ms):"; }
                            SpinBox { id: msg2Interval; from: 0; to: 10000; value: 1000; stepSize: 100; width: 100 }
                        }

                        Button { text: "Add Field"; onClicked: {/* TODO: logic thêm field cho message 2 */} }
                    }
                }

                // Phần 3: Add Message + ListView
                Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: "#cccccc"
                    Layout.fillWidth: parent.width/5*3
                    Layout.fillHeight: true
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        RowLayout{
                            spacing: 15

                            Button {
                                text: "Send All Interval (ms)"
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {

                                }
                            }
                            SpinBox { id: interval; from: 0; to: 10000; value: 1000; stepSize: 100; width: 100 }
                            Button {
                                text: "Send Follow Order"
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {

                                }
                            }
                        }
                        Button {
                            text: "Add Message"
                            // Layout.alignment: Qt.AlignHCenter
                            onClicked: {
                                MessageController.addMessage()
                            }
                        }

                        ListView {
                            id: messageList
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: MessageController
                            spacing: 10
                            delegate: Rectangle {
                                property bool isClickedFirsTime : false
                                width: messageList.width
                                height: Math.max(60 + fieldList.contentHeight + 20, 80)
                                color: index % 2 === 0 ? "#f5f5f5" : "#e0e0e0"
                                radius: 5
                                border.width: 1
                                border.color: "#cccccc"
                                property var msgObj: MessageController.getMessage(msgId)
                                
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 8
                                    
                                    RowLayout {
                                        id: messageRow
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 40
                                        spacing: 8
                                        
                                        Label { 
                                            text: "ID: " + msgId
                                            width: 50 
                                        }
                                        Label { 
                                            text: "Content:"
                                            width: 60 
                                        }
                                        TextField {
                                            text: content
                                            Layout.preferredWidth: 120
                                            onTextChanged: {
                                                MessageController.updateContent(msgId, text)
                                            }
                                        }
                                        Button {
                                            text: "Add Field"
                                            onClicked: {
                                                if (msgObj && msgObj.customFieldModel) {
                                                    msgObj.customFieldModel.addField("", "")
                                                }
                                            }
                                        }
                                        Button {
                                            text: "Delete Message"
                                            onClicked: MessageController.deleteMessage(msgId)
                                        }
                                        Item { Layout.fillWidth: true }
                                    }
                                    
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 1
                                        color: "#999999"
                                    }
                                    
                                    ListView {
                                        id: fieldList
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: contentHeight
                                        model: msgObj ? msgObj.customFieldModel : null
                                        spacing: 5
                                        clip: true
                                        interactive: false
                                        
                                        delegate: Rectangle {
                                            width: fieldList.width
                                            height: 40
                                            color: "#ffffff"
                                            border.width: 1
                                            border.color: "#dddddd"
                                            radius: 3
                                            
                                            RowLayout {
                                                anchors.fill: parent
                                                anchors.margins: 5
                                                spacing: 8
                                                
                                                TextField {
                                                    Layout.preferredWidth: 80
                                                    placeholderText: "Key"
                                                    text: key
                                                }
                                                TextField {
                                                    Layout.preferredWidth: 80
                                                    placeholderText: "Value"
                                                    text: value
                                                }
                                                Button {
                                                    text: "X"
                                                    Layout.preferredWidth: 30
                                                    onClicked: msgObj.customFieldModel.removeField(index)
                                                }
                                                Item { Layout.fillWidth: true }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
