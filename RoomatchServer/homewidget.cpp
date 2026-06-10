#include "homewidget.h"
#include "ui_homewidget.h"

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget) {
    ui->setupUi(this);
    // 视觉效果
    ui->contestTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents); // 自动适配内容大小
    ui->onlineTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 退出登录按键
    connect(ui->quitBtn, &QPushButton::clicked, this, &HomeWidget::quitLogin);
}

HomeWidget::~HomeWidget() {
    delete ui;
}

void HomeWidget::setTeacher(const QString &name) {
    teacher = name;
    ui->teacherLabel->setText(teacher);
}

