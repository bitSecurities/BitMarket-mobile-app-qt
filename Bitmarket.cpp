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

#include <stddef.h>
#include "Bitmarket.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include "basedata.h"

extern BaseData *base;

#ifdef LIVESERVER
#define BITMARKETURL string("https://www.bitmarket.pl/")
#else
#endif

Bitmarket::Bitmarket() {
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_CAPATH, "/system/etc/security/cacerts");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_USERPWD, "siteuser:123enterhere");
    bid=0.0;
    ask=0.0;
    logged=false;
    name="Bitmarket";
    id=BITMARKET;
    profit=0.0;
    profitPercentage=0.0;
    value=0.0;
    currentTables=new Tables();    
    Currency c;
    c.name="PLN";
    c.type=PLN;
    currencies.push_back(c);
    c.name="EUR";
    c.type=EUR;
    currencies.push_back(c);
    c.name="BTC";
    c.type=BTC;
    currencies.push_back(c);
    c.name="LTC";
    c.type=LTC;
    currencies.push_back(c);
    key="";
    secret="";
}

Bitmarket::~Bitmarket() {
    curl_easy_cleanup(curl);
    if(chunk.memory) free(chunk.memory);
}

void Bitmarket::fetchPosition(struct json_object *jtmp2)
{
    struct json_object *jtmp3;
    Position p;
    string t;

    json_object_object_get_ex(jtmp2,"id",&jtmp3);
    p.id=json_object_get_string(jtmp3);
    json_object_object_get_ex(jtmp2,"type",&jtmp3);
    t=json_object_get_string(jtmp3);
    if (t=="long") p.type=BUY;
    else if (t=="short") p.type=SELL;
    json_object_object_get_ex(jtmp2,"time",&jtmp3);
    p.time=json_object_get_int64(jtmp3);
    json_object_object_get_ex(jtmp2,"leverage",&jtmp3);
    p.leverage=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"fiatTotal",&jtmp3);
    p.fiatTotal=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"fiatOpened",&jtmp3);
    p.fiatOpened=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"fiatClosed",&jtmp3);
    p.fiatClosed=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"security",&jtmp3);
    p.security=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rate",&jtmp3);
    p.price=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rateOpen",&jtmp3);
    p.rateOpen=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rateClose",&jtmp3);
    p.rateClose=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rateProfit",&jtmp3);
    p.rateProfit=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rateLoss",&jtmp3);
    p.rateLoss=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"rateCurrent",&jtmp3);
    p.rateCurrent=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"fees",&jtmp3);
    p.fees=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"profit",&jtmp3);
    p.profit=json_object_get_double(jtmp3);
    json_object_object_get_ex(jtmp2,"profitPercentage",&jtmp3);
    p.profitPercentage=json_object_get_double(jtmp3);
    positions.push_back(p);
}

string Bitmarket::signature(const string& post) const
{
    std::vector<unsigned char> data(post.begin(), post.end());
    std::vector<uint8_t> bsecret (secret.c_str(), secret.c_str()+secret.size()/sizeof(uint8_t));
    vector<unsigned char> v;
    string s;

    v=hmac_sha512(data, bsecret);
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill( '0' );
    std::for_each( v.cbegin(), v.cend(), [&]( int c ) { ss << std::setw( 2 ) << c; } );
    s=ss.str();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return(s);
}

