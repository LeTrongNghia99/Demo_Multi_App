# SenderApp - MVC Architecture

## 📁 Folder Structure

```
SenderApp/
├── main.cpp
├── CMakeLists.txt
│
├── Model/                      📊 Data & Business Logic
│   ├── Message.h/cpp          (Single message)
│   ├── MessageManager.h/cpp    (Messages list)
│   ├── CustomField.h/cpp       (Key-value field)
│   ├── CustomFieldModel.h/cpp  (Fields list)
│   └── MulticastSender.h/cpp   (UDP Network)
│
├── Controller/                 🎮 Control Logic
│   └── MessageController.h/cpp (Interaction logic)
│
└── View/                       👁️ User Interface
    └── Main.qml               (QML UI)
```

---

## 🏗️ MVC Architecture

```
┌─────────────────────────┐
│   View (Main.qml)       │ ← Display UI, receive input
└──────────┬──────────────┘
           │ calls methods
┌──────────▼──────────────┐
│  Controller             │ ← Control business logic
│  MessageController      │
└──────────┬──────────────┘
           │
    ┌──────┴──────┐
    │             │
    ▼             ▼
┌─────────┐  ┌──────────────┐
│ Models  │  │MulticastSend │ ← Manage data
│ -Message│  │ (UDP Network)│
│ -Manager│  └──────────────┘
│ -Fields │
└─────────┘
```

---

## 📊 Model Layer

| Class | Purpose |
|-------|---------|
| **Message** | Single message (id, content, interval, timer, fields) |
| **MessageManager** | Manage messages list (QAbstractListModel) |
| **CustomField** | Custom key-value pair |
| **CustomFieldModel** | Manage fields list (QAbstractListModel) |
| **MulticastSender** | Send UDP to receivers, manage targets |

---

## 🎮 Controller Layer

**MessageController**
- Receive input from View (QML)
- Control Model (MessageManager)
- Control Network (MulticastSender)
- **DOES NOT** manage data

---

## 👁️ View Layer

**Main.qml**
- Display messages and receivers
- Bind data from MessageManager
- Call methods from MessageController on user interaction

---

## 🔄 How It Works

### Initialization (main.cpp)
```cpp
MessageManager manager;
MulticastSender sender;
MessageController controller;

controller.setMessageManager(&manager);
controller.setMulticastSender(&sender);

// Create 2 initial messages
manager.addMessage();
manager.addMessage();

// Register to QML
engine.rootContext()->setContextProperty("MessageManager", &manager);
engine.rootContext()->setContextProperty("MessageController", &controller);
```

### User Interaction
```
User clicks "Connect Receiver"
    ↓
QML → MessageController.addTarget(ip, port)
    ↓
Controller → MulticastSender.addTarget()
    ↓
Receiver added to list
```

### Send Message
```
User clicks "Start Send"
    ↓
QML → MessageController.startSendMessage(msgId)
    ↓
Controller → MulticastSender.startSendMessage(message)
    ↓
Timer starts → Send UDP packets periodically
```

---

## 📤 Data Flow

**Model → View** (Data binding)
```
MessageManager.addMessage()
    ↓ emit dataChanged()
    ↓
QML ListView
    ↓ auto update
```

**View → Controller → Model** (User interaction)
```
QML Button click
    ↓
MessageController.method()
    ↓
Model.update() / Network.send()
```

---

## ✅ Design Principles

1. **Separation of Concerns** - Each layer has its own responsibility
2. **Dependency Injection** - Controller receives Model from main.cpp
3. **Single Responsibility** - Model handles data, Controller handles logic, View handles UI
4. **Qt/QML Binding** - Auto-updates when data changes

---

**Benefits of this architecture:**
- ✅ Easy to maintain (logic changes don't affect UI)
- ✅ Easy to test (test each layer independently)
- ✅ Easy to extend (add new features)
