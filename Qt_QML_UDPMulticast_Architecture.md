
# Software Architecture Specification: UDP Multicast System (Qt/QML)

## 1. System Overview

- **Project Type:** Desktop Application
- **Framework:** Qt 6 (C++ for Backend Logic, QML for Frontend UI)
- **Network Protocol:** UDP Multicast (Supports both IPv4 and IPv6)
- **Data Format:** JSON Payload
- **System Components:**
  - **1 Sender App:** Configures multicast groups, sends 3 types of messages (2 continuous timers, 1 one-time trigger)
  - **2 Receiver Apps:** Identical clones. They join the multicast group and display incoming messages in a continuous scrolling log

---

## 2. Project Directory Structure

Recommended CMake subdirs structure to manage both applications in one workspace:

```text
MulticastProject/
├── CMakeLists.txt
├── SenderApp/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── MulticastSender.h
│   ├── MulticastSender.cpp
│   └── Main.qml
└── ReceiverApp/
    ├── CMakeLists.txt
    ├── main.cpp
    ├── MulticastReceiver.h
    ├── MulticastReceiver.cpp
    └── Main.qml
```

---

## 3. Message JSON Format

All messages sent via UDP Multicast must be formatted as a JSON string. The Receiver will parse this JSON to display the log.

```json
{
  "msg_id": "ms1",          // Value can be: "ms1", "ms2", or "ms3"
  "content": "Hello World", // Plain text entered by the user
  "interval_ms": 1000,      // Interval time in ms (0 for ms3)
  "timestamp": 1712030000   // Unix epoch timestamp of when the message was sent
}
```

---

## 4. Sender App Specification

### 4.1. UI/UX Design (QML - Main.qml)

The UI is divided into two main areas:

- **Top (Network Config):**
  - Inputs to define Multicast Group IP and Port for Receiver 1 and Receiver 2
  - Button: **Connect Receiver 1** (Saves configuration for Target 1)
  - Button: **Connect Receiver 2** (Saves configuration for Target 2)

- **Bottom (Message Controls):**
  - **MS1 Area (Continuous):**
    - TextField (Message Content)
    - TextField or SpinBox (Interval in ms)
    - Button (Start Timer)
    - Button (Stop Timer)
  - **MS2 Area (Continuous):**
    - TextField (Message Content)
    - TextField or SpinBox (Interval in ms)
    - Button (Start Timer)
    - Button (Stop Timer)
  - **MS3 Area (One-time):**
    - TextField (Message Content)
    - Button (Send Now)

### 4.2. Backend Logic (C++ - MulticastSender)

- **Class Inheritance:** Inherits from `QObject`
- **Network:** Uses `QUdpSocket` for broadcasting
- **Timers:** Uses two `QTimer` instances (one for MS1, one for MS2)
- **Target Storage:** Stores target addresses/ports defined by the user (e.g., `QList<QPair<QHostAddress, quint16>> m_targets`)
- **Exposed to QML (`Q_INVOKABLE`):**
  - `void addTarget(QString ip, int port)`: Adds or updates a target address
  - `void startMs1(QString content, int intervalMs)`: Starts Timer 1
  - `void stopMs1()`: Stops Timer 1
  - `void startMs2(QString content, int intervalMs)`: Starts Timer 2
  - `void stopMs2()`: Stops Timer 2
  - `void sendMs3(QString content)`: Sends MS3 immediately
- **Internal Private Method:**
  - `void broadcastMessage(QString msgId, QString content, int interval)`: Constructs the JSON, converts to `QByteArray`, and calls `writeDatagram` to all addresses in `m_targets`

---

## 5. Receiver App Specification

### 5.1. UI/UX Design (QML - Main.qml)

- **Top Bar:**
  - TextField (Multicast IP to join)
  - TextField (Port to bind)
  - Button (**Join Group**)
- **Main Area (Log Viewer):**
  - ListView paired with a ListModel
  - Must auto-scroll to the bottom when a new item is added (`positionViewAtEnd()`)
  - Delegate Design: Displays `[Time] | [Type: ms1/ms2/ms3] | Content: [Text]`
- **Bottom Bar:**
  - Button (**Clear Log**) to clear the ListModel

### 5.2. Backend Logic (C++ - MulticastReceiver)

- **Class Inheritance:** Inherits from `QObject`
- **Network:** Uses `QUdpSocket` to bind and join multicast groups
- **Exposed to QML (`Q_INVOKABLE`):**
  - `bool joinGroup(QString ip, int port)`: Executes `m_udpSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)` and `m_udpSocket->joinMulticastGroup(QHostAddress(ip))`
- **Data Handling:**
  - Connects `m_udpSocket->readyRead()` signal to a private slot `readPendingDatagrams()`
  - Reads `QByteArray`, parses using `QJsonDocument`
  - Extracts fields: `msg_id`, `content`, `timestamp`
  - Emits signal to QML: `void messageReceived(QString msgId, QString content, QString timeStr)`
- **QML Integration:**
  - QML uses `Connections` to listen to `messageReceived` and appends the data to the ListModel

---

## 6. Important Technical Requirements

- **Socket Options:** Enable `QAbstractSocket::MulticastLoopbackOption` to allow testing Sender and Receiver on the same local machine
- **Thread Safety:** Network operations and UI updates must be handled correctly (Signals/Slots across threads if `QUdpSocket` is moved to a worker thread, though main thread is usually fine for simple UDP)
- **JSON Parsing:** Ensure robust error handling when parsing JSON in the Receiver (check `isObject()` before extracting fields)