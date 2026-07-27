#include "addproblem.h"
#include "ui_addproblem.h"
#include "tools.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonObject>
#include <QSqlQuery>

AddProblem::AddProblem(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddProblem) {
    ui->setupUi(this);
    m_webView = new QWebEngineView(this);
    ui->viewLayout->addWidget(m_webView);

    m_webView->setUrl(QUrl("qrc:/html/preview.html"));

    this->resize(1650,750);

    // 连接所有改动后更新 QWebEnigneView
    connect(ui->problemNameEdit, &QLineEdit::textChanged, this, &AddProblem::updateLivePreview);
    connect(ui->describeEdit, &QPlainTextEdit::textChanged, this, &AddProblem::updateLivePreview);
    connect(ui->difficulty, &QComboBox::currentIndexChanged, this, &AddProblem::updateLivePreview);
    connect(ui->times, &QSpinBox::valueChanged, this, &AddProblem::updateLivePreview);
    connect(ui->spaces, &QSpinBox::valueChanged, this, &AddProblem::updateLivePreview);
    connect(ui->tagBox, &QComboBox::currentIndexChanged, this, &AddProblem::updateLivePreview);
    connect(ui->inputEdit, &QPlainTextEdit::textChanged, this, &AddProblem::updateLivePreview);
    connect(ui->outputEdit, &QPlainTextEdit::textChanged, this, &AddProblem::updateLivePreview);
    connect(ui->hintEdit, &QPlainTextEdit::textChanged, this, &AddProblem::updateLivePreview);

    connect(ui->addSampleBtn, &QPushButton::clicked, this, &AddProblem::onAddSample);
    connect(ui->clearAllSamplesBtn, &QPushButton::clicked, this, &AddProblem::onClearSamples);

    // 网页加载完成后再刷新一次
    connect(m_webView, &QWebEngineView::loadFinished, this, &AddProblem::updateLivePreview);

    // 点击清空编辑的按钮后，所有文本都删除
    connect(ui->clearBtn, &QPushButton::clicked, this, [this](){
        ui->problemNameEdit->clear();
        ui->difficulty->setCurrentIndex(0);
        ui->times->setValue(1000);
        ui->spaces->setValue(512);
        ui->tagBox->setCurrentIndex(-1);
        ui->describeEdit->clear();
        ui->inputEdit->clear();
        ui->outputEdit->clear();
        ui->hintEdit->clear();

        ui->clearAllSamplesBtn->click();
    });

    // 点击测试点配置按钮，预先保存测试点的磁盘路径
    connect(ui->casePathChoose, &QPushButton::clicked, this, [this](){
        QString lastFolderPath = AppConfig::settings().value("last_folder_path", QDir::homePath()).toString();

        // 打开文件对话框
        QString dir = QFileDialog::getExistingDirectory(
            this,
            "选择文件夹",
            lastFolderPath,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

        if (!dir.isEmpty()) {
            ui->pathLabel->setText(dir);
            AppConfig::settings().setValue("last_folder_path", dir);
        } else {
            ui->pathLabel->setText("Empty...");
            AppConfig::settings().setValue("last_folder_path", "");
        }
    });

    // 点击保存按钮后，将这个题目存入数据库
}

AddProblem::~AddProblem() {
    if (m_webView) {
        m_webView->setUrl(QUrl("about:blank"));
        m_webView->setParent(nullptr);
        delete m_webView;
        m_webView = nullptr;
    }

    delete ui;
}

void AddProblem::appendToTagBox(QString name, int id) {
    ui->tagBox->addItem(name, id);
}

void AddProblem::clearTagBox() {
    ui->tagBox->clear();
}

void AddProblem::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    emit typeRefresh();
}

void AddProblem::onAddSample() {
    Samples *newSample = new Samples(this);
    samplesList.append(newSample);

    newSample->setIndex(samplesList.count());
    ui->samplesLayout->addWidget(newSample);

    // 绑定对应信号
    connect(newSample, &Samples::dataChanged, this, &AddProblem::updateLivePreview);
    connect(newSample, &Samples::removeRequested, this, &AddProblem::onRemoveSample);

    // 添加样例组后，更新网页
    updateLivePreview();
}

// 删除所有样例组
void AddProblem::onClearSamples() {
    for (Samples *s : samplesList) {
        ui->samplesLayout->removeWidget(s);
        s->deleteLater();
    }
    samplesList.clear();
    updateLivePreview();
}

// 删除单个样例组
void AddProblem::onRemoveSample(Samples *s) {
    if (!s) return ;

    samplesList.removeOne(s);
    ui->samplesLayout->removeWidget(s);
    s->deleteLater();

    // 重新设置所有样例组的编号
    for (int i = 0; i < samplesList.size(); i++) {
        samplesList[i]->setIndex(i + 1);
    }

    // 更新
    updateLivePreview();
}

void AddProblem::updateLivePreview() {
    if (!m_webView || !m_webView->page()) return;

    // 各部分文本
    QString title = ui->problemNameEdit->text();
    QString difficulty = QString::number(ui->difficulty->currentIndex());
    QString time_limit = QString::number(ui->times->value());
    QString memory_limit = QString::number(ui->spaces->value());
    QString tag = (ui->tagBox->currentIndex() == -1 ? "NONE" : ui->tagBox->currentText());
    QString describe = ui->describeEdit->toPlainText().toHtmlEscaped().replace("\n", "<br>");
    QString inputFormat = ui->inputEdit->toPlainText().toHtmlEscaped().replace("\n", "<br>");
    QString outputFormat = ui->outputEdit->toPlainText().toHtmlEscaped().replace("\n", "<br>");
    QString hint = ui->hintEdit->toPlainText().toHtmlEscaped().replace("\n", "<br>");

    // 动态样例组
    QString samplesHtml;
    for (int i = 0; i < samplesList.size(); i++) {
        QString sInput = samplesList[i]->getInput();
        QString sOutput = samplesList[i]->getOutput();

        QString indexStr = QString("%1").arg(i + 1, 2, 10, QChar('0'));

        samplesHtml += QString(
           "<table class=\"sample-table\">"
           "  <thead>"
           "    <tr>"
           "      <th>Input #%1 <button class=\"copy-btn\" onclick=\"copyText('in_%1', this)\">Copy</button></th>"
           "      <th>Output #%1 <button class=\"copy-btn\" onclick=\"copyText('out_%1', this)\">Copy</button></th>"
           "    </tr>"
           "  </thead>"
           "  <tbody>"
           "    <tr>"
           "      <td><pre id=\"in_%1\">%2</pre></td>"
           "      <td><pre id=\"out_%1\">%3</pre></td>"
           "    </tr>"
           "  </tbody>"
           "</table>"
        ).arg(indexStr).arg(sInput.toHtmlEscaped()).arg(sOutput.toHtmlEscaped());
    }

    QJsonObject obj;
    obj["title"] = title;
    obj["difficulty"] = difficulty;
    obj["times"] = time_limit;
    obj["memory"] = memory_limit;
    obj["tag"] = tag;
    obj["describe"] = describe;
    obj["input"] = inputFormat;
    obj["output"] = outputFormat;
    obj["samples"] = samplesHtml;
    obj["hint"] = hint;

    // 转为 JSON 字符
    QString jsonStr = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    // 执行脚本
    QString jsCode = QString(
        "(() => {"
        "    const data = %1;"
        "    updatePreview(data.title, data.difficulty, data.times, data.memory, data.tag, data.describe, data.input, data.output, data.samples, data.hint);"
        "})();"
    ).arg(jsonStr);

    m_webView->page()->runJavaScript(jsCode);
}
