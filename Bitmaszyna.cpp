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

#include "Bitmaszyna.h"
#include "util.h"
#include "basedata.h"

const string Bitmaszyna::strMessageMagic="Bitmaszyna.pl API:\n";
extern void log(string s);
extern BaseData *base;

#define BITMASZYNAURL "https://bitmaszyna.pl/api/"
//#define BITMASZYNAURL "https://192.168.1.100:10443/api/"

#ifdef IPHONE
void BN_zero_ex(BIGNUM *)
{

}

#endif

Bitmaszyna::Bitmaszyna() {
    bb=false;
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_URL, BITMASZYNAURL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
    name="Bitmaszyna";
    logged=false;
    id=BITMASZYNA;
    profit=0;
    profitPercentage=0;
    value=0;
    currentTables=new Tables();
    Currency c;
    c.name="PLN";
    c.type=PLN;
    c.deposit=true;
    currencies.push_back(c);
    c.name="BTC";
    c.type=BTC;
    c.deposit=true;
    currencies.push_back(c);
    c.name="LTC";
    c.type=LTC;
    c.deposit=true;
    currencies.push_back(c);
    c.name="DOGE";
    c.type=DOGE;
    c.deposit=true;
    currencies.push_back(c);
    c.name="LSK";
    c.type=LSK;
    c.deposit=true;
    currencies.push_back(c);
    c.name="KBM";
    c.type=KBM;
    c.deposit=false;
    currencies.push_back(c);
    key="";
    secret="";
    withdrawalcurrency=BTC;
}

string Bitmaszyna::sign(string& privkey,string& msg) {
    v=DecodeBase64(privkey.c_str(),&bb);
    if (bb)
    {
        lasterror="Invalid secret";
        logerror();
        return("");
    }else if (v.size()!=32)
    {
        lasterror="Secret too short";
        logerror();
        return("");
    }
    ckey.SetSecret(v);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,strMessageMagic.c_str(),strMessageMagic.size());
    SHA256_Update(&ctx,msg.c_str(),msg.size());
    SHA256_Final((unsigned char *)&hash1,&ctx);
    SHA256((unsigned char *)&hash1,sizeof(hash1),(unsigned char *)&hash2);
    if (!ckey.SignCompact(hash2, vsig)) {
        lasterror="Sign failed";
        logerror();
        return("");
    }
    signature=base64_encode(&vsig[0],vsig.size());
    return(signature);
}