long Bitmarket::api(string post,string method)
{
    struct curl_slist *headers=NULL;
    string hed;
    long nonce;

    nonce=time(NULL);
    post="tonce="+to_stringl(nonce).toStdString()+"&method="+method+"&"+post;
    chunk.size=0;
    curl_easy_setopt(curl, CURLOPT_URL, (BITMARKETURL+"api2/").c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:40.0) Gecko/20100101 Firefox/40.0");
    hed="API-Key: "+string(key);
    headers = curl_slist_append(headers, hed.c_str());
    hed="API-Hash: "+signature(post);
    headers = curl_slist_append(headers, hed.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curlcode=curl_easy_perform(curl);
    if (chunk.size<=0) log("Error connecting to bitmarket\n");
    curl_slist_free_all(headers);
    return((long)chunk.size);
}

bool Bitmarket::makeApiCall(string method,struct json_object **json,string post)
{
    log(method+": "+post+"\n");
    if (api(post,method)>0) {
        if ((long)(*json=json_tokener_parse(chunk.memory))==0)
        {
            lasterror="Not a json file";
            logerror();
            log(string(chunk.memory)+"\n");        
            return(false);
        }
#ifdef DEBUG
        log(string(chunk.memory)+"\n");
#endif           
        return(true);
    }
    else {
        lasterror=name+" "+method+": "+curl_easy_strerror(curlcode);
        logerror();          
        return(false);
    }
}

void Bitmarket::getError(struct json_object **json)
{
    struct json_object *jtmp;

    json_object_object_get_ex(*json,"errorMsg",&jtmp);
    lasterror=json_object_get_string(jtmp);
    logerror();
    json_object_put(*json);
}

bool Bitmarket::fetchData(struct json_object **json,struct json_object **jtmp)
{
    if (!json_object_object_get_ex(*json,"data",jtmp))
    {
        getError(json);
        curl_mutex.unlock();
        return(false);
    }
    return(true);
}

bool Bitmarket::makeSimpleApiCall(string method,string post)
{
    struct json_object *json,*jtmp;

    curl_mutex.lock();
    if (makeApiCall(method,&json,post))
    {
        if(!fetchData(&json,&jtmp)) return(false);
        log(string(chunk.memory)+"\n");
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::getTickerData(string url,struct json_object **json)
{
    chunk.size=0;
#ifdef DEBUG
    log(string(BITMARKETURL+"json/")+url+"\n");
#endif
    curl_easy_setopt(curl, CURLOPT_URL, (string(BITMARKETURL+"json/")+url).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, 0);
    curlcode=curl_easy_perform(curl);
    if (chunk.size>0)
    {
        if ((long)(*json=json_tokener_parse(chunk.memory))==0)
        {
            log("Not a json file: "+string(BITMARKETURL+"json/")+url+"\n");
            log(string(chunk.memory)+"\n");
            return(false);
        }
        return(true);
    }
    else
    {
        lasterror=name+" "+url+": "+curl_easy_strerror(curlcode);
        logerror();
        return(false);
    }
}

bool Bitmarket::marginList(string market)
{
    struct json_object *json,*jtmp,*jtmp2,*jtmp3;
    struct array_list *arr=0;
    int i;

    curl_mutex.lock();
    positions.clear();
    if (makeApiCall("marginList",&json,"market="+market))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        log(string(chunk.memory)+"\n");
        json_object_object_get_ex(jtmp,"performance",&jtmp2);
        json_object_object_get_ex(jtmp2,"available",&jtmp3);
        balance.balance[LBTC]=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp2,"blocked",&jtmp3);
        balance.blocked[LBTC]=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp2,"profit",&jtmp3);
        profit=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp2,"profitPercentage",&jtmp3);
        profitPercentage=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp2,"value",&jtmp3);
        value=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp,"short",&jtmp2);
        arr=json_object_get_array(jtmp2);
        for(i=0;i<arr->length;i++) fetchPosition((struct json_object *)array_list_get_idx(arr,i));
        json_object_object_get_ex(jtmp,"long",&jtmp2);
        arr=json_object_get_array(jtmp2);
        for(i=0;i<arr->length;i++) fetchPosition((struct json_object *)array_list_get_idx(arr,i));
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::marketChart(string market,long,long,long interval)
{
    struct json_object *json,*jtmp2,*jtmp3;
    struct array_list *arr=0;
    int i;
    string timeframe;

    curl_mutex.lock();
    if (interval==60) timeframe="90m";
    else if (interval==240) timeframe="6h";
    else if (interval==960) timeframe="1d";
    else if (interval==7200) timeframe="7d";
    else if (interval==28800) timeframe="1m";
    else if (interval==86400) timeframe="3m";
    else if (interval==172800) timeframe="6m";
    else if (interval==345600) timeframe="1y";
    chunk.size=0;
    curl_easy_setopt(curl, CURLOPT_URL, (string(BITMARKETURL)+"/graphs/"+market+"/"+timeframe+".json?t="+to_stringl(rand()).toStdString()).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, 0);
    curlcode=curl_easy_perform(curl);
    if (chunk.size>0)
    {
        if ((long)(json=json_tokener_parse(chunk.memory))==0)
        {
            log("Not a json file\n");
            log(string(chunk.memory)+"\n");
            curl_mutex.unlock();
            return(false);
        }
        //log(string(chunk.memory)+"\n");
        arr=json_object_get_array(json);
        for(i=0;i<arr->length;i++)
        {
            Ohlc s;

            jtmp2=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp2,"time",&jtmp3);
            s.t=json_object_get_int64(jtmp3);
            json_object_object_get_ex(jtmp2,"open",&jtmp3);
            s.o=json_object_get_double(jtmp3);
            json_object_object_get_ex(jtmp2,"close",&jtmp3);
            s.c=json_object_get_double(jtmp3);
            json_object_object_get_ex(jtmp2,"high",&jtmp3);
            s.h=json_object_get_double(jtmp3);
            json_object_object_get_ex(jtmp2,"low",&jtmp3);
            s.l=json_object_get_double(jtmp3);
            base->swieczki.push_back(s);
        }
        std::reverse(base->swieczki.begin(), base->swieczki.end());
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    else
    {
        lasterror=name+" marketChart: "+curl_easy_strerror(curlcode);
        logerror();
        curl_mutex.unlock();
        return(false);
    }

}

