#include <QHeaderView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
//#include <QCameraViewfinder>
#include "Bitmarket.h"
#include "Bitmaszyna.h"
#include "mainwindow.h"
#include "QZXing/QZXing.h"

extern string last;

void MainWindow::set(QTableWidget *table,int x,int y,QString txt)
{
    if (table->item(x,y)==0) {
        table->setItem(x,y,new QTableWidgetItem(txt));
    }
    else table->item(x,y)->setText(txt);
}

void MainWindow::populate()
{
    list<string>::iterator it;

    market->clear();
    norefresh=true;
    for(it=currentExchange->marketsLong.begin();it!=currentExchange->marketsLong.end();it++) market->addItem(QString((*it).c_str()));
    market->setCurrentIndex(checked);
    norefresh=false;
    populate(bidtable,currentExchange->bids,false);
    populate(asktable,currentExchange->asks,true);
    asktable->scrollToBottom();
}
/*
void MainWindow::populate(QTableWidget *table,list<Offer>& l,bool invert)
{
    list<Offer>::iterator it;
    int i,maxrow;

    if (l.size()<MAXROWS) maxrow=l.size();
    else maxrow=MAXROWS;
    table->setRowCount(maxrow);
    i=0;
    if (invert)
    {
        for(it=l.begin();it!=l.end();it++)
        {
            set(table,maxrow-i-1,0,QString::number(it->price));
            set(table,maxrow-i-1,1,QString::number(it->amount));
            set(table,maxrow-i-1,2,QString::number(it->price*it->amount));
            i++;
            if (i>maxrow) break;
        }        
    }else
    {
        for(it=l.begin();it!=l.end();it++)
        {
            set(table,i,0,QString::number(it->price));
            set(table,i,1,QString::number(it->amount));
            set(table,i,2,QString::number(it->price*it->amount));
            i++;
            if (i>maxrow) break;
        }
    }
}
*/
void MainWindow::populate(QTableWidget *table,list<Offer>& l,bool invert)
{
    list<Offer>::iterator it;
    int i,maxrow;

    if (l.size()<MAXROWS) maxrow=l.size();
    else maxrow=MAXROWS;
    table->setRowCount(maxrow);
    i=0;
    if (invert)
    {
        for(it=l.begin();it!=l.end();it++)
        {
            set(table,maxrow-i-1,0,QString::number(it->price));
            set(table,maxrow-i-1,1,QString::number(it->amount));
            set(table,maxrow-i-1,2,QString::number(it->price*it->amount));
            i++;
            if (i>maxrow) break;
        }
    }else
    {
        for(it=l.begin();it!=l.end();it++)
        {
            set(table,i,0,QString::number(it->price));
            set(table,i,1,QString::number(it->amount));
            set(table,i,2,QString::number(it->price*it->amount));
            i++;
            if (i>maxrow) break;
        }
    }
}

QTableWidget *MainWindow::makeTable()
{
    QTableWidget *table=new QTableWidget();
    table->setFont(font);
    table->setColumnCount(4);
    table->setFixedWidth(200*scalex);
    table->setFixedHeight(240*scaley);
    table->setShowGrid(true);
    table->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setDefaultSectionSize(20*scaley);
    table->verticalHeader()->hide();
    table->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    table->horizontalHeader()->setDefaultSectionSize(60*scalex);
    table->horizontalHeader()->hide();
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setColumnWidth(3, 20*scalex);
    table->setFrameStyle(QFrame::NoFrame);
    return(table);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    Exchange *e;

    QZXing::registerQMLTypes();
    setbuf(stdout, NULL);
    painter=new QPainter();
    norefresh=false;
    first=true;
    checked=0;
    e=new Bitmarket();
    e->markets.push_back("BTCPLN");
    e->marketsLong.push_back("BTC/PLN");
    e->markets.push_back("LTCPLN");
    e->marketsLong.push_back("LTC/PLN");
    e->markets.push_back("BTCEUR");
    e->marketsLong.push_back("BTC/EUR");
    e->markets.push_back("LTCBTC");
    e->marketsLong.push_back("LTC/BTC");
    exchanges.push_back(e);
    currentExchange=e;
    e=new Bitmaszyna();
    e->markets.push_back("BTCPLN");
    e->marketsLong.push_back("BTC/PLN");
    e->markets.push_back("LTCPLN");
    e->marketsLong.push_back("LTC/PLN");
    e->markets.push_back("KBMBTC");
    e->marketsLong.push_back("KBM/BTC");
    exchanges.push_back(e);
    cmarket=currentExchange->markets.front();
    currentExchange->getdepth(cmarket);
}

