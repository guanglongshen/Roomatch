#include "loginwidget.h"
#include "networkmanager.h"
#include "ui_loginwidget.h"

#include <QMessageBox>
#include <tools.h>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget) {
    ui->setupUi(this);

    // 点击注册按钮，跳转到注册页面
    connect(ui->registerBtn, &QPushButton::clicked, this, [this](){
        emit toRegisterPage();
    });

    // 点击登录按钮
    connect(ui->loginBtn, &QPushButton::clicked, this, [this](){
        if (ui->onlineTeacherComboBox->currentIndex() == -1) {
            QMessageBox::information(this, tr("登录提示"), tr("<span style='color: #e74c3c; font-weight: bold;'>❌ 未选择在线教师，请重新检查！</span>"));
            return ;
        }

        QString username = ui->usernameEdit->text();
        if (username.isEmpty()) {
            QMessageBox::information(this, tr("登录提示"), tr("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的昵称长度至少 1 位，请重新检查！</span>"));
            return ;
        }

        QString pwd = ui->pwdEdit->text();
        if (pwd.length() <= 5) {
            QMessageBox::information(this, tr("登录提示"), tr("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的密码长度至少 6 位，请重新检查！</span>"));
            return ;
        }

        QString uniqueKey = ui->onlineTeacherComboBox->currentData().toString();
        QStringList target = uniqueKey.split(":");
        if (target.size() < 2) return ;

        QString teacherIP = target.at(0);
        quint16 teacherPort = target.at(1).toUShort();

        USERINFO info;
        info.type = 1;
        info.username = username;
        info.pwd = pwd;

        qDebug() << "申请登录";
        qDebug() << "地址：" << teacherIP << ":" << teacherPort;
        ui->loginBtn->setEnabled(false);
        emit NetworkManager::instance()->loginRequest(teacherIP, teacherPort, info);
    });

    connect(NetworkManager::instance(), &NetworkManager::loginReply, this, [this](const NET_STATUS &status){
        ui->loginBtn->setEnabled(true);

        if (status.code == 0) {
            // 登录成功，跳转主页面
            emit toHomePage();
        } else {
            QMessageBox::warning(this, tr("登录失败"), status.msg);
        }
    });
}

LoginWidget::~LoginWidget() {
    delete ui;
}

void LoginWidget::onTeacherDiscovered(const QString &name, const QString &ip, quint16 port) {
    QString displayText = QString("%1(%2)").arg(name, ip);
    QString uniqueKey = QString("%1:%2").arg(ip).arg(port);

    int index = ui->onlineTeacherComboBox->findText(displayText);
    if (index == -1) {
        ui->onlineTeacherComboBox->addItem(displayText, uniqueKey);
    }

    // UI 提醒
    if (ui->onlineTeacherComboBox->count() != 0) {
        ui->onlineTeacherComboBox->setPlaceholderText(tr("下拉选择教师"));
    }
}

void LoginWidget::onTeacherDismissed(const QString &name, const QString &ip, quint16 port) {
    QString displayText = QString("%1(%2)").arg(name, ip);
    QString uniqueKey = QString("%1:%2").arg(ip).arg(port);

    // 得到离线教师的那条消息
    int index = ui->onlineTeacherComboBox->findText(displayText);
    if (index != -1) {
        ui->onlineTeacherComboBox->removeItem(index);
    }

    if (ui->onlineTeacherComboBox->count() == 0) {
        ui->onlineTeacherComboBox->setPlaceholderText(tr("暂无在线教师"));
    }
}
