#include "getstocklist.h"
#include "ui_getstocklist.h"
#include <QDebug>
#include <QFile>
#include <QNetworkCookieJar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLibrary> // 用于加载dll动态库的类

GetStockList::GetStockList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GetStockList)
{
    ui->setupUi(this);
    ui->pushButton_get->hide();
    ui->statusbar->hide();

    // 方案2:
    // 如果https 访问的时候，还是爬取失败，可以代码加载动态库来支持
    // 前提条件是把2个dll动态库拷贝的生成目录中:libeay32.dll 和 ssleay32.dll
    // 拷贝到生成目录，就可以直接加载
    QLibrary lb("libeay32.dll");
    if(lb.load()){
        qDebug()<<"load libeay32.dll 成功";
    }

    QLibrary lb2("ssleay32.dll");
    if(lb2.load()){
        qDebug()<<"load ssleay32.dll 成功";
    }

    // 模型及表格视图
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(10); // 设置10列
    QStringList labels;
    labels<<"股票名字"<<"股票代号"<<"总市值(亿)"<<"流通市值(亿)"<<"涨跌幅(%)"<<"收盘价"<<"成交额(亿)"<<"成交量(手)"<<"换手率(%)"<<"市盈率";
    m_model->setHorizontalHeaderLabels(labels); // 设置模型的列标签名
    ui->tableView->setModel(m_model); // 将模型和视图关联
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 设置内容自适应，拉伸

    // 给表格视图添加右击自定义菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu); // 设置上下文菜单策略为自定义菜单
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showMenu()));
    m_menu = new QMenu(this);
    QAction *policy = m_menu->addAction("策略分析");
    QAction *price =  m_menu->addAction("最新价格");
    connect(policy,SIGNAL(triggered(bool)),this,SLOT(policySlot())); // 当点击菜单策略分析之后，就会出发槽 policySlot 函数
    connect(price,SIGNAL(triggered(bool)),this,SLOT(priceSlot()));

    // 处理数据爬取的操作
    // 模拟浏览器的参数
    QString userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
    // 设置要访问的网址
    m_request.setUrl(QUrl("https://xueqiu.com/"));
    // 设置请求头，用户代理，用来模拟浏览器
    m_request.setHeader(QNetworkRequest::UserAgentHeader,userAgent);
    // 关联信号槽，当请求管理对象完全打开网页之后，会将数据给槽函数处理
    connect(&m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(showAplyData(QNetworkReply*)));
    on_pushButton_get_clicked();
}

GetStockList::~GetStockList()
{
    delete ui;
}

/*
 * 要处理的大概格式如下
 *
 * {"data":{"count":5000,"list":[{"symbol":"SZ300817","net_profit_cagr":-17.50317062659299,"north_net_inflow":null,
 * "ps":4.4809,"type":11,"percent":20.03,"has_follow":false,"tick_size":0.01,"pb_ttm":3.2607,"float_shares":99366912,
 * "current":18.04,"amplitude":24.02,"pcf":21.3128,"current_year_percent":28.25,"float_market_capital":1.792579092E9,
 * "north_net_inflow_time":null,"market_capital":3.1504802E9,"dividend_yield":1.153,"lot_size":100,"roe_ttm":5.6761589850147,
 * "total_percent":78.15,"percent5m":0.0,"income_cagr":2.235799161377394,"amount":3.5973639118E8,"chg":3.01,
 * "issue_date_ts":1581955200000,"eps":0.32,"main_net_inflows":6078207.0,"volume":21355714,"volume_ratio":4.35,"pb":3.143,
 * "followers":6931,"turnover_rate":21.49,"first_percent":43.98,"name":"双飞股份","pe_ttm":56.123,"total_shares":174638592,
 * "limitup_days":1}]}}
 */

