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

const string Bitmaszyna::strMessageMagic="Bitmaszyna.pl API:\n";
extern void log(string s);

#define BITMASZYNAURL "https://bitmaszyna.pl/api/"

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
    currencies.push_back(c);
    c.name="BTC";
    c.type=BTC;
    currencies.push_back(c);
    c.name="LTC";
    c.type=LTC;
    currencies.push_back(c);
    c.name="KBM";
    c.type=KBM;
    currencies.push_back(c);
    key="";
    secret="";
}

string Bitmaszyna::sign(string& privkey,string& msg) {
    v=DecodeBase64(privkey.c_str(),&bb);
    if (bb)
    {
        log("invalid secret\n");
        return("");
    }else if (v.size()!=32)
    {
        log("secret too short\n");
        return("");
    }
    ckey.SetSecret(v);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,strMessageMagic.c_str(),strMessageMagic.size());
    SHA256_Update(&ctx,msg.c_str(),msg.size());
    SHA256_Final((unsigned char *)&hash1,&ctx);
    SHA256((unsigned char *)&hash1,sizeof(hash1),(unsigned char *)&hash2);
    if (!ckey.SignCompact(hash2, vsig)) {
        printf("Sign failed\n");
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
    lasterror=json_object_get_string(*json);
    logerror();
    json_object_put(*json);
}

bool Bitmaszyna::fetchData(struct json_object **json,struct json_object **jtmp)
{
    json_object_object_get_ex(*json,"result",jtmp);
    if (string(json_object_get_string(*jtmp))!=string("ok")) {
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

bool Bitmaszyna::marketChart(string market,long,long,long)
{
    Q_UNUSED(market);
    return(true);
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

bool Bitmaszyna::withdraw(double,const string&,const string&,const string&,const string&,bool,bool,double&)
{
    return(false);
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

bool Bitmaszyna::getfunds()
{    
    struct json_object *json,*jtmp,*jtmp2;

    curl_mutex.lock();
    if (makeApiCall("funds",&json,string("nonce=")+to_stringl(getctime()).toStdString()))
    {
        if (!fetchData(&json,&jtmp)) return(false);
        json_object_object_get_ex(json,"funds",&jtmp);
        json_object_object_get_ex(jtmp,"available_btc",&jtmp2);
        balance.balance[BTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_btc",&jtmp2);
        balance.blocked[BTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_ltc",&jtmp2);
        balance.balance[LTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_ltc",&jtmp2);
        balance.blocked[LTC]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_kbm",&jtmp2);
        balance.balance[KBM]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_kbm",&jtmp2);
        balance.blocked[KBM]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"available_pln",&jtmp2);
        balance.balance[PLN]=json_object_get_double(jtmp2);
        json_object_object_get_ex(jtmp,"blocked_pln",&jtmp2);
        balance.blocked[PLN]=json_object_get_double(jtmp2);
        json_object_put(json);
        logged=true;
        curl_mutex.unlock();
        return(true);
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
