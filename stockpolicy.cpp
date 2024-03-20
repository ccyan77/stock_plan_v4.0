#include "stockpolicy.h"
#include "ui_stockpolicy.h"





StockPolicy::StockPolicy(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StockPolicy)
{
    ui->setupUi(this);
    initUi();
}

StockPolicy::~StockPolicy()
{
    delete ui;
}

void StockPolicy::initUi()
{

    m_chart = new QChart;
    m_chart->setTitle("趋势查看");
    m_chart->setBackgroundBrush(QColor(255,255,255));  // 用于设置背景色的   QColor(10,10,10)  r,g,b 红绿蓝3原色
    QLineSeries *series = new QLineSeries; // 用于存放数据的系列，相当于存了XY对应数据的容器

    series->append(1686240000000,17.63); // X Y的值
    series->append(1686844800000,19.31);
    series->append(1687276800000,30.11);
    series->append(1688054400000,19.31);
    series->append(1688659200000,50.81);
    m_chart->addSeries(series);

    m_dtAx = new QDateTimeAxis;  // X轴，时间轴
    m_dtAx->setFormat("yyyy-MM-dd"); // 设置时间格式
    m_dtAx->setTitleText("时间");
    m_dtAx->setTickCount(10); // 设置网格数
    m_chart->setAxisX(m_dtAx,series);  // 设置 X轴为时间轴、数据

    m_axisY = new QValueAxis;  // Y轴
    m_axisY->setTickCount(10); // 设置格数
    m_axisY->setRange(0,300); // 设置Y值的范围
    m_chart->setAxisY(m_axisY,series);  // 设置 Y轴为数据轴、数据

    m_chartView = new QChartView(m_chart);
    // 框选放大，右键缩小
    m_chartView->setRubberBand(QChartView::HorizontalRubberBand); // 设置鼠标左击框选放大，右击缩小
    m_chartView->setRenderHint(QPainter::Antialiasing,true);
    this->setCentralWidget(m_chartView); // 将表视图放到中心部件来显示，可以做到自适应大小
}

void StockPolicy::showNewStockData(QLineSeries *series)
{

    m_chart->addSeries(series);  // 添加数据到图表
    m_chart->setAxisX(m_dtAx,series);  // 设置 X轴为时间轴、数据
    m_chart->setAxisY(m_axisY,series);  // 设置 Y轴为数据轴、数据
}

void StockPolicy::showNewStockData(QSplineSeries *series)
{
    m_chart->addSeries(series);  // 添加数据到图表
    m_chart->setAxisX(m_dtAx,series);  // 设置 X轴为时间轴、数据
    m_chart->setAxisY(m_axisY,series);  // 设置 Y轴为数据轴、数据
}

