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

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>
#include <QRect>
#include <QFile>
#include <QUrl>
#include <QSettings>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include "basedata.h"
#include "Bitmarket.h"
#include "Bitmaszyna.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#ifdef ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif
extern string path;

BaseData::BaseData()
{    
#ifndef ANDROID
    //    width=1080;
    //    height=1920;
    width=360;
    height=600;
    sx=width/1080.0;
    sy=height/1920.0;
    path="/logi";
#else
    QRect rec = QApplication::desktop()->screenGeometry();
    height = rec.height();
    width = rec.width();
    sx=width/1080.0;
    sy=height/1920.0;
    QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
    path = mediaPath.toString().toStdString();
#endif
    Exchange *e;
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
    //exchanges.push_back(e);
    cmarket=currentExchange->markets.front();
    cmarketI=0;
    cmarket=currentExchange->markets[cmarketI];
    //    currentLanguage=ENGLISH;
    currentLanguage=POLISH;
    encrypted=false;
    reverse=true;
#ifdef DEBUG
    //savekeys();
#endif
    loadkeys();
    preparetrans();
    menuitem.push_back(0);
    menuitem.push_back(1);
    menuitem.push_back(2);
    menuitem.push_back(64);
    menuitem.push_back(65);
    menuitem.push_back(66);
    menuitem.push_back(31);
    menuitem.push_back(36);
    menuitem.push_back(37);
    menuitem.push_back(3);
    menuitem.push_back(4);
    menuitem.push_back(5);
    menuitem.push_back(6);
    levs.push_back(1.0);
    levs.push_back(2.0);
    levs.push_back(3.0);
    levs.push_back(5.0);
    levs.push_back(10.0);
    timeframe=300;
    OpenSSL_add_all_algorithms();
    connect(this, SIGNAL(dataLoaded()),this, SLOT(onCompleted()));
    ileswieczek=40;
    podzial=6;
    stopemit=true;
    //notify("aa");
    //OPENSSL_config(NULL);

    //timeframe=604800;
    //currentExchange->history("BTC");
    //    currentExchange->transferToLeverage(cmarket,0.1);
    //    currentExchange->marketChart(cmarket);
    //prepareChartData();
}

void BaseData::saveAlerts(bool active,double bidabove,double bidbelow,double askabove,double askbelow)
{
    currentExchange->alerts[cmarketI].active=active;
    currentExchange->alerts[cmarketI].bidabove=bidabove;
    currentExchange->alerts[cmarketI].bidbelow=bidbelow;
    currentExchange->alerts[cmarketI].askabove=askabove;
    currentExchange->alerts[cmarketI].askbelow=askbelow;
    savekeys();
}

bool BaseData::getHistory(int n)
{
    bool ret;

    ret=currentExchange->history(currentExchange->currencies[n].name);
    if (ret)
    {
        emit(model[HISTORY]->layoutChanged());
    }
    return(ret);
}

bool BaseData::getActive()
{
    return(currentExchange->alerts[cmarketI].active);
}

QString BaseData::menu(int k)
{
    return(translations[menuitem[k]]);
}

QString BaseData::getBidAbove()
{
    if (currentExchange->alerts[cmarketI].bidabove>0) return(to_string4(currentExchange->alerts[cmarketI].bidabove));
    else return(trans(69));
}

QString BaseData::getBidBelow()
{
    if (currentExchange->alerts[cmarketI].bidbelow>0) return(to_string4(currentExchange->alerts[cmarketI].bidbelow));
    else return(trans(72));
}

QString BaseData::getAskAbove()
{
    if (currentExchange->alerts[cmarketI].askabove>0) return(to_string4(currentExchange->alerts[cmarketI].askabove));
    else return(trans(73));
}

QString BaseData::getAskBelow()
{
    if (currentExchange->alerts[cmarketI].askbelow>0) return(to_string4(currentExchange->alerts[cmarketI].askbelow));
    else return(trans(70));
}

bool BaseData::getReverse()
{
    return(reverse);
}

void BaseData::setReverse(bool r)
{
    reverse=r;
    savekeys();
}

void BaseData::changeRange(QString name)
{
    if (name=="---") limitRange=-1;
    else
    {
        int limitSec;
        vector<Offer>::iterator it;
        time_t now;

        limitSec=0;
        now=time(NULL);
        limitRange=0;
        if (name=="30 m") limitSec=1800;
        else if (name=="90 m") limitSec=5400;
        else if (name=="4 h") limitSec=14400;
        else if (name=="24 h") limitSec=86400;
        else if (name=="48 h") limitSec=172800;
        for (it=currentExchange->trades.begin();it!=currentExchange->trades.end();++it)
        {
            if (it->time<now-limitSec) break;
            limitRange++;
        }
    }
    emit(model[LASTTRADES]->layoutChanged());
}

