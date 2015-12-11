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

#ifndef BITMASZYNA_H_
#define BITMASZYNA_H_

#include <string>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include <json/json.h>
#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include "key.h"
#include "Base.h"
using namespace std;

class Bitmaszyna:public Exchange {
public:
	static const string strMessageMagic;
    CKey ckey;
	vector<unsigned char> v,vsig;
	bool bb;
	string msg,signature;
	SHA256_CTX ctx;
	uint256 hash1,hash2;
	Balance balanceltc;
	list<Offer> bidsltc,asksltc,ordersltc;
	double dummy;

	Bitmaszyna();
	virtual ~Bitmaszyna();
    bool getdepth(string market,Tables&);
	bool getfunds();
    bool tradepair(double price,double amount,char type,string market);
    bool getopenorders(string market);
	bool cancelorder(string id);
	bool cancelall();
    bool withdraw(double,const string&,const string&,const string&,const string&,bool,bool,double& fee);
    bool deposit(const string&);
    bool lasttrades(string);
    bool getticker(string,Ticker&);
    bool marginList(string market);
    bool marginOpen(string market,char type,double leverage,double amount,double rate,double rateLoss,double rateProfit);
    bool marginClose(string market,string id,double amount);
    bool marginCancel(string market,string id,double amount);
    bool marginModify(string market,string id,double rate,double rateLoss,double rateProfit);
    bool swapList(string);
    bool swapOpen(string,double,double);
    bool swapClose(string,string);
    bool marketChart(string market);
    bool marginBalanceAdd(string market,double amount);
    bool marginBalanceRemove(string market,double amount);
    bool marketChart(string market,long,long,long);
    bool history(string currency);

    bool makeApiCall(string method,struct json_object **json,string post);
    void getError(struct json_object **json);
    bool fetchData(struct json_object **json,struct json_object **jtmp);
    bool makeSimpleApiCall(string method,string post);
    bool getTickerData(string url,struct json_object **json);
	string sign(string& privkey,string& msg);
	int api(string,string);
	string strfromtype(char type);
};

#endif /* BITMASZYNA_H_ */
