#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class NetworkWorker : public QObject {
    Q_OBJECT
public:
    explicit NetworkWorker(QObject *parent = nullptr);
    ~NetworkWorker();

signals:
    // 网络动态
    void logMessage(const QString &event, const QString &status, const QString &source);

public slots:
    // 开启网络核心服务
    void onStartNetworkService();

private slots:
    // 定时器：若干秒喊话
    void onSendBroadcast();

private:
    QUdpSocket *udpSocket = nullptr;
    QTimer     *broadcastTimer = nullptr;
    quint16     tcpPort = 55520;            // TCP 监听端口
};

#endif // NETWORKWORKER_H
