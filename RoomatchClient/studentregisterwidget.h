#ifndef STUDENTREGISTERWIDGET_H
#define STUDENTREGISTERWIDGET_H

#include "tools.h"
#include <QWidget>

namespace Ui { class StudentRegisterWidget; }

class StudentRegisterWidget : public QWidget {
    Q_OBJECT

public:
    explicit StudentRegisterWidget(QWidget *parent = nullptr);
    ~StudentRegisterWidget();

signals:
    void toLoginPage();

public slots:
    // 接收 UDP 广播消息
    void onTeacherDiscovered(const QString &name, const QString &ip, quint16 port);
    void onTeacherDismissed(const QString &name, const QString &ip, quint16 port);

private:
    Ui::StudentRegisterWidget *ui;

    // 重写
    void rewriteForm();
    // 隐藏提示
    void hideTip();
};

#endif // STUDENTREGISTERWIDGET_H
