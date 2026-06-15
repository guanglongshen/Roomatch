#ifndef STUDENTHOMEWIDGET_H
#define STUDENTHOMEWIDGET_H

#include <QWidget>

namespace Ui { class StudentHomeWidget; }

class StudentHomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit StudentHomeWidget(QWidget *parent = nullptr);
    ~StudentHomeWidget();

private:
    Ui::StudentHomeWidget *ui;
};

#endif // STUDENTHOMEWIDGET_H