bool BaseData::deposit(QString currency)
{
    vector<Deposit>::iterator it;
    bool found;

    found=false;
    for(it=currentExchange->deposits.begin();it!=currentExchange->deposits.end();++it)
    {
        if(it->currency==currency.toStdString())
        {
            currentExchange->currentDeposit=*it;
            found=true;
            break;
        }
    }
    if (!found) return(currentExchange->deposit(currency.toStdString()));
    return(true);
}

QString BaseData::getDeposit()
{
    if ((currentExchange->currentDeposit.currency=="BTC")||(currentExchange->currentDeposit.currency=="LTC")) return(trans(110)+" "+currentExchange->currentDeposit.currency.c_str()+":<br><br><b>"+currentExchange->currentDeposit.acc_num.c_str())+"</b>";
    else {
        QString s;
        s=trans(106)+": <b>"+QString(currentExchange->currentDeposit.acc_num.c_str())+"</b><br>"+trans(107)+": <b>"+currentExchange->currentDeposit.bank_name.c_str()+"</b><br>"+trans(108)+": <b>"+currentExchange->currentDeposit.pay_to.c_str()+"</b><br>"+trans(109)+": <b>"+currentExchange->currentDeposit.transfer_title.c_str()+"</b><br>";
        if (currentExchange->currentDeposit.currency=="EUR") s+=trans(78)+": <b>"+currentExchange->currentDeposit.swift_code.c_str()+"</b>";
        return(s);
    }
}

bool BaseData::withdraw(double amount,QString currency,QString address,QString swift,QString note,bool fast)
{
    return(currentExchange->withdraw(amount,currency.toStdString(),address.toStdString(),swift.toStdString(),note.toStdString(),false,fast,lastFee));
}

bool BaseData::testWithdraw(double amount,QString currency,QString address,QString swift,QString note,bool fast)
{
    if ((currency=="PLN")||(currency=="EUR")) return(currentExchange->withdraw(amount,currency.toStdString(),address.toStdString(),swift.toStdString(),note.toStdString(),true,fast,lastFee));
    else return(true);
}

double BaseData::getLastFee()
{
    return(lastFee);
}

void BaseData::encryptKeys(QString pass)
{
    this->pass=pass.toStdString();
    encrypted=true;
    savekeys();
}

bool BaseData::marginList()
{
    bool ret;

    ret=currentExchange->marginList(cmarket);
    emit(model[OPENPOSITIONS]->layoutChanged());
    return(ret);
}

bool BaseData::marginBalanceAdd(QString amount)
{
    return(currentExchange->marginBalanceAdd(cmarket,amount.toDouble()));
}

bool BaseData::swapList(QString currency)
{
    bool ret;

    ret=currentExchange->swapList(currency.toStdString());
    emit(model[OPENSWAPS]->layoutChanged());
    return(ret);
}

bool BaseData::swapOpen(QString currency,QString price,QString amount)
{
    return(currentExchange->swapOpen(currency.toStdString(),coma(price),coma(amount)));
}

bool BaseData::swapClose(QString currency,QString id)
{
    return(currentExchange->swapClose(currency.toStdString(),id.toStdString()));
}

bool BaseData::marginClose(QString id,double amount)
{
    return(currentExchange->marginClose(cmarket,id.toStdString(),amount));
}

bool BaseData::marginCancel(QString id,double amount)
{
    return(currentExchange->marginCancel(cmarket,id.toStdString(),amount));
}

bool BaseData::marginModify(QString id,double rate,double takeprofit,double stoploss)
{
    return(currentExchange->marginModify(cmarket,id.toStdString(),rate,takeprofit,stoploss));
}

bool BaseData::marginBalanceRemove(QString amount)
{
    return(currentExchange->marginBalanceRemove(cmarket,amount.toDouble()));
}

QString BaseData::getLastError()
{
    return(QString(currentExchange->lasterror.c_str()));
}

bool BaseData::executeLeverage(QString price,QString amount,int type,int leverage,QString rateProfit,QString rateLoss)
{
    return(currentExchange->marginOpen(cmarket,type,levs[leverage],coma(amount),coma(price),coma(rateProfit),coma(rateLoss)));
}

