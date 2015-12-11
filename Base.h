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

#ifndef BASE_H_
#define BASE_H_

#include <string>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <json/json.h>
#include <curl/curl.h>
#include <vector>
#include <list>
#include <unistd.h>
#include <QString>
#include <QMutex>

using namespace std;

#define LIVESERVER
//#define DEBUG

#define ARMED

//#define DESKTOP
//#define ANDROID

#define MAXROWS 50
#define MAXRETRY 3
#define SLEEPTIME 30
#define TIMEOUT 10

#define BITMARKET 0
#define BITMASZYNA 1

#define T_SENT 0
#define T_COMPLETED 10

#define FIRST 1
#define SECOND 2

#define BID 0
#define BUY 0
#define ASK 1
#define SELL 1
#define MARKETS 2
#define EXCHANGES 3
#define ORDERS 4
#define BALANCE 5
#define LASTTRADES 6
#define MENU 7
#define LEVERAGE 8
#define OPENPOSITIONS 9
#define OPENSWAPS 10
#define ORDER 11
#define HISTORY 12
#define ORDERTYPE 13

#define MENUITEMSCOUNT 12

#define ENGLISH 0
#define POLISH 1

#define MAXMODELS 14

#define SATO 100000000

#define BUF 2048

#define PLN 0
#define EUR 1
#define BTC 2
#define LTC 3
#define KBM 4
#define LBTC 5

#define MAXCURR 6

#define MAXTRANS 200

//#define ILESWIECZEK 40
#define ILESWIECZEKMAX 60

static const string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

struct MemoryStruct {
	char *memory;
	size_t size;
};

class Ohlc
{
public:
    long t;
    double o,h,l,c;
};

class Offer
{
public:
	string id;
	char type;
    double price,amount,sum,sumLower;
    long time;

    QString getPrice();
    QString getAmount();
    QString format(double);
};

class Position:public Offer
{
public:
    double leverage,fiatTotal,fiatOpened,fiatClosed,security,rateOpen,rateClose,rateProfit,rateLoss,rateCurrent,fees,profit,profitPercentage;
};

class Balance
{
public:
    double balance[MAXCURR],blocked[MAXCURR];
	Balance(){
        int i;

        for(i=0;i<MAXCURR;i++)
        {
            balance[i]=0.0;
            blocked[i]=0.0;
        }
	}
};

class Fee
{
public:
    double maker,taker;
};

class Transfer
{
public:
	int id;
	double amount;
	char cur,status,fromm,tom;
};

class Currency
{
public:
    string name;
    int type;
};

class Swap
{
public:
    string id;
    double amount,rate,earnings;
};

class Deposit
{
public:
    string currency,bank_name,pay_to,acc_num,swift_code,transfer_title;
};

class Alert
{
public:
    double bidbelow,bidabove,askbelow,askabove;
    bool active;
    string name;
};

class Ticker
{
public:
   double bid,ask,last,high,low,volume;
   Ticker();
};

class Tables
{
public:
    vector<Offer> bids,asks;
};

class History
{
public:
    string id,type,currency;
    double amount,rate,commission;
    long long time;
};

class Exchange
{
public:
	Balance balance;
    Fee fees;
    vector<Offer> orders,trades;
    Tables *currentTables;
    vector<Tables *> updatedTables;
    vector<History> historyTable;
    string name;
	char id;
    bool online,logged;
    double profit,profitPercentage,value;
    Ticker currentTicker;
    string key,secret,lasterror;
    vector<string> markets,marketsLong;
    vector<Currency> currencies;
    vector<Position> positions;
    vector<Swap> swaps;
    vector<Deposit> deposits;
    Deposit currentDeposit;
    CURLcode curlcode;
    CURL *curl;
    struct MemoryStruct chunk;
    vector<Alert> alerts;
    QMutex curl_mutex;

    Exchange();
    virtual ~Exchange();
    virtual bool getdepth(string market,Tables&)=0;
	virtual bool getfunds()=0;
    virtual bool getopenorders(string market)=0;
    virtual bool getticker(string market,Ticker &)=0;
    virtual bool cancelorder(string id)=0;
	virtual bool cancelall()=0;
    virtual bool tradepair(double price,double amount,char type,string market)=0;
    virtual bool withdraw(double,const string&,const string&,const string&,const string&,bool,bool,double&)=0;
    virtual bool deposit(const string&)=0;
    virtual bool lasttrades(string)=0;
    virtual bool marginList(string market)=0;
    virtual bool marginOpen(string market,char type,double leverage,double amount,double rate,double rateLoss,double rateProfit)=0;
    virtual bool marginClose(string market,string id,double amount)=0;
    virtual bool marginCancel(string market,string id,double amount)=0;
    virtual bool marginModify(string market,string id,double rate,double rateProfit,double rateLoss)=0;
    virtual bool swapList(string)=0;
    virtual bool swapOpen(string,double,double)=0;
    virtual bool swapClose(string,string)=0;
    virtual bool marginBalanceAdd(string market,double amount)=0;
    virtual bool marginBalanceRemove(string market,double amount)=0;
    virtual bool marketChart(string market,long,long,long)=0;
    virtual bool history(string currency)=0;
    void clearmem();
    void init();
    void logerror();
    void offline();
	double getFee(char type);
	double getBestBid();
	double getBestAsk();
	double getBestBidwFee();
	double getBestAskwFee();
	double getBestBidRaw();
	double getBestAskRaw();
	struct json_object *makeObj();
};

long tosato(double in);
double fromsato(long in);
void log(string s);
void log(QString);
void log(const char *s);
void logError(QString);

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
int64_t getctime();
inline bool is_base64(unsigned char c);
string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
string base64_decode(std::string const& encoded_string);
QString to_stringl(long l);
QString to_stringd(double d);
QString to_stringp(double d);
QString to_stringd2(double d);
QString to_string2(double d);
QString to_string1(double d);
QString to_string4(double d);
string randomString(int len);
int currency(string market,int k);
std::vector<unsigned char> sha256(const std::string& data);
void sha256buf(const std::string& data,unsigned char *buf);
std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& data,const std::vector<unsigned char>& key);
std::vector<unsigned char> hmac_sha256(const std::vector<unsigned char>& data,const std::vector<unsigned char>& key);
std::vector<unsigned char> b64_decode(const std::string& data);
std::string b64_encode(const std::vector<unsigned char>& data);
#endif /* BASE_H_ */