// 获取所有沪深股票的信息
void GetStockList::getStockAllCode(QByteArray &buffer)
{
    // 使用JSON进行数据处理
    QJsonDocument jd = QJsonDocument::fromJson(buffer);
    if(jd.isArray()){
        qDebug()<<"js array";

    }else if(jd.isObject()){
        qDebug()<<"js object";
        QJsonObject jobject = jd.object();
        QStringList jlist = jobject.keys();
        qDebug()<<"jlist:"<<jlist; // 输出一下一级键的情况
        // 通过键值对的方式提取数据
        int count = jobject["data"].toObject()["count"].toInt();
        qDebug()<<"count:"<<count;
        QJsonArray listData = jobject["data"].toObject()["list"].toArray();
        for(int i=0;i<count;++i){
            QString symbol = listData.at(i).toObject().value("symbol").toString(); // 股票代号
            QString name = listData.at(i).toObject().value("name").toString();  // 股票名字
            double market_capital = listData.at(i).toObject().value("market_capital").toDouble()/100000000; // 总市值 单位 亿
            double float_market_capital = listData.at(i).toObject().value("float_market_capital").toDouble()/100000000; // 流通市值 单位 亿
            double percent = listData.at(i).toObject().value("percent").toDouble(); // 涨跌幅 百分比，大于0上涨，小于0下跌
            double current = listData.at(i).toObject().value("current").toDouble(); // 收盘价
            double amount = listData.at(i).toObject().value("amount").toDouble()/100000000; // 成交额 单位 亿
            double volume = listData.at(i).toObject().value("volume").toInt(); // 成交量 单位 手
            double turnover_rate = listData.at(i).toObject().value("turnover_rate").toDouble(); // 换手率 百分比
            double pe_ttm = listData.at(i).toObject().value("pe_ttm").toDouble(); // PE值 市盈率 百分比
            //qDebug()<<name<<symbol<<market_capital;
            // 添加模型行的数据:股票名字、股票代号 总市值 流通市值 涨跌幅 收盘价 成交额 成交量 换手率 市盈率
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem(name));
            rowItems.append(new QStandardItem(symbol));
            rowItems.append(new QStandardItem(QString::number(market_capital)));
            rowItems.append(new QStandardItem(QString::number(float_market_capital)));
            QStandardItem *percentItem =  new QStandardItem(QString::number(percent));
            if(percent > 0){
                percentItem->setData(QColor("red"),Qt::DecorationRole); // 添加一个装饰的颜色为红色
                percentItem->setData(QColor("red"),Qt::TextColorRole);  // 将字体颜色设置为红色
                rowItems.at(0)->setData(QColor("red"),Qt::TextColorRole);  // 将股票名字设置为红色
             }
            else if(percent < 0){
                percentItem->setData(QColor("green"),Qt::BackgroundColorRole);
                rowItems.at(0)->setData(QColor("green"),Qt::TextColorRole);
            }
            rowItems.append(percentItem);
            rowItems.append(new QStandardItem(QString::number(current)));
            rowItems.append(new QStandardItem(QString::number(amount)));
            rowItems.append(new QStandardItem(QString::number(volume)));
            rowItems.append(new QStandardItem(QString::number(turnover_rate)));
            if(pe_ttm > 0){
                rowItems.append(new QStandardItem(QString::number(pe_ttm)));
            }
            else{
                rowItems.append(new QStandardItem("亏损"));
            }
            // 添加一行数据项到模型中
            m_model->appendRow(rowItems);
        }
    }
}


/*  要处理的单个股票的格式如下
{"data":{"symbol":"SH601127","column":["timestamp","volume","open","high","low","close","chg","percent","turnoverrate","amount","volume_post","amount_post"],
"item":[[1680192000000,592214010,37.57,41.56,35.63,37.85,0.2,0.53,50.63,2.3075281252E10,0,0.0],
[1682611200000,355174985,36.6,36.6,28.61,29.52,-8.33,-22.01,27.0,1.1568002446E10,0,0.0],
[1685462400000,345031667,29.51,30.1,25.27,25.78,-3.74,-12.67,23.04,9.442101926E9,0,0.0],
[1688054400000,1217980960,25.6,42.19,24.75,36.77,10.99,42.63,81.24,4.2515733378E10,0,0.0],
[1688572800000,427585117,37.5,47.3,37.06,46.62,9.85,26.79,28.37,1.8368604411E10,0,0.0]]},"error_code":0,"error_description":""}

*/
// 单个股票的 JSON格式处理
void GetStockList::getStockOne(QByteArray &buffer)
{
    QJsonDocument jd = QJsonDocument::fromJson(buffer);
    if(jd.isObject()){
        QJsonObject jObject = jd.object();
        QJsonArray jArr = jObject.value("data").toObject().value("item").toArray(); // 通过键值对取值
        int cnt = jArr.count();
        qDebug()<<"数组size:"<<cnt;
        QString symbol = jObject.value("data").toObject().value("symbol").toString();
        QLineSeries *series = new QLineSeries;  // QLineSeries 折线的类，QSplineSeries 为圆滑曲线类
        series->setName(symbol);  // 设置折线名字
        //series->setColor(QColor(255,0,0)); // 设置颜色 r,g,b 红绿蓝三原色，随意组合
        QSplineSeries *sp = new QSplineSeries;
        sp->setName(symbol);
        for(int i=0;i<cnt;++i){
            qint64 timestamp = jArr.at(i).toArray().at(0).toVariant().toLongLong();
            double close = jArr.at(i).toArray().at(5).toVariant().toDouble();
            qDebug()<<timestamp<<close;
            series->append(timestamp,close); // 折线数据添加
            sp->append(timestamp,close);  // 曲线数据添加
        }
        if(m_menuIndex == 0){
            // 策略分析  只能选一种线形数据
            //emit sendChartData(series); // 发送折线数据
            emit sendChartData(sp);   // 发送曲线数据
        }else if(m_menuIndex == 1){
            // 最新价格
        }
    }
}