double BaseData::getLeverage(int leverage)
{
    return(levs[leverage]);
}

double BaseData::getAvLeverage()
{
    return(currentExchange->balance.balance[LBTC]);
}

double BaseData::getBid()
{
    return(currentExchange->currentTicker.bid);
}

double BaseData::getAsk()
{
    return(currentExchange->currentTicker.ask);
}

QString BaseData::getLeverageInfo(int k)
{
    if (k==0) return(to_stringd(currentExchange->balance.balance[LBTC])+" "+getFirstCurrency()+"\n");
    else if (k==1) return(to_stringd(currentExchange->balance.blocked[LBTC])+" "+getFirstCurrency()+"\n");
    else if (k==2) return(to_stringd(currentExchange->balance.blocked[LBTC]+currentExchange->balance.balance[LBTC])+" "+getFirstCurrency()+"\n");
    else if (k==3) return(to_string2(currentExchange->profit)+"\n");
    else if (k==4) return(to_string2(currentExchange->profitPercentage)+" %\n");
    else if (k==5) return(to_string2(currentExchange->value)+" %\n");
    return("");
}

void BaseData::changeTimeFrame(QString tframe)
{
    if (tframe=="1 m") timeframe=60;
    else if (tframe=="4 m") timeframe=240;
    else if (tframe=="16 m") timeframe=960;
    else if (tframe=="2 h") timeframe=7200;
    else if (tframe=="8 h") timeframe=28800;
    else if (tframe=="1 d") timeframe=86400;
    else if (tframe=="2 d") timeframe=172800;
    else if (tframe=="4 d") timeframe=345600;
    else timeframe=60;
    prepareChartData();
}

int BaseData::getWidth()
{
    return(width);
}

int BaseData::getHeight()
{
    return(height);
}

void BaseData::prepareChartData()
{
    double minzakres,maxzakres;
    bool first,retry;
    long zks;
    vector<Offer>::iterator it;
    Ohlc ohlc;

    minzakres=0.0;
    maxzakres=0.0;
    zks=timeframe;
    first=true;
    swieczki.clear();
    mutex_list.lock();
    for(it=currentExchange->trades.begin();it!=currentExchange->trades.end();++it)
    {
        if ((minzakres>it->price)||(minzakres==0.0)) minzakres=it->price;
        if (maxzakres<it->price) maxzakres=it->price;
    }
    for(it=currentExchange->trades.begin();it!=currentExchange->trades.end();++it)
    {
        do
        {
            retry=false;
            if (first)
            {
                time_t tt;

                ohlc.h=it->price;
                ohlc.o=it->price;
                ohlc.c=it->price;
                ohlc.l=it->price;
                tt=time(NULL);
                ohlc.t=tt-tt%zks;
                //log(QString::number(tt)+"\n");
                //log(QString::number(ohlc.t)+"\n");
                first=false;
            }
            if (it->time<ohlc.t)
            {
                swieczki.push_back(ohlc);
                ohlc.t=ohlc.t-zks;
                ohlc.h=ohlc.o;
                ohlc.l=ohlc.o;
                ohlc.c=ohlc.o;
                retry=true;
            }
            if (it->price>ohlc.h) ohlc.h=it->price;
            if (it->price<ohlc.l) ohlc.l=it->price;
            ohlc.o=it->price;
            //log(("timeframe: "+QString::number(it->time)+" "+QString::number(ohlc.t-zks)+"\n").toStdString());
        }while((retry)&&(swieczki.size()<90));
        if (swieczki.size()>=90) break;
        //log(("timeframe: "+QString::number(it->time)+" "+QString::number(ohlc.t)+" "+QString::number(swieczki.size())+"\n").toStdString());
    }
    mutex_list.unlock();
    swieczki.push_back(ohlc);
    if (swieczki.size()<ILESWIECZEKMAX)
    {
        swieczki.clear();
        currentExchange->marketChart(cmarket,swieczki.front().t-ILESWIECZEKMAX*timeframe,ohlc.t,timeframe);
    }
}

void BaseData::rotate(int st)
{
    if (st==90) {
        ileswieczek=60;
        podzial=9;
    }
    else {
        podzial=6;
        ileswieczek=40;
    }
}

QString BaseData::curtostring(int cur)
{
    if (cur==BTC) return("BTC");
    else if (cur==LTC) return("LTC");
    else if (cur==KBM) return("KBM");
    else if (cur==PLN) return("PLN");
    else if (cur==EUR) return("EUR");
    else return("");
}

