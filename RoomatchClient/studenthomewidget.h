#ifndef STUDENTHOMEWIDGET_H
#define STUDENTHOMEWIDGET_H

#include <QWidget>

namespace Ui { class StudentHomeWidget; }

class StudentHomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit StudentHomeWidget(QWidget *parent = nullptr);
    ~StudentHomeWidget();

signals:
    void toLoginPage();

private:
    Ui::StudentHomeWidget *ui;

    void initConnection();
};

#endif // STUDENTHOMEWIDGET_H