bool Bitmarket::marginClose(string market,string id,double amount)
{    
    return(makeSimpleApiCall("marginClose","market="+market+"&id="+id+"&amount="+to_stringd(amount).toStdString()));
}

bool Bitmarket::marginCancel(string market,string id,double amount)
{
    return(makeSimpleApiCall("marginCancel","market="+market+"&id="+id+"&amount="+to_stringd(amount).toStdString()));
}

bool Bitmarket::marginModify(string market,string id,double rate,double rateProfit,double rateLoss)
{
    return(makeSimpleApiCall("marginModify","market="+market+"&id="+id+"&rate="+to_stringp(rate).toStdString()+"&rateLoss="+to_stringp(rateLoss).toStdString()+"&rateProfit="+to_stringp(rateProfit).toStdString()));
}

bool Bitmarket::marginOpen(string market,char type,double leverage,double amount,double price,double rateProfit,double rateLoss)
{
    string b;

    if (type==BUY) b="long";
    else if (type==SELL) b="short";
    return(makeSimpleApiCall("marginOpen","market="+market+"&type="+b+"&rate="+to_stringp(price).toStdString()+"&amount="+to_stringd(amount).toStdString()+"&leverage="+to_string2(leverage).toStdString()+"&rateLoss="+to_stringp(rateLoss).toStdString()+"&rateProfit="+to_stringp(rateProfit).toStdString()));
}

bool Bitmarket::swapOpen(string currency,double rate,double amount)
{
    return(makeSimpleApiCall("swapOpen","currency="+currency+"&amount="+to_stringd(amount).toStdString()+"&rate="+to_stringp(rate).toStdString()));
}

bool Bitmarket::swapList(string currency)
{
    struct json_object *json,*jtmp,*jtmp2,*jtmp3;
    struct array_list *arr=0;
    int i;

    curl_mutex.lock();
    swaps.clear();
    if (makeApiCall("swapList",&json,"currency="+currency))
    {
        log(chunk.memory);
        if (!fetchData(&json,&jtmp)) return(false);
        arr=json_object_get_array(jtmp);
        for(i=0;i<arr->length;i++)
        {
            Swap s;

            jtmp2=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp2,"id",&jtmp3);
            s.id=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"amount",&jtmp3);
            s.amount=json_object_get_double(jtmp3);
            json_object_object_get_ex(jtmp2,"rate",&jtmp3);
            s.rate=json_object_get_double(jtmp3);
            json_object_object_get_ex(jtmp2,"earnings",&jtmp3);
            s.earnings=json_object_get_double(jtmp3);
            swaps.push_back(s);
        }
        //log((QString("aaa:")+QString::number(swaps.size())+"\n").toStdString());
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
    //    return(makeSimpleApiCall("swapList","currency="+currency));
}