int BaseData::getCurrId(int t)
{
    return(currency(cmarket,t));
}

QString BaseData::getFirstCurrency()
{
    return(curtostring(currency(cmarket,FIRST)));
}

QString BaseData::getSecondCurrency()
{
    return(curtostring(currency(cmarket,SECOND)));
}

int BaseData::getRow(QString price,QString amount,bool side)
{
    int i,s;
    double sum,res;
    vector<Offer>::iterator it;

    i=0;
    sum=0.0;
    s=currency(cmarket,SECOND);
    if ((s==PLN)||(s==EUR)) res=0.009;
    else res=0.00000009;
    if (side)
    {
        i=currentExchange->currentTables->asks.size()-1;
        for(it=currentExchange->currentTables->asks.begin();it!=currentExchange->currentTables->asks.end();++it)
        {
            sum+=it->amount;
            if (((price.toDouble()<it->price)&&(fabs(price.toDouble()-it->price)>res))||(sum>amount.toDouble())||(fabs(sum-amount.toDouble())<res))
            {
                if ((i==(int)currentExchange->currentTables->asks.size()-1)&&(fabs(price.toDouble()-it->price)>res)) return(-1);
                return(i);
            }
            i--;
        }
    }else if (!side)
    {
        for(it=currentExchange->currentTables->bids.begin();it!=currentExchange->currentTables->bids.end();++it)
        {
            sum+=it->amount;
            if (((price.toDouble()>it->price)&&(fabs(price.toDouble()-it->price)>res))||(sum>amount.toDouble())||(fabs(sum-amount.toDouble())<res))
            {
                //qDebug()<<i<<" "<<price.toDouble()<<" "<<it->price<<" "<<sum<<" "<<amount.toDouble()<<" "<<(price.toDouble()>it->price)<<" "<<fabs(price.toDouble()-it->price);
                if ((i==0)&&(fabs(price.toDouble()-it->price)>res)) return(-1);
                return(i);
            }
            i++;
        }
    }
    return(-1);
}

void BaseData::setKey(QString key)
{
    currentExchange->key=key.toStdString();
    savekeys();
}

void BaseData::setSecret(QString key)
{
    currentExchange->secret=key.toStdString();
    savekeys();
}

QString BaseData::getKey()
{
    if ((encrypted)&&(!currentExchange->logged)) return("");
    else return(currentExchange->key.c_str());
}

void handleErrors()
{

}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

string BaseData::encryptValue(string txt)
{
    if (encrypted)
    {
        int ciphertext_len;
        std::vector<unsigned char> ciphertext(256);
        unsigned char key[SHA256_DIGEST_LENGTH+1],iv[SHA256_DIGEST_LENGTH+1];

        sha256buf((pass+"strange seed value").data(),key);
        sha256buf((pass+"even stranger seed value").data(),iv);
        ciphertext_len = encrypt ((unsigned char *)txt.c_str(),txt.size()+1, key, iv,ciphertext.data());
        ciphertext.resize(ciphertext_len);
        return(b64_encode(ciphertext));
    }else return(txt);
}

string BaseData::decryptValue(string enc)
{
    if (encrypted)
    {
        std::vector<unsigned char> ciphertext;
        unsigned char key[SHA256_DIGEST_LENGTH+1],iv[SHA256_DIGEST_LENGTH+1];
        std::vector<unsigned char> decryptedtext(256);

        sha256buf((pass+"strange seed value").data(),key);
        sha256buf((pass+"even stranger seed value").data(),iv);
        ciphertext=b64_decode(enc);
        decrypt(ciphertext.data(), ciphertext.size(), key, iv,decryptedtext.data());
        //pass check
        return((char *)decryptedtext.data());
    }else return(enc);
}

