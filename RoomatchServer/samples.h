#ifndef SAMPLES_H
#define SAMPLES_H

#include <QWidget>

namespace Ui { class Samples; }

class Samples : public QWidget {
    Q_OBJECT

public:
    explicit Samples(QWidget *parent = nullptr);
    ~Samples();

    QString getInput();
    QString getOutput();
    void setIndex(int index);

signals:
    void dataChanged();
    void removeRequested(Samples *m);

private:
    Ui::Samples *ui;
};

#endif // SAMPLES_H
