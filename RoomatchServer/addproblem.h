#ifndef ADDPROBLEM_H
#define ADDPROBLEM_H

#include "samples.h"
#include <QComboBox>
#include <QDialog>
#include <QWebEngineView>

namespace Ui { class AddProblem; }

class AddProblem : public QDialog {
    Q_OBJECT

public:
    explicit AddProblem(QWidget *parent = nullptr);
    ~AddProblem();
    void appendToTagBox(QString name, int id);
    void clearTagBox();

signals:
    void typeRefresh();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::AddProblem *ui;
    QWebEngineView *m_webView;

    QList<Samples*> samplesList;        // 记录所有的样例组

private slots:
    void onAddSample();                 // 添加样例
    void onClearSamples();              // 清空所有样例
    void onRemoveSample(Samples *s);    // 删除单个样例
    void updateLivePreview();           // 更新右侧 WebEngine
};

#endif // ADDPROBLEM_H