void BaseData::loadkeys()
{
    vector<Exchange *>::iterator it;
    vector<string>::iterator it2;
    unsigned int i;

    QSettings settings("Bitmarket","Bitkom");
    if (settings.value("Bitkom/passCheck")==QVariant()) savekeys();
    for(it=exchanges.begin();it!=exchanges.end();++it)
    {
        (*it)->key=decryptValue(settings.value(QString(((*it)->name+"/key").c_str())).toString().toStdString());
        (*it)->secret=decryptValue(settings.value(QString(((*it)->name+"/secret").c_str())).toString().toStdString());
        i=0;
        for(it2=(*it)->markets.begin();it2!=(*it)->markets.end();++it2)
        {
            Alert a;
            a.name=(*it2);
            a.active=settings.value(QString(((*it)->name+"/"+a.name+"/active").c_str())).toBool();
            a.bidabove=settings.value(QString(((*it)->name+"/"+a.name+"/bidabove").c_str())).toDouble();
            a.bidbelow=settings.value(QString(((*it)->name+"/"+a.name+"/bidbelow").c_str())).toDouble();
            a.askabove=settings.value(QString(((*it)->name+"/"+a.name+"/askabove").c_str())).toDouble();
            a.askbelow=settings.value(QString(((*it)->name+"/"+a.name+"/askbelow").c_str())).toDouble();
            if ((*it)->alerts.size()<=i) (*it)->alerts.push_back(a);
            else (*it)->alerts[i]=a;
            i++;
        }
    }
    encrypted=settings.value("Bitkom/encrypted").toBool();
    reverse=settings.value("Bitkom/reverse").toBool();
}

void BaseData::savekeys(){
    vector<Exchange *>::iterator it;
    vector<Alert>::iterator it2;

    QSettings settings("Bitmarket","Bitkom");
    for(it=exchanges.begin();it!=exchanges.end();++it)
    {
        settings.setValue(QString(((*it)->name+"/key").c_str()), QString(encryptValue((*it)->key).c_str()));
        settings.setValue(QString(((*it)->name+"/secret").c_str()), QString(encryptValue((*it)->secret).c_str()));
        for(it2=(*it)->alerts.begin();it2!=(*it)->alerts.end();++it2)
        {
            settings.setValue(QString(((*it)->name+"/"+it2->name+"/active").c_str()),to_stringl(it2->active));
            settings.setValue(QString(((*it)->name+"/"+it2->name+"/bidabove").c_str()),to_string4(it2->bidabove));
            settings.setValue(QString(((*it)->name+"/"+it2->name+"/bidbelow").c_str()),to_string4(it2->bidbelow));
            settings.setValue(QString(((*it)->name+"/"+it2->name+"/askabove").c_str()),to_string4(it2->askabove));
            settings.setValue(QString(((*it)->name+"/"+it2->name+"/askbelow").c_str()),to_string4(it2->askbelow));
        }
    }
    settings.setValue("Bitkom/lang",currentLanguage);
    settings.setValue("Bitkom/encrypted",encrypted);
    settings.setValue("Bitkom/reverse",reverse);
    settings.setValue("Bitkom/passCheck",QString(encryptValue(randomString(30)).c_str()));
}

bool BaseData::checkWithdrawalPass(QString pass)
{
    string tmp;
    bool ret;

    tmp=this->pass;
    this->pass=pass.toStdString();
    ret=checkPass();
    this->pass=tmp;
    return(ret);
}

bool BaseData::checkPass()
{
    string s;
    QSettings settings("Bitmarket","Bitkom");
    unsigned int i;

    s=decryptValue(settings.value("Bitkom/passCheck").toString().toStdString());
    if (s.size()<30) return(false);
    for(i=0;i<s.size();i++)
    {
        if ((s[i]<'0')||(s[i]>'9')) return(false);
    }
    return(true);
}

bool BaseData::isEncrypted()
{
    return(encrypted);
}

bool BaseData::loadLastParallel()
{
    bool ret;

    mutex_list.lock();
    ret=currentExchange->lasttrades(cmarket);
    mutex_list.unlock();
    prepareChartData();
    emit(dataLoaded());
    return(ret);
}

void BaseData::loadLast()
{    
    QtConcurrent::run(this,&BaseData::loadLastParallel);
}

int BaseData::currentlang()
{
    return(currentLanguage);
}

void BaseData::changelang(QString name)
{
    if (name=="English") currentLanguage=ENGLISH;
    else if (name=="Polski") currentLanguage=POLISH;
    preparetrans();
    emit(model[MENU]->layoutChanged());
    emit(model[ORDER]->layoutChanged());
}

double BaseData::coma(QString d)
{
    int i;

    i=d.indexOf(',');
    if (i<0) return(d.toDouble());
    else
    {
        d[i]='.';
        return(d.toDouble());
    }
}

bool BaseData::execute(QString price,QString amount,bool type)
{
    return(currentExchange->tradepair(coma(price),coma(amount),type,cmarket));
}

bool BaseData::cancelOrder(QString id)
{
    return(currentExchange->cancelorder(id.toStdString()));
}

