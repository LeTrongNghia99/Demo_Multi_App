import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 500
    height: 600
    title: "UDP Multicast Receiver 2"

    Rectangle {
        anchors.fill: parent
        color: "white" // Đặt màu nền là trắng
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        GroupBox {
            id: connectGroupBox
            title: "Join Multicast Group"
            Layout.fillWidth: true
            RowLayout {
                spacing: 8
                Label { text: "Multicast IP:" }
                TextField { id: ipField; width: 120; placeholderText: "239.255.0.1" }
                Label { text: "Port:" }
                TextField { id: portField; width: 70; inputMethodHints: Qt.ImhDigitsOnly; placeholderText: "6000" }
                Button {
                    text: "Join Group"
                    onClicked: receiver.joinGroup(ipField.text, parseInt(portField.text))
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Button {
                onClicked: logModel.clear()
                contentItem: Text {
                    text: "Clear Log"
                    color: "black"
                    font: control.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                }
            }
        }

        ListView {
            id: logView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: logModel
            delegate: Rectangle {
                width: logView.width
                height: 48 + (customFields ? Object.keys(customFields).length * 10 : 0)
                border.color: "black"
                border.width: 1
                radius: 6
                property string d_time: time !== undefined ? String(time) : ""
                property string d_msgId: msgId !== undefined ? String(msgId) : ""
                property string d_content: content !== undefined ? String(content) : ""
                
                Column {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing: 2
                    
                    RowLayout {
                        width: parent.width
                        spacing: 8
                        Text { text: '[' + d_time + ']'; width: 120; color: "black" }
                        Text { text: '[Msg_Id: ' + d_msgId + ']'; width: 80; color: "black" }
                        Text { text: '[Content: ' + d_content + ']'; elide: Text.ElideRight; Layout.fillWidth: true; color: "black" }
                    }
                    
                    // Hiển thị customFields nếu có
                    Repeater {
                        model: customFields ? Object.keys(customFields) : []
                        Text { text: '[' + modelData + ' : ' + customFields[modelData] + ']'; width: 80 ;color: "black"}
                    }
                }
                
                Component.onCompleted: {
                    console.log("Delegate created:", d_time, d_msgId, d_content, customFields)
                }
            }
            property bool userAtEnd: true
            property int autoScrollThreshold: 80 // px, chiều cao tối đa cách cuối vẫn auto-scroll
            onCountChanged: {
                console.log("ListView count changed:", count)
                if (userAtEnd && count > 0) logView.positionViewAtEnd();
            }
            onContentYChanged: {
                // Nếu người dùng đang ở gần cuối (cách cuối tối đa autoScrollThreshold px), vẫn auto-scroll
                userAtEnd = (logView.contentY + logView.height >= logView.contentHeight - autoScrollThreshold);
            }
        }
        ListModel {
            id: logModel
            function appendLog(entry) {
                console.log("ListModel appendLog:", entry)
                append(entry)
            }
        }
        Connections {
            target: receiver
            function onMessageReceived(msgId, content, time, customFields) {
                console.log("QML received:", msgId, content, time, customFields);
                logModel.appendLog({msgId: msgId, content: content, time: time, customFields: customFields});
            }
        }
        }
        Item { Layout.fillHeight: true } // Spacer
    
}
