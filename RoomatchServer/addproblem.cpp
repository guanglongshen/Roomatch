#include "addproblem.h"
#include "ui_addproblem.h"

AddProblem::AddProblem(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddProblem) {
    ui->setupUi(this);
    m_webView = new QWebEngineView(this);
    ui->viewLayout->addWidget(m_webView);

    m_webView->setUrl(QUrl("qrc:/html/preview.html"));

    this->resize(1200,750);
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