bool BaseData::getOpenOrders()
{
    bool ret;

    ret=currentExchange->getopenorders(cmarket);
    if (ret) emit(model[ORDERS]->layoutChanged());
    return(ret);
}

void BaseData::logError(QString s)
{
    log(s);
}

bool BaseData::getFunds()
{
    bool ret;

    ret=currentExchange->getfunds();
    if (ret)
    {
        emit(model[BALANCE]->dataChanged(model[BALANCE]->index(0,0),model[BALANCE]->index(model[BALANCE]->rowCount()-1,0)));
        emit(model[BID]->dataChanged(model[BID]->index(0,0),model[BID]->index(model[BID]->rowCount()-1,0)));
        emit(model[ASK]->dataChanged(model[ASK]->index(0,0),model[ASK]->index(model[ASK]->rowCount()-1,0)));
    }
    return(ret);
}

bool BaseData::login(QString pass)
{
    if (!currentExchange->logged) {
        this->pass=pass.toStdString();
        if ((encrypted)&&(!checkPass())) {
            currentExchange->lasterror=trans(63).toStdString();
            return(false);
        }
        loadkeys();
        return(getFunds());
    }
    else return(true);
}

bool BaseData::isLogged()
{
    return(currentExchange->logged);
}

QString BaseData::getFees()
{
    QString ret;

    ret="";
    if (isLogged())
    {
        ret+="Maker Fee: <font color=\"#41bb1a\">"+QString::number(currentExchange->fees.maker)+" %</font> ";
        ret+="Taker Fee: <font color=\"#41bb1a\">"+QString::number(currentExchange->fees.taker)+" %</font>\n";
    }
    return(ret);
}

QString BaseData::getMarketInfo(int k)
{
    QString ret;

    if (k==0) ret=to_string2(currentExchange->currentTicker.bid)+" "+getSecondCurrency()+"\n";
    else if (k==1) ret=to_string2(currentExchange->currentTicker.ask)+" "+getSecondCurrency()+"\n";
    else if (k==2) ret=to_string2(currentExchange->currentTicker.last)+" "+getSecondCurrency()+"\n";
    else if (k==3) ret=to_string2(currentExchange->currentTicker.high)+" "+getSecondCurrency()+"\n";
    else if (k==4) ret=to_string2(currentExchange->currentTicker.low)+" "+getSecondCurrency()+"\n";
    else if (k==5) ret=to_string2(currentExchange->currentTicker.volume)+" "+getFirstCurrency()+"\n";
    return(ret);
}

QString BaseData::buyPrice(int row)
{
    vector<Offer>::iterator it;
    int i,rowr;
    double sum;

    i=0;
    sum=0.0;
    rowr=currentExchange->currentTables->asks.size()-row-1;
    for(it=currentExchange->currentTables->asks.begin();it!=currentExchange->currentTables->asks.end();++it)
    {
        sum+=it->sum;
        if ((i==rowr)||(sum>currentExchange->balance.balance[currency(cmarket,SECOND)])) return(QString::number(it->price));
        i++;
    }
    return("");
}

QString BaseData::buyAmount(int row)
{
    vector<Offer>::iterator it;
    int i,rowr;
    double amount,sum,sum2;

    amount=0.0;
    sum=0.0;
    sum2=0.0;
    rowr=currentExchange->currentTables->asks.size()-row-1;
    i=0;
    for(it=currentExchange->currentTables->asks.begin();it!=currentExchange->currentTables->asks.end();++it)
    {
        sum+=it->sum;
        if ((i==rowr)||(sum>currentExchange->balance.balance[currency(cmarket,SECOND)])) break;
        sum2+=it->sum;
        amount+=it->amount;
        i++;
    }
    if (sum>currentExchange->balance.balance[currency(cmarket,SECOND)]) amount=amount+(currentExchange->balance.balance[currency(cmarket,SECOND)]-sum2)/it->price;
    return(to_stringd(amount));
}

QString BaseData::sellPrice(int row)
{
    vector<Offer>::iterator it;
    int i;
    double amount;

    amount=0.0;
    i=0;
    for(it=currentExchange->currentTables->bids.begin();it!=currentExchange->currentTables->bids.end();++it)
    {
        amount+=it->amount;
        if ((i==row)||(amount>currentExchange->balance.balance[currency(cmarket,FIRST)])) return(QString::number(it->price));
        i++;
    }
    return("");
}

