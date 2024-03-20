#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"


ClientMainWindow::ClientMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);
    // 启动界面初始化的函数
    initUi();
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

void ClientMainWindow::initUi()
{
    ui->tabWidget->clear(); // 清除无用的前2个tab页面

    // 实例化获取股票列表的界面对象
    m_getStockList = new GetStockList(this);
    // 将股票界面对象放到主界面的tab选项卡里面来
    ui->tabWidget->addTab(m_getStockList,"获取信息");

    m_stockPolicy = new StockPolicy(this);
    ui->tabWidget->addTab(m_stockPolicy,"股票策略");

    // 关联一下2个界面的信号和槽，用来传递图表所需的数据及展示
    connect(m_getStockList,SIGNAL(sendChartData(QLineSeries*)),m_stockPolicy,SLOT(showNewStockData(QLineSeries*)));
    connect(m_getStockList,SIGNAL(sendChartData(QSplineSeries*)),m_stockPolicy,SLOT(showNewStockData(QSplineSeries*)));

}
