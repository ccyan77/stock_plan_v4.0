#ifndef GETSTOCKLIST_H
#define GETSTOCKLIST_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QMenu>
#include <QLineSeries>   // 折线类
#include <QSplineSeries> // 曲线类

QT_CHARTS_USE_NAMESPACE  // 命名空间得使用起来，要不然类会拿不到


/*
 * 关于SSL的处理，由于要请求 https的链接，需要将编译器目录下的两个dll动态库拷贝到对应编译器的bin目录下
 * 将libeay32.dll 和 ssleay32.dll 两个文件复制到对应的区域
 * 例如从编译器的opt\bin原始目录：...\Qt5.9.4\Tools\mingw530_32\opt\bin 找个这2个dll
 * 拷贝到编译器的bin目录下: ...\Qt5.9.4\Tools\mingw530_32\bin
 */
namespace Ui {
class GetStockList;
}

class GetStockList : public QMainWindow
{
    Q_OBJECT

public:
    explicit GetStockList(QWidget *parent = 0);
    ~GetStockList();
    void getStockAllCode(QByteArray &buffer); // 获取所有沪深股票的信息
    void getStockOne(QByteArray &buffer); // 获取单只股票的处理

signals:
    void sendChartData(QLineSeries*);  // 折线
    void sendChartData(QSplineSeries*);  // 曲线

private slots:
    void on_pushButton_get_clicked();
    void showAplyData(QNetworkReply *reply);  // 用来响应请求的回传数据的槽函数

    void on_pushButton_get_2_clicked();  // 获取沪深股市数据的按钮槽函数
    void showMenu();
    void policySlot();
    void priceSlot();

private:
    Ui::GetStockList *ui;
    // 这2个用于爬取数据的
    QNetworkRequest m_request; // 用于管理请求头的
    QNetworkAccessManager m_manager; // 用于管理http请求及响应的

    // 下面这几个用于显式爬到的股票数据
    QStandardItemModel *m_model; // 标准项模型，用来存储数据的，将模型和视图进行了分离
    QMenu *m_menu; // 菜单指针，用来处理右击菜单的
    int m_numsCnt; // 单个股票的数据行数
    int m_menuIndex; // 针对单个股票需要判断是哪个操作，用来记录下标的
};

#endif // GETSTOCKLIST_H