QString BaseData::sellAmount(int row)
{
    vector<Offer>::iterator it;
    int i;
    double amount;

    amount=0.0;
    i=0;
    for(it=currentExchange->currentTables->bids.begin();it!=currentExchange->currentTables->bids.end();++it)
    {
        amount+=it->amount;
        if ((i==row)||(amount>currentExchange->balance.balance[currency(cmarket,FIRST)])) {
            if (amount>currentExchange->balance.balance[currency(cmarket,FIRST)]) amount=currentExchange->balance.balance[currency(cmarket,FIRST)];
            return(to_stringd(amount));
        }
        i++;
    }
    return("");
}

double BaseData::scalex()
{
    return(sx);
}

double BaseData::scaley()
{
    return(sy);
}

void BaseData::remove(const QUrl& url)
{
    QFile::remove(url.path());
}

double BaseData::getmaxaskamount()
{
    return(maxaskamount);
}

double BaseData::getmaxbidvalue()
{
    return(maxbidvalue);
}

void BaseData::computedepth(Tables &tables)
{
    vector<Offer>::iterator it;
    maxaskamount=0.0;
    maxbidvalue=0.0;

    for(it=tables.asks.begin();it!=tables.asks.end();++it)
    {
        if (maxaskamount<it->amount) maxaskamount=it->amount;
    }
    for(it=tables.bids.begin();it!=tables.bids.end();++it)
    {
        if (maxbidvalue<it->amount*it->price) maxbidvalue=it->amount*it->price;
    }
}

void BaseData::updateLast()
{
    emit(model[LASTTRADES]->layoutChanged());
}

bool BaseData::updateTables()
{
    bool ret;
    vector<Tables *>::iterator it;

    ret=false;
    mutex_list.lock();
    if (currentExchange->updatedTables.size()>0)
    {
        int oldRowAsk,rowAsk,oldRowBid,rowBid;

        oldRowAsk=currentExchange->currentTables->asks.size();
        oldRowBid=currentExchange->currentTables->bids.size();
        for(it=currentExchange->updatedTables.begin();it!=currentExchange->updatedTables.end();++it)
        {
            delete(currentExchange->currentTables);
            currentExchange->currentTables=*it;
        }
        rowAsk=currentExchange->currentTables->asks.size()-oldRowAsk;
        rowBid=currentExchange->currentTables->bids.size()-oldRowBid;
        currentExchange->updatedTables.clear();
        if (rowAsk>0) model[ASK]->insertRows(0,rowAsk-1);
        else if (rowAsk<0) model[ASK]->removeRows(0,-rowAsk-1);
        if (rowBid>0) model[BID]->insertRows(0,rowBid-1);
        else if (rowBid<0) model[BID]->removeRows(0,-rowBid-1);
        mutex_list.unlock();
        if (oldRowAsk==0) emit(model[ASK]->layoutChanged());
        else emit(model[ASK]->dataChanged(model[ASK]->index(0,0),model[ASK]->index(model[ASK]->rowCount()-1,0)));
        emit(model[BID]->dataChanged(model[BID]->index(0,0),model[BID]->index(model[BID]->rowCount()-1,0)));
        emit(model[BALANCE]->dataChanged(model[BALANCE]->index(0,0),model[BALANCE]->index(model[BALANCE]->rowCount()-1,0)));
    }else mutex_list.unlock();
    return(ret);
}

bool BaseData::getdepthparallel()
{
    Tables *table=new Tables();

    mutex_depth.lock();
    updated=currentExchange->getdepth(cmarket,*table);
    if (updated) computedepth(*table);
    if (!currentExchange->alerts[cmarketI].active) currentExchange->getticker(cmarket,currentExchange->currentTicker);
    mutex_depth.unlock();
    if (updated)
    {
        mutex_list.lock();
        currentExchange->updatedTables.push_back(table);
        mutex_list.unlock();
        if (!stopemit) emit(dataLoaded());
    }
    if (currentExchange->logged) currentExchange->getfunds();
    return(updated);
}

void BaseData::getdepth()
{
    QtConcurrent::run(this,&BaseData::getdepthparallel);
}

void BaseData::onCompleted()
{
    QMetaObject::invokeMethod(newDataObject, "refreshdata");
}

