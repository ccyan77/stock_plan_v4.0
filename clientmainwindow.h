#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include "getstocklist.h"
#include "stockpolicy.h"

namespace Ui {
class ClientMainWindow;
}

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(QWidget *parent = 0);
    ~ClientMainWindow();

    void initUi();

private:
    Ui::ClientMainWindow *ui;
    GetStockList *m_getStockList;
    StockPolicy *m_stockPolicy;
};

#endif // CLIENTMAINWINDOW_H