int Bitmaszyna::api(string post,string method)
{
    struct curl_slist *headers=NULL;
    string privkey,hed;
    stringstream ss;

    chunk.size=0;
    privkey=secret;
    curl_easy_setopt(curl, CURLOPT_POST,1);
    curl_easy_setopt(curl, CURLOPT_URL, (BITMASZYNAURL+method).c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
    hed=string("Rest-Key: ")+key;
    headers = curl_slist_append(headers, hed.c_str());
    hed=string("Rest-Sign: ")+sign(privkey,post);
    headers = curl_slist_append(headers, hed.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_perform(curl);
    if (chunk.size<=0) printf("Error connecting to bitmaszyna.pl %s\n",(BITMASZYNAURL+method).c_str());
    //else log(string(chunk.memory)+"\n");
    curl_slist_free_all(headers);
    //log(":::"+to_string(chunk.size)+"\n");
    return(chunk.size);
}

bool Bitmaszyna::makeApiCall(string method,struct json_object **json,string post)
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

void Bitmaszyna::getError(struct json_object **json)
{
    string err;

    err=json_object_get_string(*json);
    if (err!="") lasterror=err;
    logerror();
    json_object_put(*json);
}

bool Bitmaszyna::fetchData(struct json_object **json,struct json_object **jtmp)
{
    json_object_object_get_ex(*json,"result",jtmp);
    if ((*jtmp!=NULL)&&(string(json_object_get_string(*jtmp))!=string("ok"))) {
        json_object_object_get_ex(*json,"reason",jtmp);
        getError(jtmp);
        curl_mutex.unlock();
        return(false);
    }
    return(true);
}

bool Bitmaszyna::makeSimpleApiCall(string method,string post)
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

bool Bitmaszyna::getTickerData(string url,struct json_object **json)
{
    chunk.size=0;
    curl_easy_setopt(curl, CURLOPT_URL, (BITMASZYNAURL+url).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, 0);
#ifdef DEBUG
    log((BITMASZYNAURL+url)+"\n");
#endif
    curlcode=curl_easy_perform(curl);
    if (chunk.size>0)
    {
        if ((long)(*json=json_tokener_parse(chunk.memory))==0)
        {
            log("Not a json file\n");
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

string Bitmaszyna::strfromtype(char type)
{
    if (type==BUY) return("buypair");
    else if (type==SELL) return("sellpair");
    else return("");
}

Bitmaszyna::~Bitmaszyna() {

}

bool Bitmaszyna::marketChart(string market,long starttime,long endtime,long interval)
{
    struct json_object *json,*jtmp2,*jtmp3;
    struct array_list *arr;
    int i;

    curl_mutex.lock();
    if (getTickerData(market+"/chartData.json?starttime="+to_stringl(starttime).toStdString()+"&endtime="+to_stringl(endtime).toStdString()+"&interval="+to_stringl(interval).toStdString(),&json))
    {
        base->swieczki.clear();
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
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmaszyna::swapOpen(string currency,double amount,double rate)
{
    Q_UNUSED(currency);
    Q_UNUSED(amount);
    Q_UNUSED(rate);
    return(true);
}

bool Bitmaszyna::swapList(string currency)
{
    Q_UNUSED(currency);
    return(true);
}

bool Bitmaszyna::swapClose(string,string id)
{
    Q_UNUSED(id);
    return(true);
}

bool Bitmaszyna::marginList(string market)
{
    Q_UNUSED(market);
    return(true);
}

bool Bitmaszyna::history(string currency)
{
    Q_UNUSED(currency);
    return(true);
}

bool Bitmaszyna::transfers(int count,int start)
{
    Q_UNUSED(count);
    Q_UNUSED(start);
    return(true);
}

bool Bitmaszyna::withdrawals(int count,int start)
{
    Q_UNUSED(count);
    Q_UNUSED(start);
    return(true);
}

bool Bitmaszyna::transfer(string,string,double)
{
    return(true);
}

bool Bitmaszyna::marginOpen(string market,char type,double leverage,double amount,double rate,double rateLoss,double rateProfit)
{
    Q_UNUSED(market);
    Q_UNUSED(type);
    Q_UNUSED(leverage);
    Q_UNUSED(amount);
    Q_UNUSED(rate);
    Q_UNUSED(rateLoss);
    Q_UNUSED(rateProfit);
    return(true);
}

bool Bitmaszyna::marginClose(string market,string id,double amount)
{
    Q_UNUSED(market);
    Q_UNUSED(id);
    Q_UNUSED(amount);
    return(true);
}

bool Bitmaszyna::marginCancel(string market,string id,double amount)
{
    Q_UNUSED(market);
    Q_UNUSED(id);
    Q_UNUSED(amount);
    return(true);
}

bool Bitmaszyna::marginModify(string market,string id,double rate,double rateLoss,double rateProfit)
{
    Q_UNUSED(market);
    Q_UNUSED(id);
    Q_UNUSED(rate);
    Q_UNUSED(rateLoss);
    Q_UNUSED(rateProfit);
    return(true);
}

bool Bitmaszyna::getticker(string market,Ticker& t)
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
        json_object_object_get_ex(json,"volume1",&jtmp);
        t.volume=json_object_get_double(jtmp);
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmaszyna::withdraw(double amount,const string& currency,const string& address,const string& swift,const string& note,bool test,int type,double& fee)
{
    Q_UNUSED(swift);
    Q_UNUSED(note);
    Q_UNUSED(test);
    Q_UNUSED(fee);
    string ex;

    if (test) return(true);
    if (type==WITHDRAW_FAST) ex="true";
    else ex="false";
    return(makeSimpleApiCall("withdrawal",string("nonce=")+to_stringl(getctime()).toStdString()+"&account="+address+"&amount="+to_stringd(amount).toStdString()+"&cur="+currency+"&express="+ex));
}

bool Bitmaszyna::deposit(const string&)
{
    return(false);
}

bool Bitmaszyna::lasttrades(string market)
{    
    struct json_object *json,*jtmp,*jtmp2;
    struct array_list *arr;
    int i;

    curl_mutex.lock();
    trades.clear();
    if (getTickerData(market+"/transactions.json",&json))
    {
        arr=json_object_get_array(json);
        for(i=0;i<arr->length;i++)
        {
            Offer offer;
            jtmp=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp,"date",&jtmp2);
            offer.time=json_object_get_int64(jtmp2);
            json_object_object_get_ex(jtmp,"price",&jtmp2);
            offer.price=json_object_get_double(jtmp2);
            json_object_object_get_ex(jtmp,"amount",&jtmp2);
            offer.amount=json_object_get_double(jtmp2);
            json_object_object_get_ex(jtmp,"type",&jtmp2);
            offer.type=json_object_get_int(jtmp2);
            json_object_object_get_ex(jtmp,"type",&jtmp2);
            offer.id=json_object_get_int(jtmp2);
            trades.push_back(offer);
        }
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmaszyna::tradepair(double price,double amount,char type,string market)
{
#ifdef ARMED
    string b;

    if (type==BUY) b="buy";
    else if (type==SELL) b="sell";

    return(makeSimpleApiCall(market+"/"+b,string("nonce=")+to_stringl(getctime()).toStdString()+"&price="+to_stringp(price).toStdString()+"&amount="+to_stringd(amount).toStdString()+"&offer=true"));
#else
    Q_UNUSED(price);
    Q_UNUSED(amount);
    Q_UNUSED(type);
    Q_UNUSED(market);
    return(true);
#endif
}

bool Bitmaszyna::getAccountInfo()
{
    struct json_object *json,*jtmp,*jtmp2,*jtmp3;
    struct array_list *arr,*arr2;
    int i,j;

    curl_mutex.lock();
    if (makeApiCall("accountInfo",&json,string("nonce=")+to_stringl(getctime()).toStdString()))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        log(string(chunk.memory)+"\n");
        json_object_object_get_ex(json,"deposits",&jtmp);
        arr=json_object_get_array(jtmp);
        deposits.clear();
        for(i=0;i<arr->length;i++)
        {
            Deposit d;

            jtmp2=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp2,"currency",&jtmp3);
            d.currency=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"bank_name",&jtmp3);
            d.bank_name=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"pay_to",&jtmp3);
            d.pay_to=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"acc_num",&jtmp3);
            d.acc_num=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"swift_code",&jtmp3);
            d.swift_code=json_object_get_string(jtmp3);
            json_object_object_get_ex(jtmp2,"transfer_title",&jtmp3);
            d.transfer_title=json_object_get_string(jtmp3);
            deposits.push_back(d);
        }
        json_object_object_get_ex(json,"withdrawalAccounts",&jtmp);
        arr=json_object_get_array(jtmp);
        for(j=0;j<arr->length;j++)
        {
            withdrawalaccounts[j].clear();
            arr2=json_object_get_array((struct json_object *)array_list_get_idx(arr,j));
            for(i=0;i<arr2->length;)
            {
                WithdrawalAccount w;
                jtmp2=(struct json_object *)array_list_get_idx(arr2,i);
                w.account=json_object_get_string(jtmp2);
                i++;
                jtmp2=(struct json_object *)array_list_get_idx(arr2,i);
                w.name=json_object_get_string(jtmp2);
                i++;
                if (w.account.size()>0) withdrawalaccounts[adjustCurrency(j)].push_back(w);
            }
        }
        json_object_put(json);
        logged=true;
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

int Bitmaszyna::adjustCurrency(int in)
{
    if (in==0) return(BTC);
    else if (in==1) return(LTC);
    else if (in==2) return(PLN);
    return(-1);
}

bool Bitmaszyna::getfunds()
{    
    struct json_object *json,*jtmp,*jtmp2;

    curl_mutex.lock();
    if (makeApiCall("funds",&json,string("nonce=")+to_stringl(getctime()).toStdString()))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        log(string(chunk.memory)+"\n");
        json_object_object_get_ex(json,"funds",&jtmp);
        json_object_object_get_ex(jtmp,"available_BTC",&jtmp2);
        balance.balance[BTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_BTC",&jtmp2);
        balance.blocked[BTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_LTC",&jtmp2);
        balance.balance[LTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_LTC",&jtmp2);
        balance.blocked[LTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_KBM",&jtmp2);
        balance.balance[KBM]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_KBM",&jtmp2);
        balance.blocked[KBM]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_PLN",&jtmp2);
        balance.balance[PLN]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_PLN",&jtmp2);
        balance.blocked[PLN]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"takerfee",&jtmp2);
        fees.taker=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"makerfee",&jtmp2);
        fees.maker=json_object_get_double(jtmp2);
        json_object_put(json);
        logged=true;
        curl_mutex.unlock();
        return(getAccountInfo());
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmaszyna::getdepth(string market,Tables& tables)
{

    struct json_object *json,*jtmp;
    struct array_list *arr,*arr2;
    int i;
    double sum;

    curl_mutex.lock();
    tables.bids.clear();
    tables.asks.clear();
    if (getTickerData(market+"/depthSimple.json",&json))
    {
        json_object_object_get_ex(json,"bids",&jtmp);
        arr=json_object_get_array(jtmp);
        sum=0.0;
        for(i=0;i<arr->length;i++)
        {
            Offer offer;
            jtmp=(struct json_object *)array_list_get_idx(arr,i);
            arr2=json_object_get_array(jtmp);
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
            jtmp=(struct json_object *)array_list_get_idx(arr,i);
            arr2=json_object_get_array(jtmp);
            offer.type=BID;
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

bool Bitmaszyna::getopenorders(string)
{
    struct json_object *json,*jtmp;
    struct array_list *arr;
    int i;
    Offer o;

    curl_mutex.lock();
    orders.clear();
    if (makeApiCall("offers",&json,string("nonce=")+to_stringl(getctime()).toStdString()))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(json,"orders",&jtmp);
        arr=json_object_get_array(jtmp);
        for(i=0;i<arr->length;i++)
        {
            struct json_object *jtmp3,*jtmp4;

            jtmp3=(struct json_object *)array_list_get_idx(arr,i);
            json_object_object_get_ex(jtmp3,"id",&jtmp4);
            o.id=to_stringl(json_object_get_int(jtmp4)).toStdString();
            json_object_object_get_ex(jtmp3,"price",&jtmp4);
            o.price=json_object_get_double(jtmp4);
            json_object_object_get_ex(jtmp3,"amount",&jtmp4);
            o.amount=json_object_get_double(jtmp4);
            json_object_object_get_ex(jtmp3,"type",&jtmp4);
            o.type=json_object_get_double(jtmp4);
            orders.push_back(o);
        }
        json_object_put(json);
        curl_mutex.unlock();
        return(true);
    }
    curl_mutex.unlock();
    return(false);
}

bool Bitmaszyna::cancelorder(string id)
{
#ifdef ARMED
    return(makeSimpleApiCall("canceloffer",string("nonce=")+to_stringl(getctime()).toStdString()+"&id="+id));
#else
    Q_UNUSED(id);
    return(true);
#endif
}

bool Bitmaszyna::cancelall()
{
    vector<Offer>::iterator it;

    for(it=orders.begin();it!=orders.end();++it)
    {
        cancelorder(it->id);
    }
    return(true);
}

bool Bitmaszyna::marginBalanceAdd(string,double)
{
    return(true);
}

bool Bitmaszyna::marginBalanceRemove(string,double)
{
    return(true);
}
