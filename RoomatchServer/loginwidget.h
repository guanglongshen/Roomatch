#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui { class LoginWidget; }

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

    // 获取用户名昵称
    QString getUsername() const;
    // 获取明文密码
    QString getPassword() const;

    void setLoginEnableBtn(bool t);

signals:
    void toRegisterPage();

private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
