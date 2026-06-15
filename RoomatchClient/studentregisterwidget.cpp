#include "networkmanager.h"
#include "studentregisterwidget.h"
#include "ui_studentregisterwidget.h"

#include <QMessageBox>
#include <tools.h>

StudentRegisterWidget::StudentRegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentRegisterWidget) {
    ui->setupUi(this);

    // 提示语隐藏并连接相关信号
    hideTip();
    rewriteForm();

    // 点击返回登录按钮
    connect(ui->backBtn, &QPushButton::clicked, this, [this](){
        emit toLoginPage();
    });

    // 清空输入按钮
    connect(ui->clearBtn, &QPushButton::clicked, this, [this](){
        ui->usernameEdit->clear();
        ui->pwdEdit->clear();
        ui->pwdConfirmEdit->clear();
    });

    // 点击注册按钮
    connect(ui->registerBtn, &QPushButton::clicked, this, [this](){
        if (ui->onlineTeacherComboBox->currentIndex() == -1) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 教师未选择，请重新选择！</span>");
            ui->tipLabel->show();
            return ;
        }

        QString username = ui->usernameEdit->text();
        if (username.isEmpty()) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 昵称未填写，请输入您的真实姓名！</span>");
            ui->tipLabel->show();
            return ;
        }

        QString pwd = ui->pwdEdit->text();
        QString confirm = ui->pwdConfirmEdit->text();
        if (pwd.length() <= 5 || confirm.length() <= 5) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的密码长度至少 6 位，请重新检查！</span>");
            ui->tipLabel->show();
            return ;
        }
        if (pwd != confirm) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 两次输入的密码不一致，请重新检查！</span>");
            ui->tipLabel->show();
            return ;
        }

        ui->tipLabel->setText("<span style='color: #32cd32; font-weight: bold;'>🟢 正在提交注册信息，请稍后...</span>");
        ui->tipLabel->show();
        ui->registerBtn->setEnabled(false);

        // 获取注册时的教师 IP 地址（IP:port 这个之前读取 教师UDP 存储的时候就放入了）
        QString uniqueKey = ui->onlineTeacherComboBox->currentData().toString();
        QStringList target = uniqueKey.split(":");
        if (target.size() < 2) return ;

        QString teacherIP = target.at(0);
        quint16 teacherPort = target.at(1).toUShort();

        USERINFO info;
        info.username = username;
        info.pwd = pwd;
        info.type = 1;
        // qDebug() << "申请注册 connect(ui->registerBtn, &QPushButton::clicked, this, [this](){}";
        // qDebug() << "地址：" << teacherIP << ":" << teacherPort;
        emit NetworkManager::instance()->registerRequest(teacherIP, teacherPort, info);
    });

    // 与 NetworkManager 的 registerReply 信号连接
    connect(NetworkManager::instance(), &NetworkManager::registerReply, this, [this](const NET_STATUS &status){
        // 拿到了服务器端的注册回复
        ui->registerBtn->setEnabled(true);

        if (status.code == 0) {
            // 注册成功，跳转登录页面
            // 成功注册
            QMessageBox::information(this, tr("注册成功"), tr("用户注册成功，即将前往登录页面！"));
            emit ui->clearBtn->clicked(true);       // 成功查询，则清空输入框
            ui->onlineTeacherComboBox->setCurrentIndex(-1);

            emit toLoginPage();
        } else {
            QMessageBox::warning(this, tr("注册失败"), status.msg);
            ui->tipLabel->setText(ui->tipLabel->text() + QString("<br/><span style='color: #e74c3c; font-weight: bold;'>%1</span>").arg(status.msg));
        }
    });
}

StudentRegisterWidget::~StudentRegisterWidget() {
    delete ui;
}

void StudentRegisterWidget::onTeacherDiscovered(const QString &name, const QString &ip, quint16 port) {
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

void StudentRegisterWidget::onTeacherDismissed(const QString &name, const QString &ip, quint16 port) {
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

void StudentRegisterWidget::hideTip() {
    ui->tipLabel->hide();
}

void StudentRegisterWidget::rewriteForm() {
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &StudentRegisterWidget::hideTip);
    connect(ui->pwdEdit, &QLineEdit::textChanged, this, &StudentRegisterWidget::hideTip);
    connect(ui->pwdConfirmEdit, &QLineEdit::textChanged, this, &StudentRegisterWidget::hideTip);
    connect(ui->onlineTeacherComboBox, &QComboBox::currentIndexChanged, this, &StudentRegisterWidget::hideTip);
}