void GetStockList::on_pushButton_get_clicked()
{
    qDebug()<<"开始请求";
    m_manager.get(m_request);
    qDebug()<<"请求中...";
}

void GetStockList::showAplyData(QNetworkReply *reply)
{
    qDebug()<<"收到响应";
    // 将响应的数据，一把读取完，放到字节数组里面来处理
    QByteArray buffer = reply->readAll();

    // 将数据写到文件中，方便观察数据内容
    QFile file("data.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream out(&file);
    out << buffer << endl;
    file.close();

    // 下面使用JSON进行数据处理
    if(reply->url() == QUrl("https://xueqiu.com/")){
        qDebug()<<"发现首页url";

    }else if(reply->url().toString().indexOf("https://stock.xueqiu.com/v5/stock/chart/kline.json") != -1){
        qDebug()<<"查看个股情况:";
        getStockOne(buffer);

    }else if(reply->url().toString().indexOf("https://stock.xueqiu.com/v5/stock/screener/quote/list.json") != -1){
        qDebug()<<"获取所有沪深股票 可以干活了";
        getStockAllCode(buffer);
    }

    reply->deleteLater();
}

void GetStockList::on_pushButton_get_2_clicked()
{
    // 设置要爬取的核心网址
    m_request.setUrl(QUrl("https://stock.xueqiu.com/v5/stock/screener/quote/list.json?page=1&size=5000&order=desc&orderby=amount&order_by=amount&market=CN&type=sh_sz"));
    // 执行get请求，去拿网址对应的数据
    m_manager.get(m_request);
    qDebug()<<"req ok";
}

void GetStockList::showMenu()
{
    qDebug()<<"右击菜单";
    m_menu->popup(QCursor::pos());  // 在鼠标光标的位置弹出菜单
}

void GetStockList::policySlot()
{
    qDebug()<<"策略查看";
    if(m_model->rowCount() == 0)return; // 防止异常

    int row = ui->tableView->currentIndex().row(); // 取出菜单所在的行号
    QString symbol = m_model->item(row,1)->text(); // 取当前行所在的股票代号
    qint64 begin = QDateTime::currentMSecsSinceEpoch(); // 取当前时间戳
    m_numsCnt = 30; // 这里是设置获取单个股票的数据量
    QString urlStr = QString("https://stock.xueqiu.com/v5/stock/chart/kline.json?symbol=%1&begin=%2&period=week&type=before&count=-%3&indicator=kline").arg(symbol).arg(begin).arg(m_numsCnt);
    qDebug()<<urlStr;
    // 下一步就是根据url请求到的数据，进行分析
    m_menuIndex = 0; // 第一个菜单
    m_request.setUrl(QUrl(urlStr)); // 设置新的网址
    m_manager.get(m_request);  // 打开网址，拿数据
}

void GetStockList::priceSlot()
{
    qDebug()<<"最新价格";
    if(m_model->rowCount() == 0)return;
    int row = ui->tableView->currentIndex().row();
    QString symbol = m_model->item(row,1)->text(); // 取当前行所在的股票代号
    qint64 begin = QDateTime::currentMSecsSinceEpoch(); // 取当前时间戳
    int numsCnt = 20;
    QString urlStr = QString("https://stock.xueqiu.com/v5/stock/chart/kline.json?symbol=%1&begin=%2&period=day&type=before&count=-%3&indicator=kline").arg(symbol).arg(begin).arg(numsCnt);
    qDebug()<<urlStr;
    // 下一步就是根据url请求到的数据，进行分析
    m_menuIndex = 1; // 第二个菜单项
    m_request.setUrl(QUrl(urlStr));
    m_manager.get(m_request);

}