void MainWindow::start()
{
    mainScreen=new QStackedWidget();
    widget=new QWidget(this);
    widgetKeys=new QWidget(this);
    widget2=new QWidget(this);
    widgetTop=new QWidget(this);
//    camera=new QCameraControllerWidget(this);
    layout=new QGridLayout();
    layout2=new QGridLayout();
    layoutTop=new QGridLayout();
    amount=new QTextEdit(this);
    price=new QTextEdit(this);
    go=new QPushButton(this);
    buy=new QRadioButton(this);
    sell=new QRadioButton(this);
    market = new QComboBox(this);
    exchange = new QComboBox(this);
    exchange->addItem(tr("Bitmarket"));
    exchange->addItem(tr("Bitmaszyna"));
    widget->setLayout(layout);
    widgetTop->setLayout(layoutTop);
    mainScreen->addWidget(widget);
    mainScreen->addWidget(widgetKeys);
//    mainScreen->addWidget(camera);
    mainScreen->setCurrentIndex(0);
    setCentralWidget(mainScreen);
//    setCentralWidget(camera);
    //qview = new QQuickView();
    //qview->setResizeMode(QQuickView::SizeRootObjectToView);
    //qview->rootContext()->setContextProperty("closeView", closeView());
    //qview->setSource(QUrl("qrc:///camera.qml"));
    /*Table model;
    MyModel model2;
    model2.setData(model2.index(0,0), "Data 1", MyModel::price);
    model2.setData(model2.index(0,0), "Data 1", MyModel::amount);
    model2.setData(model2.index(1,0), "Data 2", MyModel::price);
    model2.setData(model2.index(1,0), "Data 2", MyModel::amount);
    qview->engine()->rootContext()->setContextProperty("theModel", &model);
    qview->engine()->rootContext()->setContextProperty("themodel2", &model2);
    qview->setSource(QUrl("qrc:///main.qml"));
    if ((width()==200)||(width()==640)) {
        resize(360,600);
        qview->resize(360,600);
    }*/
    /*QWidget *container = QWidget::createWindowContainer(qview);
    QSize ss;
    ss.setWidth(width());
    ss.setHeight(height());
    container->setMinimumSize(ss);
    container->setMaximumSize(ss);
    container->setFocusPolicy(Qt::TabFocus);
    mainScreen->addWidget(container);*/
    //widgetLayout->addWidget(container);
    //log(QString::number(width()).toStdString());
    //view->show();
    scalex=width()/480.0;
    scaley=height()/800.0;
    font.setPixelSize(12*scaley);
    font2.setPixelSize(16*scaley);
    buy->setChecked(true);
    buy->setText("Kup");
    buy->setFont(font2);
    sell->setText("Sprzedaj");
    sell->setFont(font2);
    go->setText("Wykonaj");
    go->setFont(font2);
    bidtable=makeTable();
    asktable=makeTable();
    populate();
    layout->setContentsMargins(0,0,0,0);
    layout2->setContentsMargins(0,0,0,0);
    layoutTop->setContentsMargins(0,0,0,0);
    widget2->setLayout(layout2);
    widget2->setFixedHeight(70*scaley);
    widgetTop->setFixedHeight(50*scaley);
    buy->setFixedWidth(120*scalex);
    sell->setFixedWidth(120*scalex);
    amount->setFixedWidth(100*scalex);
    amount->setFixedHeight(36*scalex);
    price->setFixedWidth(100*scalex);
    price->setFixedHeight(36*scalex);
    go->setFixedWidth(100*scalex);
    layoutTop->addWidget(exchange,0,0);
    layoutTop->addWidget(market,0,1);
    layout2->addWidget(buy,0,0);
    layout2->addWidget(sell,1,0);
    layout2->addWidget(amount,0,1,2,1);
    layout2->addWidget(price,0,2,2,1);
    layout2->addWidget(go,0,3,2,1);
    layout->addWidget(widgetTop,0,1);
    layout->addWidget(asktable,1,1);
    layout->addWidget(widget2,2,0,1,3);
    layout->addWidget(bidtable,3,1);
    connect(exchange,SIGNAL(currentIndexChanged(int)),this,SLOT(changeEx(int)));
    connect(market,SIGNAL(currentIndexChanged(int)),this,SLOT(changeMarket(int)));
    menu=menuBar()->addMenu(tr("Menu"));
    trade = new QAction(tr("&Trade"), this);
    trade->setStatusTip(tr("Trade"));
    connect(trade, SIGNAL(triggered()), this, SLOT(tradeWindow()));
    menu->addAction(trade);
    keys = new QAction(tr("&Keys"), this);
    keys->setStatusTip(tr("Keys"));
    connect(keys, SIGNAL(triggered()), this, SLOT(keysWindow()));
    menu->addAction(keys);
    exit = new QAction(tr("&Exit"), this);
    exit->setStatusTip(tr("Exit"));
    connect(exit, SIGNAL(triggered()), this, SLOT(quit()));
    menu->addAction(exit);
    /*
    camera=new QCamera(this);
    QCameraViewfinder *camview = new QCameraViewfinder(this);
    camera->setViewfinder(camview);
    mainScreen->addWidget(camview);
    setCentralWidget(camview);
    camera->start();*/
    //keysWindow();
    /*
    Table model;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("themodel", &model);
    engine.load(QUrl("qrc:/main.qml"));
    QList<QObject*> temp = engine.rootObjects();
    QObject *topLevel = temp.value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    if ( !window ) {
        qWarning("Error: Your root item has to be a Window.");
        //return -1;
     }
    window->show();*/
    //QZXing qr;
    //log(":"+qr.decodeImageFromFile("qr2.png").toStdString());
    //log(":"+QString::number(qr.getProcessTimeOfLastDecoding()).toStdString());
/*    model=new QStandardItemModel(4, 4);
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model->setItem(row, column, item);
        }
    }*/
}

void MainWindow::tradeWindow()
{
    mainScreen->setCurrentIndex(0);
}

void MainWindow::keysWindow()
{
    //mainScreen->setCurrentIndex(2);
    this->hide();
//    menu->hide();
//    menuBar()->hide();
    qview->show();
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if (first)
    {
        start();
        first=false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *)
{

}

void MainWindow::changeMarket(int m)
{
    list<string>::iterator it;
    int i;

    if (norefresh) return;
    i=0;
    for(it=currentExchange->markets.begin();it!=currentExchange->markets.end();it++)
    {
        if (i==m)
        {
            cmarket=*it;
            checked=i;
            currentExchange->getdepth(cmarket);
            populate();
            break;
        }
        i++;
    }
}

void MainWindow::changeEx(int ex)
{
    list<Exchange *>::iterator it;
    int i;

    if (norefresh) return;
    i=0;
    for(it=exchanges.begin();it!=exchanges.end();it++)
    {
        if (i==ex)
        {
            currentExchange=*it;
            cmarket=currentExchange->markets.front();
            checked=0;
            currentExchange->getdepth(cmarket);
            populate();
            break;
        }
        i++;
    }
}