void BaseData::checkAlerts()
{
    vector<Exchange *>::iterator it;
    vector<Alert>::iterator it2;
    QString s;

    s="";
    for(it=exchanges.begin();it!=exchanges.end();++it)
    {
        for(it2=(*it)->alerts.begin();it2!=(*it)->alerts.end();++it2)
        {
            if (it2->active)
            {
                Ticker t;

                (*it)->getticker(it2->name,t);
                if (t.bid>it2->bidabove) s+=QString(currentExchange->marketsLong[cmarketI].c_str())+" "+trans(69)+" "+to_string4(it2->bidabove)+"\n";
                if (t.bid<it2->bidbelow) s+=QString(currentExchange->marketsLong[cmarketI].c_str())+" "+trans(72)+" "+to_string4(it2->bidbelow)+"\n";
                if (t.ask>it2->askabove) s+=QString(currentExchange->marketsLong[cmarketI].c_str())+" "+trans(72)+" "+to_string4(it2->askabove)+"\n";
                if (t.ask<it2->askbelow) s+=QString(currentExchange->marketsLong[cmarketI].c_str())+" "+trans(72)+" "+to_string4(it2->askbelow)+"\n";
                if ((*it==currentExchange)&&(it2->name==cmarket)) currentExchange->currentTicker=t;
            }
        }
    }
    if (s.size()>0) makeAlert(s);
}

void BaseData::getdepthimmediate()
{
    vector<Tables *>::iterator it;

    stopemit=true;
    getdepthparallel();
    mutex_list.lock();
    for(it=currentExchange->updatedTables.begin();it!=currentExchange->updatedTables.end();++it)
    {
        delete(currentExchange->currentTables);
        currentExchange->currentTables=*it;
    }
    currentExchange->updatedTables.clear();
    if (currentExchange->logged) getFunds();
    stopemit=false;
    mutex_list.unlock();
    emit(model[BID]->layoutChanged());
    emit(model[ASK]->layoutChanged());
    emit(model[BALANCE]->layoutChanged());
}

void BaseData::changeEx(QString s)
{
    vector<Exchange *>::iterator it;

    for(it=exchanges.begin();it!=exchanges.end();++it)
    {
        if (QString((*it)->name.c_str())==s)
        {
            currentExchange=*it;
            emit(model[MARKETS]->layoutChanged());
            cmarket=currentExchange->markets.front();
            //getdepth();
            getdepthimmediate();
            //qDebug()<<s<<"\n";
            break;
        }
    }
}

void BaseData::changeMarket(QString s)
{
    vector<string>::iterator it,it2;
    int i,j;

    i=0;
    for(it=currentExchange->marketsLong.begin();it!=currentExchange->marketsLong.end();++it)
    {
        if (QString((*it).c_str())==s)
        {
            j=0;
            for(it2=currentExchange->markets.begin();it2!=currentExchange->markets.end();++it2)
            {
                if (j==i) break;
                j++;
            }
            cmarket=*it2;
            cmarketI=j;
            getdepthimmediate();
            break;
        }
        i++;
    }
}

QString BaseData::getlangfile()
{
    if (currentLanguage==ENGLISH) return(":/translations/english");
    else if (currentLanguage==POLISH) return(":/translations/polish");
    else return("");
}

void BaseData::preparetrans()
{
    int i;
    QFile file(getlangfile());

    i=0;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd()) {
            translations[i]= in.readLine();
            i++;
        }
    }
    file.close();
}

QString BaseData::trans(int i)
{
    return(translations[i]);
}

void BaseData::makeAlert(QString s)
{
#ifdef ANDROID
    log("notify: "+s);
    QAndroidJniObject javas = QAndroidJniObject::fromString(trans(75));
    QAndroidJniObject javatxt = QAndroidJniObject::fromString(s);
    QAndroidJniObject::callStaticMethod<void>("com/bitkom/NotificationClient","notify","(Ljava/lang/String;Ljava/lang/String;)V",javas.object<jstring>(),javatxt.object<jstring>());
#else
    QVariant ret;

    QMetaObject::invokeMethod(msgObject, "displayMessage",Q_RETURN_ARG(QVariant, ret),Q_ARG(QVariant, s));
#endif
}

void BaseData::scanner()
{
#ifdef ANDROID
    QAndroidJniObject::callStaticMethod<void>("com/bitkom/NotificationClient","scanner","()V");
#endif
}

QString BaseData::getscannedtxt()
{
#ifdef ANDROID
    QAndroidJniObject result=QAndroidJniObject::callStaticObjectMethod<jstring>("com/bitkom/NotificationClient","getscannedtxt");
    log("scannertxt: "+result.toString()+"\n");
    return(result.toString());
#endif
    log("scannertxt\n");
    return("");
}


