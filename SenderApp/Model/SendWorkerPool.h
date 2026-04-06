#ifndef SENDWORKERPOOL_H
#define SENDWORKERPOOL_H

#include <QObject>
#include <QThreadPool>
#include <QFuture>
#include <QMap>
#include <QMutex>
#include "Message.h"
#include "MulticastSender.h"

/**
 * @class SendWorkerPool
 * @brief Quản lý send operations trên multiple threads mà không block UI
 *
 * WHY này là thiết kế tối ưu:
 * 1. QThreadPool: Automatic thread management - không cần tạo/xóa threads thủ công
 * 2. maxThreadCount: Giới hạn số threads tránh tạo quá nhiều threads → crash
 * 3. QtConcurrent: Lightweight, dễ dùng, built-in thread pool support
 * 4. QMutex: Thread-safe khi access m_activeSends từ nhiều threads
 * 5. QFuture: Track send operations, có thể cancel/wait
 */
class SendWorkerPool : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     * @param maxThreads: Số thread tối đa chạy cùng lúc (VD: 4)
     *                    - Giới hạn này tránh quá tải system
     *                    - Quy luật: maxThreads = CPU cores hoặc CPU cores + 2
     * @param parent: Owner object
     */
    explicit SendWorkerPool(int maxThreads = 4, QObject* parent = nullptr);

    ~SendWorkerPool();

    /**
     * @brief Gửi một message trên worker thread
     * @param msgId: Message ID để tracking
     * @param msg: Message object chứa dữ liệu
     * @param sender: MulticastSender instance để gửi UDP
     *
     * WHY: Separated msgId vì QFuture không biết msgId, cần store nó
     */
    void sendMessage( Message* msg, MulticastSender* sender);

    /**
     * @brief Gửi nhiều messages song song
     * @param msgIds: Danh sách message IDs
     * @param msgs: Danh sách messages
     * @param sender: MulticastSender instance
     *
     * WHY: Group send - gửi nhiều messages đồng thời
     *      Mỗi message chạy trên thread pool riêng
     */
    void sendGroupMessages(const QList<Message*>& msgs,
                           MulticastSender* sender, const int &intervalGroupMs);


    void sendGroupMessageFollowOder(const QList<Message *> &messages,MulticastSender* sender);


    /**
     * @brief Dừng một send operation đang chạy
     * @param msgId: Message ID cần dừng
     *
     * WHY: Cancel pattern - có thể hủy send được
     *      QFuture hỗ trợ cancel(), QMutex tránh race condition
     */
    void cancelSend(int msgId);

    /**
     * @brief Dừng tất cả send operations
     *
     * WHY: Cleanup khi close app hoặc stop sending
     */
    void cancelAll();

    /**
     * @brief Kiểm tra send operation còn đang chạy không
     * @param msgId: Message ID
     * @return true nếu đang gửi
     *
     * WHY: UI cần biết status để enable/disable buttons
     */
    bool isRunning(int msgId) const;

    /**
     * @brief Lấy số send operations đang chạy
     */
    int activeCount() const;

    /**
     * @brief Set max threads (có thể đổi động)
     */
    void setMaxThreads(int maxThreads);

signals:
    /**
     * @brief Emit khi send hoàn thành
     */
    void sendFinished(int msgId, int bytesReceived = 0);

    /**
     * @brief Emit khi có lỗi trong quá trình gửi
     */
    void sendError(int msgId, const QString& error);

    /**
     * @brief Emit progress của send operation
     */
    void progress(int msgId, int current, int total);

private:
    QThreadPool* m_threadPool;
    QMap<int, QFuture<void>> m_activeSends;
    mutable QMutex m_mutex;
    int m_maxThreads;
};

#endif // SENDWORKERPOOL_H