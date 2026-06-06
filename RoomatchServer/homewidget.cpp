#include "homewidget.h"
#include "ui_homewidget.h"

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget) {
    ui->setupUi(this);
    // 视觉效果
    ui->contestTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents); // 自动适配内容大小
    ui->onlineTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

HomeWidget::~HomeWidget() {
    delete ui;
}
