#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTableWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QComboBox>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QStackedWidget>
#include <QQuickView>
#include <QStandardItemModel>
#include "Base.h"
#include "table.h"
//#include <QCameraViewfinder>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QPainter *painter;
    QFont font,font2;
    QTableWidget *bidtable,*asktable;
    QTextEdit *amount,*price;
    QPushButton *go;
    QRadioButton *buy,*sell;
    QWidget *widget,*widget2,*widgetTop,*widgetKeys;
    QGridLayout *layout,*layout2,*layoutTop;
    QStackedWidget *mainScreen;
    QComboBox *market,*exchange;
    QMenu *menu;
    QAction *exit,*trade,*keys;
    QQuickView *qview;
//    QCameraControllerWidget *camera;
   // QCamera *camera;
    //QCameraViewfinder *camview;
    double scalex,scaley;
    bool first,norefresh;
    string cmarket;
    list<Exchange *> exchanges;
    Exchange *currentExchange;
    int checked;

    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void populate();
    void populate(QTableWidget *table,list<Offer>& l,bool order);
    void set(QTableWidget *table,int,int,QString);
    void start();
    void closeView();
    QTableWidget *makeTable();
signals:

public slots:
    void changeEx(int);
    void changeMarket(int);
    void quit();
    void tradeWindow();
    void keysWindow();
};

#endif // MAINWINDOW_H
