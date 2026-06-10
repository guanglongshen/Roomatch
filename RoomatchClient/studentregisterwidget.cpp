#include "studentregisterwidget.h"
#include "ui_studentregisterwidget.h"

StudentRegisterWidget::StudentRegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentRegisterWidget) {
    ui->setupUi(this);

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
