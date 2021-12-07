#ifndef RESULTSDOCK_H
#define RESULTSDOCK_H

#include <QDockWidget>

#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QtCharts>

class ResultsDock : public QDockWidget {
    Q_OBJECT
    
public:
    ResultsDock(QWidget *parent = nullptr);
    ~ResultsDock();
    
private:
    QBarSeries *_probsSeries;
    QBarSeries *_measureSeries;
    QChart *_probsChart;
    QChart *_measureChart;
    QChartView *_probsView;
    QChartView *_measureView;
    
    QBarSet *_probSet;
    QBarSet *_measureSet;
};

#endif // RESULTSDOCK_H
