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
                                MessageController.addTarget(ip1.text, parseInt(port1.text));
                                receiver1Connected = true;
                            } else {
                                MessageController.removeTarget(ip1.text, parseInt(port1.text));
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
                                MessageController.addTarget(ip2.text, parseInt(port2.text));
                                receiver2Connected = true;
                            } else {
                                MessageController.removeTarget(ip2.text, parseInt(port2.text));
                                receiver2Connected = false;
                            }
                        }
                    }
                }
            }
        }


        GroupBox {
            title: "Messages"
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width
            Layout.fillHeight: true
            RowLayout {
                anchors.fill: parent
                spacing: 5

                Message{
                    id: message_1
                }

                Message{
                    id: message_2
                }

                Repeater {
                    model: [
                        { messageNum: 1, messageObj: message_1 },
                        { messageNum: 2, messageObj: message_2 }
                    ]

                    Rectangle {
                        id: msRect
                        color: "#f5f5f5"
                        radius: 8
                        border.color: "#cccccc"
                        Layout.preferredWidth: parent.width / 7 * 2
                        Layout.fillHeight: true
                        property bool messageIsSending: false
                        property var messageObj: modelData.messageObj
                        
                        ColumnLayout {
                            spacing: 15

                            GridLayout {
                                columns: 2
                                Layout.fillWidth: true
                                Layout.margins: 5
                                rowSpacing: 10
                                columnSpacing: 10

                                Label { text: "Message " + modelData.messageNum}
                                RowLayout {
                                    Button {
                                        text: " Start Send "
                                        onClicked: {
                                            MessageController.startSendMessage(modelData.messageObj)
                                            msRect.messageIsSending = true
                                        }
                                    }
                                    Button {
                                        text: "     Stop     "
                                        onClicked: {
                                            MessageController.stopSendMessage(modelData.messageObj)
                                            msRect.messageIsSending = false
                                        }
                                    }
                                }

                                Label { text: "Content:" }
                                TextField {
                                    Layout.fillWidth: true
                                    placeholderText: modelData.messageObj.content
                                    onTextChanged: modelData.messageObj.content = text
                                    enabled: !msRect.messageIsSending
                                }

                                Label { text: "Interval (ms):" }
                                SpinBox {
                                    from: 0; to: 10000; value: modelData.messageObj.intervalMs; stepSize: 100
                                    Layout.preferredWidth: 70
                                    onValueChanged: modelData.messageObj.intervalMs = value
                                    editable: true
                                    enabled: !msRect.messageIsSending
                                }
                            }

                            Button {
                                text: "Add Field"
                                onClicked: {
                                    modelData.messageObj.customFieldModel.addField("", "")
                                }
                                enabled: !msRect.messageIsSending
                            }

                            ListView {
                                Layout.preferredWidth: msRect.width
                                Layout.preferredHeight: contentHeight
                                model: modelData.messageObj.customFieldModel
                                spacing: 5
                                clip: true
                                interactive: false

                                delegate: Rectangle {
                                    width: msRect.width
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
                                            Layout.fillWidth: true
                                            placeholderText: "Key"
                                            text: key
                                            onTextChanged: msRect.messageObj.customFieldModel.updateKey(index,text)
                                            enabled: !msRect.messageIsSending
                                        }
                                        TextField {
                                            Layout.fillWidth: true
                                            placeholderText: "Value"
                                            text: value
                                            onTextChanged: msRect.messageObj.customFieldModel.updateValue(index,text)
                                            enabled: !msRect.messageIsSending
                                        }
                                        Button {
                                            text: "X"
                                            Layout.preferredWidth: 30
                                            onClicked: msRect.messageObj.customFieldModel.removeField(index)
                                            enabled: !msRect.messageIsSending
                                        }
                                        Item { Layout.fillWidth: true }
                                    }
                                }
                            }
                        }
                    }
                }

                // Phần 3: Add Message + ListView
                Rectangle {
                    id: msGroupRect
                    color: "#f5f5f5"
                    radius: 8
                    border.color: "#cccccc"
                    Layout.fillWidth: parent.width/7*3
                    Layout.fillHeight: true
                    property bool isGroupSending: false
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        RowLayout{
                            spacing: 5

                            Button {
                                text: "Send All Interval (ms)"
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {
                                    MessageController.startSendGroupMessage()
                                    msGroupRect.isGroupSending = true
                                }
                            }
                            SpinBox {
                                id: interval; from: 0; to: 10000; value: MessageController.intervalGroupMs; stepSize: 100; width: 50
                                onValueChanged:  MessageController.intervalGroupMs = value
                                enabled: !msGroupRect.isGroupSending
                            }
                            Button {
                                text: "Stop Send MS"
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {
                                    MessageController.stopSendGroupMessage()
                                    msGroupRect.isGroupSending = false
                                }
                            }
                        }
                        Button {
                            text: "Send One Time Follow Order"
                            onClicked: {
                                MessageController.sendGroupMessageFollowOder()
                            }
                        }
                        Button {
                            text: "Add Message"
                            onClicked: {
                                MessageManager.addMessage()

                            }
                            enabled: !msGroupRect.isGroupSending
                        }

                        ListView {
                            id: messageList
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: MessageManager
                            spacing: 10
                            delegate: Rectangle {
                                width: messageList.width
                                height: Math.max(60 + fieldList3.contentHeight + 20, 80)
                                color: index % 2 === 0 ? "#f5f5f5" : "#e0e0e0"
                                radius: 5
                                border.width: 1
                                border.color: "#cccccc"
                                property var msgObj: MessageManager.getMessage(msgId)
                                
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 4
                                    
                                    RowLayout {
                                        id: messageRow
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 40
                                        spacing: 8
                                        
                                        Label { 
                                            text: "ID " + msgId
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
                                                MessageManager.updateContent(msgId, text)
                                            }
                                            enabled: !msGroupRect.isGroupSending
                                        }
                                        Button {
                                            text: "Add Field"
                                            onClicked: {
                                                if (msgObj && msgObj.customFieldModel) {
                                                    msgObj.customFieldModel.addField("", "")
                                                }
                                            }
                                            enabled: !msGroupRect.isGroupSending
                                        }
                                        Button {
                                            text: "X"
                                            Layout.preferredWidth: 30
                                            onClicked: MessageManager.deleteMessage(msgId)
                                            enabled: !msGroupRect.isGroupSending
                                        }
                                        Item { Layout.fillWidth: true }
                                    }
                                    
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 1
                                        color: "#999999"
                                    }
                                    
                                    ListView {
                                        id: fieldList3
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: contentHeight
                                        model: msgObj ? msgObj.customFieldModel : null
                                        spacing: 5
                                        clip: true
                                        interactive: false
                                        
                                        delegate: Rectangle {
                                            width: fieldList3.width
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
                                                    onTextChanged: msgObj.customFieldModel.updateKey(index,text)
                                                    enabled: !msGroupRect.isGroupSending
                                                }
                                                TextField {
                                                    Layout.preferredWidth: 80
                                                    placeholderText: "Value"
                                                    text: value
                                                    onTextChanged: msgObj.customFieldModel.updateValue(index,text)
                                                    enabled: !msGroupRect.isGroupSending
                                                }
                                                Button {
                                                    text: "X"
                                                    Layout.preferredWidth: 30
                                                    onClicked: msgObj.customFieldModel.removeField(index)
                                                    enabled: !msGroupRect.isGroupSending
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