bool Bitmarket::history(string currency)
{
    struct json_object *json,*jtmp,*jtmp2;
    struct array_list *arr=0;
    int i;

    curl_mutex.lock();
    historyTable.clear();
    if (makeApiCall("history",&json,"currency="+currency))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(jtmp,"results",&jtmp2);
        arr=json_object_get_array(jtmp2);
        for(i=0;i<arr->length;i++)
        {
            struct json_object *jtmp3,*jtmp4;
            History h;

            jtmp3=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp3,"id",&jtmp4);
            h.id=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"amount",&jtmp4);
            h.amount=json_object_get_double(jtmp4);
            json_object_object_get_ex(jtmp3,"currency",&jtmp4);
            h.currency=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"type",&jtmp4);
            h.type=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"time",&jtmp4);
            h.time=json_object_get_int64(jtmp4);
            json_object_object_get_ex(jtmp3,"rate",&jtmp4);
            h.rate=json_object_get_double(jtmp4);
            json_object_object_get_ex(jtmp3,"commission",&jtmp4);
            h.commission=json_object_get_double(jtmp4);
            historyTable.push_back(h);
        }
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::swapClose(string currency,string id)
{
    return(makeSimpleApiCall("swapClose","currency="+currency+"&id="+id));
}

bool Bitmarket::withdraw(double amount,const string& currency,const string& address,const string& swift,const string& note,bool test,bool fast,double& fee)
{
    string t;
    string method,post;
    struct json_object *json,*jtmp,*jtmp2;

    if (test) t="y";
    else t="n";
    if (currency=="PLN")
    {
        if (fast) {
            method="withdrawFiatFast";
            post="currency="+currency+"&amount="+to_stringd(amount).toStdString()+"&account="+address+"&account2="+swift+"&test_only="+t+"&withdrawal_note="+note;
        }
        else {
            method="withdrawFiat";
            post="currency="+currency+"&amount="+to_stringd(amount).toStdString()+"&account="+address+"&account2="+swift+"&test_only="+t+"&withdrawal_note="+note;
        }
    }else if (currency=="EUR") {
        method="withdrawFiat";
        post="currency="+currency+"&amount="+to_stringd(amount).toStdString()+"&account="+address+"&account2="+swift+"&test_only="+t+"&withdrawal_note="+note;

    }else {
        method="withdraw";
        post="currency="+currency+"&amount="+to_stringd(amount).toStdString()+"&address="+address;
    }
    curl_mutex.lock();
    if (makeApiCall(method,&json,post))
    {
        if(!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(jtmp,"fee",&jtmp2);
        if (jtmp2!=NULL) fee=json_object_get_double(jtmp2);
        else fee=0.0;
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::deposit(const string& currency)
{
    struct json_object *json,*jtmp;
    vector<Deposit>::iterator it;
    Deposit d;

    for(it=deposits.begin();it!=deposits.end();++it)
    {
        if (it->currency==currency) {
            currentDeposit=*it;
            return(true);
        }
    }
    currentDeposit=d;
    curl_mutex.lock();
    if (makeApiCall("deposit",&json,"currency="+currency))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        d.acc_num=json_object_get_string(jtmp);
        d.currency=currency;
        deposits.push_back(d);
        currentDeposit=d;
        curl_mutex.unlock();
        json_object_put(json);
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::lasttrades(string market)
{
    struct json_object *json,*jtmp,*jtmp2;
    struct array_list *arr;
    int i;

    curl_mutex.lock();
    trades.clear();
    if (getTickerData(market+"/trades.json",&json))
    {
        //log("parsing\n");
        arr=json_object_get_array(json);
        for(i=0;i<arr->length;i++)
        {
            Offer offer;
            QString s;

            jtmp=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp,"price",&jtmp2);
            offer.price=json_object_get_double(jtmp2);
            json_object_object_get_ex(jtmp,"amount",&jtmp2);
            offer.amount=json_object_get_double(jtmp2);
            json_object_object_get_ex(jtmp,"tid",&jtmp2);
            offer.id=QString::number(json_object_get_double(jtmp2)).toStdString();
            json_object_object_get_ex(jtmp,"type",&jtmp2);
            s=json_object_get_string(jtmp2);
            if (s=="bid") offer.type=BID;
            else offer.type=ASK;
            json_object_object_get_ex(jtmp,"date",&jtmp2);
            offer.time=json_object_get_int64(jtmp2);
            trades.push_back(offer);
        }
        json_object_put(json);
        //log("completed\n");
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::getdepth(string market,Tables& tables)
{
    struct json_object *json,*jtmp,*jtmp2;
    struct array_list *arr,*arr2;
    int i;
    double sum;

    curl_mutex.lock();
    tables.bids.clear();
    tables.asks.clear();
    if (getTickerData(market+"/orderbook.json",&json))
    {
        json_object_object_get_ex(json,"bids",&jtmp);
        arr=json_object_get_array(jtmp);
        sum=0.0;
        for(i=0;i<arr->length;i++)
        {
            Offer offer;
            jtmp2=(struct json_object *)array_list_get_idx(arr,i);
            arr2=json_object_get_array(jtmp2);
            offer.type=BID;
            offer.price=json_object_get_double((struct json_object *)array_list_get_idx(arr2,0));
            offer.amount=json_object_get_double((struct json_object *)array_list_get_idx(arr2,1));
            offer.sum=sum;
            sum+=offer.amount;
            tables.bids.push_back(offer);
        }
        json_object_object_get_ex(json,"asks",&jtmp);
        arr=json_object_get_array(jtmp);
        sum=0.0;
        for(i=0;i<arr->length;i++)
        {
            Offer offer;
            jtmp2=(struct json_object *)array_list_get_idx(arr,i);
            arr2=json_object_get_array(jtmp2);
            offer.type=ASK;
            offer.price=json_object_get_double((struct json_object *)array_list_get_idx(arr2,0));
            offer.amount=json_object_get_double((struct json_object *)array_list_get_idx(arr2,1));
            offer.sumLower=sum;
            sum+=offer.amount*offer.price;
            tables.asks.push_back(offer);
        }
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::getfunds()
{
    struct json_object *json,*jtmp,*jtmp2,*jtmp3,*jtmp4;

    curl_mutex.lock();
    if (makeApiCall("info",&json,""))
    {
        Deposit d;

        log(string(chunk.memory)+"\n");
        if(!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(jtmp,"balances",&jtmp2);
        json_object_object_get_ex(jtmp2,"available",&jtmp3);
        json_object_object_get_ex(jtmp3,"PLN",&jtmp4);
        balance.balance[PLN]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"BTC",&jtmp4);
        balance.balance[BTC]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"LTC",&jtmp4);
        balance.balance[LTC]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"EUR",&jtmp4);
        balance.balance[EUR]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp2,"blocked",&jtmp3);
        json_object_object_get_ex(jtmp3,"PLN",&jtmp4);
        balance.blocked[PLN]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"BTC",&jtmp4);
        balance.blocked[BTC]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"LTC",&jtmp4);
        balance.blocked[LTC]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp3,"EUR",&jtmp4);
        balance.blocked[EUR]=json_object_get_double(jtmp4);
        json_object_object_get_ex(jtmp,"account",&jtmp2);
        json_object_object_get_ex(jtmp2,"commissionMaker",&jtmp3);
        fees.maker=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp2,"commissionTaker",&jtmp3);
        fees.taker=json_object_get_double(jtmp3);
        json_object_object_get_ex(jtmp,"bank_deposit_fiat",&jtmp2);
        if (jtmp2!=NULL)
        {
            deposits.clear();
            d.currency="PLN";
            json_object_object_get_ex(jtmp2,"PLN",&jtmp3);
            json_object_object_get_ex(jtmp3,"bank_name",&jtmp4);
            d.bank_name=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"pay_to",&jtmp4);
            d.pay_to=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"acc_num",&jtmp4);
            d.acc_num=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"transfer_title",&jtmp4);
            d.transfer_title=json_object_get_string(jtmp4);
            d.swift_code="";
            deposits.push_back(d);
            d.currency="EUR";
            json_object_object_get_ex(jtmp2,"EUR",&jtmp3);
            json_object_object_get_ex(jtmp3,"bank_name",&jtmp4);
            d.bank_name=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"pay_to",&jtmp4);
            d.pay_to=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"acc_num",&jtmp4);
            d.acc_num=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"swift_code",&jtmp4);
            d.swift_code=json_object_get_string(jtmp4);
            json_object_object_get_ex(jtmp3,"transfer_title",&jtmp4);
            d.transfer_title=json_object_get_string(jtmp4);
            deposits.push_back(d);
        }
        json_object_put(json);
        logged=true;
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::getticker(string market,Ticker& t)
{
    struct json_object *json,*jtmp;

    curl_mutex.lock();
    if (getTickerData(market+"/ticker.json",&json))
    {
        json_object_object_get_ex(json,"ask",&jtmp);
        t.ask=json_object_get_double(jtmp);
        json_object_object_get_ex(json,"bid",&jtmp);
        t.bid=json_object_get_double(jtmp);
        json_object_object_get_ex(json,"last",&jtmp);
        t.last=json_object_get_double(jtmp);
        json_object_object_get_ex(json,"high",&jtmp);
        t.high=json_object_get_double(jtmp);
        json_object_object_get_ex(json,"low",&jtmp);
        t.low=json_object_get_double(jtmp);
        json_object_object_get_ex(json,"volume",&jtmp);
        t.volume=json_object_get_double(jtmp);
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::cancelall()
{
    vector<Offer>::iterator it;
    bool ret;

    ret=true;
    for(it=orders.begin();it!=orders.end();it++)
    {
        ret=ret&cancelorder((*it).id);
    }
    return(true);
}

bool Bitmarket::tradepair(double price,double amount,char type,string market)
{
    string b;
    struct json_object *json,*jtmp;

#ifdef ARMED
    if (type==BUY) b="buy";
    else if (type==SELL) b="sell";

    curl_mutex.lock();
    if (makeApiCall("trade",&json,"market="+market+"&type="+b+"&rate="+to_stringp(price).toStdString()+"&amount="+to_stringd(amount).toStdString()))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
#else
    return(true);
#endif
}

bool Bitmarket::getopenorders(string market)
{
    struct json_object *json,*jtmp,*jtmp2,*jtmp3,*jtmp4,*jtmp5,*jtmp6;
    struct array_list *arr=0;
    int i;

    curl_mutex.lock();
    orders.clear();
    if (makeApiCall("orders",&json,"market="+market))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(json,"data",&jtmp);
        json_object_object_get_ex(jtmp,"buy",&jtmp2);
        if (jtmp2!=NULL) arr=json_object_get_array(jtmp2);
        if (arr!=NULL)
        {
            for(i=0;i<arr->length;i++)
            {
                Offer o;
                jtmp3=(struct json_object *)array_list_get_idx(arr,i);
                json_object_object_get_ex(jtmp3,"id",&jtmp4);
                o.id=to_stringl(json_object_get_int64(jtmp4)).toStdString();
                json_object_object_get_ex(jtmp3,"amount",&jtmp5);
                o.amount=json_object_get_double(jtmp5);
                json_object_object_get_ex(jtmp3,"rate",&jtmp6);
                o.price=json_object_get_double(jtmp6);
                o.type=BUY;
                orders.push_back(o);
            }
        }else
        {
            json_object_put(json);
            curl_mutex.unlock();
            return(false);
        }
        json_object_object_get_ex(jtmp,"sell",&jtmp2);
        if (jtmp2!=NULL) arr=json_object_get_array(jtmp2);
        if (arr!=NULL)
        {
            for(i=0;i<arr->length;i++)
            {
                Offer o;
                jtmp3=(struct json_object *)array_list_get_idx(arr,i);
                json_object_object_get_ex(jtmp3,"id",&jtmp4);
                o.id=to_stringl(json_object_get_int64(jtmp4)).toStdString();
                json_object_object_get_ex(jtmp3,"amount",&jtmp5);
                o.amount=json_object_get_double(jtmp5);
                json_object_object_get_ex(jtmp3,"rate",&jtmp6);
                o.price=json_object_get_double(jtmp6);
                o.type=SELL;
                orders.push_back(o);
            }
        }else
        {
            json_object_put(json);
            curl_mutex.unlock();
            return(false);
        }
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmarket::cancelorder(string id)
{
    return(makeSimpleApiCall("cancel","id="+id));
}

bool Bitmarket::marginBalanceAdd(string market,double amount)
{
    return(makeSimpleApiCall("marginBalanceAdd","market="+market+"&amount="+to_stringd(amount).toStdString()));
}

bool Bitmarket::marginBalanceRemove(string market,double amount)
{
    return(makeSimpleApiCall("marginBalanceRemove","market="+market+"&amount="+to_stringd(amount).toStdString()));
}
