#include "samples.h"
#include "ui_samples.h"

Samples::Samples(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Samples) {
    ui->setupUi(this);

    QFont monospaceFont("Consolas");
    monospaceFont.setStyleHint(QFont::Monospace); // 如果没装 Consolas 就用系统默认等宽
    ui->inputData->setFont(monospaceFont);
    ui->outputData->setFont(monospaceFont);

    connect(ui->inputData, &QPlainTextEdit::textChanged, this, &Samples::dataChanged);
    connect(ui->outputData, &QPlainTextEdit::textChanged, this, &Samples::dataChanged);

    connect(ui->deleteBtn, &QPushButton::clicked, this, [this](){
        emit removeRequested(this);
    });
}

Samples::~Samples() {
    delete ui;
}

QString Samples::getInput() {
    return ui->inputData->toPlainText();
}

QString Samples::getOutput() {
    return ui->outputData->toPlainText();
}

void Samples::setIndex(int index) {
    ui->caseID->setText(tr("样例 #%1").arg(index, 2, 10, QChar('0')));
}
