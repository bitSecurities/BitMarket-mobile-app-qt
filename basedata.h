/*
 *   Copyright (c) 2015 Damian Obernikowicz <damin.obernikowicz@gmail.com>, BitMarket Limited Global Gateway 8, Rue de la Perle, Providence, Mahe, Seszele
 *
 *   This file is part of Bitkom.
 *
 *   Bitkom is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Bitkom is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Bitkom.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef BASEDATA_H
#define BASEDATA_H

#include <QObject>
#include <QMutex>
#include <QQuickWindow>
#include "table.h"
#include "Base.h"

class Table;
class BaseData:public QObject
{
    Q_OBJECT

public:
    double sx,sy,maxaskamount,maxbidvalue,lastFee;
    int width,height;
    vector<Exchange *> exchanges;
    Exchange *currentExchange;
    string cmarket;
    Table *model[MAXMODELS];
    QString translations[MAXTRANS];
    int currentLanguage;
    vector<Ohlc> swieczki;
    //int nmenu[MENUITEMSCOUNT];
    long timeframe;
    vector<int> menuitem;
    vector<float> levs;
    string pass;
    bool encrypted,reverse,updated;
    int limitRange,cmarketI;
    QObject *msgObject,*newDataObject;
    QMutex mutex_depth,mutex_list;
    QQuickWindow *window;
    int ileswieczek,podzial;
    bool stopemit;

    BaseData();
    double coma(QString);
    bool loadLastParallel();
    Q_INVOKABLE void getdepthimmediate();
    Q_INVOKABLE int getCurrId(int t);
    Q_INVOKABLE void rotate(int);
    Q_INVOKABLE void updateLast();
    Q_INVOKABLE void remove(const QUrl& path);
    Q_INVOKABLE int getWidth();
    Q_INVOKABLE int getHeight();
    Q_INVOKABLE double scalex();
    Q_INVOKABLE double scaley();
    Q_INVOKABLE void changeEx(QString);
    Q_INVOKABLE void changeMarket(QString);
    Q_INVOKABLE QString buyPrice(int);
    Q_INVOKABLE QString buyAmount(int);
    Q_INVOKABLE QString sellPrice(int);
    Q_INVOKABLE QString sellAmount(int);
    Q_INVOKABLE bool login(QString);
    Q_INVOKABLE bool isLogged();
    Q_INVOKABLE QString getFees();
    Q_INVOKABLE bool getOpenOrders();
    Q_INVOKABLE bool execute(QString,QString,bool);
    Q_INVOKABLE bool executeLeverage(QString,QString,int,int,QString,QString);
    Q_INVOKABLE bool cancelOrder(QString);
    Q_INVOKABLE void preparetrans();
    Q_INVOKABLE QString trans(int);
    QString getlangfile();    
    Q_INVOKABLE void changelang(QString name);
    Q_INVOKABLE int currentlang();
    Q_INVOKABLE void loadLast();
    void computedepth(Tables&);
    Q_INVOKABLE double getmaxaskamount();
    Q_INVOKABLE double getmaxbidvalue();
    Q_INVOKABLE void getdepth();
    Q_INVOKABLE bool getFunds();
    Q_INVOKABLE QString getMarketInfo(int);
    Q_INVOKABLE void savekeys();
    void loadkeys();
    Q_INVOKABLE void setKey(QString);
    Q_INVOKABLE void setSecret(QString);
    Q_INVOKABLE QString getKey();
    Q_INVOKABLE int getRow(QString,QString,bool);
    Q_INVOKABLE QString getFirstCurrency();
    Q_INVOKABLE QString curtostring(int);
    Q_INVOKABLE void prepareChartData();
    Q_INVOKABLE QString getSecondCurrency();
    Q_INVOKABLE void changeTimeFrame(QString);
    Q_INVOKABLE QString getLeverageInfo(int);
    Q_INVOKABLE QString getLastError();
    Q_INVOKABLE bool marginList();
    Q_INVOKABLE bool marginBalanceAdd(QString);
    Q_INVOKABLE bool marginBalanceRemove(QString);
    Q_INVOKABLE bool marginClose(QString,double);
    Q_INVOKABLE bool marginCancel(QString,double);
    Q_INVOKABLE bool swapOpen(QString,QString,QString);
    Q_INVOKABLE bool swapClose(QString,QString);
    Q_INVOKABLE bool swapList(QString);
    string encryptValue(string enc);
    string decryptValue(string enc);
    Q_INVOKABLE void encryptKeys(QString);
    bool checkPass();
    Q_INVOKABLE bool checkWithdrawalPass(QString);
    Q_INVOKABLE bool isEncrypted();\
    Q_INVOKABLE bool deposit(QString);
    Q_INVOKABLE bool withdraw(double amount,QString currency,QString address,QString swift,QString note,bool fast);
    Q_INVOKABLE bool testWithdraw(double amount,QString currency,QString address,QString swift,QString note,bool fast);
    Q_INVOKABLE double getLastFee();
    Q_INVOKABLE QString getDeposit();
    Q_INVOKABLE void changeRange(QString);
    Q_INVOKABLE void makeAlert(QString);
    Q_INVOKABLE void logError(QString);
    Q_INVOKABLE void saveAlerts(bool,double,double,double,double);
    Q_INVOKABLE bool getActive();
    Q_INVOKABLE QString getBidAbove();
    Q_INVOKABLE QString getBidBelow();
    Q_INVOKABLE QString getAskAbove();
    Q_INVOKABLE QString getAskBelow();
    Q_INVOKABLE  void checkAlerts();
    Q_INVOKABLE bool getReverse();
    Q_INVOKABLE void setReverse(bool);
    Q_INVOKABLE bool marginModify(QString id,double rate,double takeprofit,double stoploss);
    Q_INVOKABLE void scanner();
    Q_INVOKABLE QString getscannedtxt();
    bool getdepthparallel();
    Q_INVOKABLE bool updateTables();
    Q_INVOKABLE bool getHistory(int);
    Q_INVOKABLE QString menu(int);
    Q_INVOKABLE double getAvLeverage();
    Q_INVOKABLE double getBid();
    Q_INVOKABLE double getAsk();
    Q_INVOKABLE double getLeverage(int);
public slots:
    void onCompleted();
signals:
    void dataLoaded();
};

#endif // BASEDATA_H
