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

#ifndef BITMARKET_H_
#define BITMARKET_H_

#include <curl/curl.h>
#include <string>
#include <list>
#include "Base.h"

class Bitmarket:public Exchange {
public:
	double bid,ask,sred,sredltc,dummy;

	Bitmarket();
	virtual ~Bitmarket();
    bool getdepth(string market,Tables&);
	void clearmem();
	bool getfunds();
    bool tradepair(double price,double amount,char,string market);
    bool getopenorders(string market);
	bool cancelorder(string id);
	bool cancelall();
    bool withdraw(double,const string&,const string&,const string&,const string&,bool,bool,double&);
    bool deposit(const string&);
    bool lasttrades(string);
    bool getticker(string,Ticker &);
    bool marginList(string market);
    bool marginOpen(string market,char type,double leverage,double amount,double rate,double rateLoss,double rateProfit);
    bool marginClose(string market,string id,double amount);
    bool marginCancel(string market,string id,double amount);
    bool marginModify(string market,string id,double rate,double rateProfit,double rateLoss);
    bool swapList(string);
    bool swapOpen(string,double,double);
    bool swapClose(string,string);
    bool marketChart(string market,long,long,long);
    bool makeApiCall(string method,struct json_object **json,string post);
    void getError(struct json_object **json);
    bool fetchData(struct json_object **json,struct json_object **jtmp);
    bool makeSimpleApiCall(string method,string post);
    bool getTickerData(string url,struct json_object **json);
    bool marginBalanceAdd(string market,double amount);
    bool marginBalanceRemove(string market,double amount);
    bool history(string currency);
    long api(string post,string);
	string signature(const string&) const;
    void fetchPosition(struct json_object *jtmp2);
};

#endif /* BITMARKET_H_ */
