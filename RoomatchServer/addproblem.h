#ifndef ADDPROBLEM_H
#define ADDPROBLEM_H

#include <QDialog>
#include <QWebEngineView>

namespace Ui { class AddProblem; }

class AddProblem : public QDialog {
    Q_OBJECT

public:
    explicit AddProblem(QWidget *parent = nullptr);
    ~AddProblem();

private:
    Ui::AddProblem *ui;
    QWebEngineView *m_webView;
};

#endif // ADDPROBLEM_H
