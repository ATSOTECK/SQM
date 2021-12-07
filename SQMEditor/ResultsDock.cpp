#include "ResultsDock.h"

#include <QGroupBox>
#include <QVBoxLayout>

ResultsDock::ResultsDock(QWidget *parent) :
    QDockWidget(parent),
    _probsSeries(new QBarSeries()),
    _measureSeries(new QBarSeries()),
    _probsChart(new QChart()),
    _measureChart(new QChart()),
    _probsView(new QChartView(this)),
    _measureView(new QChartView(this)),
    _probSet(new QBarSet("State")),
    _measureSet(new QBarSet("Result"))
{
    setWindowTitle("RESULTS");
    
    *_probSet << 25 << 25 << 25 << 25 << 0 << 0 << 0 << 0;
    
    _probsSeries->setName("Probabilities");
    _probsSeries->append(_probSet);
    
    _probsChart->setTheme(QChart::ChartThemeDark);
    _probsChart->addSeries(_probsSeries);
    _probsChart->setTitle("PROBABILITY");
    
    QStringList categories;
    categories << "000" << "001" << "010" << "011" << "100" << "101" << "110" << "111";
    QBarCategoryAxis *axisXProb = new QBarCategoryAxis();
    axisXProb->append(categories);
    axisXProb->setTitleText("Computational basis states");
    _probsChart->addAxis(axisXProb, Qt::AlignBottom);
    _probsSeries->attachAxis(axisXProb);
    
    QValueAxis *axisYProb = new QValueAxis();
    axisYProb->setRange(0, 100);
    axisYProb->setTitleText("Probability (%)");
    _probsChart->addAxis(axisYProb, Qt::AlignLeft);
    _probsSeries->attachAxis(axisYProb);
    _probsChart->legend()->setVisible(true);
    _probsChart->legend()->setAlignment(Qt::AlignBottom);
    
    _probsView->setRenderHint(QPainter::Antialiasing, true);
    _probsView->setChart(_probsChart);
    
    /*-------------------------------------------------------------------------------------*/
    
    *_measureSet << 0 << 100 << 0 << 0 << 0 << 0 << 0 << 0;
    
    _measureSeries->setName("Measurement");
    _measureSeries->append(_measureSet);
    
    _measureChart->setTheme(QChart::ChartThemeDark);
    _measureChart->addSeries(_measureSeries);
    _measureChart->setTitle("MEASUREMENT");
    
    QBarCategoryAxis *axisXMeasure = new QBarCategoryAxis();
    axisXMeasure->append(categories);
    axisXMeasure->setTitleText("Computational basis states");
    _measureChart->addAxis(axisXMeasure, Qt::AlignBottom);
    _measureSeries->attachAxis(axisXMeasure);
    
    QValueAxis *axisYMeasure = new QValueAxis();
    axisYMeasure->setRange(0, 100);
    axisYMeasure->setTitleText("Probability (%)");
    _measureChart->addAxis(axisYMeasure, Qt::AlignLeft);
    _measureSeries->attachAxis(axisYMeasure);
    
    _measureView->setRenderHint(QPainter::Antialiasing, true);
    _measureView->setChart(_measureChart);
    
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(_probsView);
    layout->addWidget(_measureView);
    
    QGroupBox *box = new QGroupBox(this);
    box->setLayout(layout);
    
    setWidget(box);
}

ResultsDock::~ResultsDock() {
    delete _probsSeries;
    delete _measureSeries;
    delete _probsChart;
    delete _measureChart;
}
