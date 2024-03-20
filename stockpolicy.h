#ifndef STOCKPOLICY_H
#define STOCKPOLICY_H

#include <QMainWindow>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChart>
#include <QGraphicsSimpleTextItem>
#include <QLineSeries>  // 折线
#include <QSplineSeries> // 曲线

QT_CHARTS_USE_NAMESPACE  // 命名空间得使用起来，要不然类会拿不到

namespace Ui {
class StockPolicy;
}

class StockPolicy : public QMainWindow
{
    Q_OBJECT

public:
    explicit StockPolicy(QWidget *parent = 0);
    ~StockPolicy();
    void initUi();
public slots:
    void showNewStockData(QLineSeries *series); // 用于刷新折线表格数据的
    void showNewStockData(QSplineSeries *series); // 用于刷新曲线表格数据的
private:
    Ui::StockPolicy *ui;

    QChart *m_chart;
    QDateTimeAxis *m_dtAx;
    QValueAxis *m_axisY;
    QChartView *m_chartView;

};

#endif // STOCKPOLICY_H
