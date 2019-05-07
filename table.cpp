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

#include "table.h"
#include <QDebug>
#include <QDateTime>
#include <QWindow>
#include <iostream>
#include <sstream>

Table::Table(BaseData *base,int type)
{
    this->base=base;
    this->type=type;
}
QHash<int, QByteArray> Table::roleNames() const
{
    if ((type==MARKETS)||(type==EXCHANGES)||(type==MENU)||(type==LEVERAGE)||(type==ORDER)||(type==ORDERTYPE)||(type==TIMEFRAMES)||(type==WITHDRAWALACCOUNTS)||(type==DEPOSITCURRENCIES)||(type==WITHDRAWCURRENCIES))
    {
        QHash<int, QByteArray> roles;
        roles[Price] = "name";
        return roles;
    }else if (type==BALANCE)
    {
        QHash<int, QByteArray> roles;
        roles[Curr] = "currency";
        roles[Price] = "name";
        roles[Available] = "available";
        roles[Blocked] = "blocked";
        roles[Total] = "total";
        return roles;
    }
    else if (type==LASTTRADES)
    {
        QHash<int, QByteArray> roles;
        roles[Time] = "time";
        roles[Type] = "type";
        roles[Price] = "price";
        roles[Amount] = "amount";
        roles[Sum] = "total";
        return roles;
    }
    else if (type==ORDERS)
    {
        QHash<int, QByteArray> roles;
        roles[Type] = "type";
        roles[Price] = "price";
        roles[Amount] = "amount";
        roles[Sum] = "sum";
        roles[Action] = "action";
        roles[Tid] = "tid";
        return roles;
    }
    else if (type==OPENPOSITIONS)
    {
        QHash<int, QByteArray> roles;
        roles[Opened] = "opened";
        return roles;
    }
    else if (type==OPENSWAPS)
    {
        QHash<int, QByteArray> roles;
        roles[Price] = "price";
        roles[Amount] = "amount";
        roles[Earnings] = "earnings";
        roles[Tid] = "tid";
        return roles;
    }
    else if (type==HISTORY)
    {
        QHash<int, QByteArray> roles;
        roles[Time] = "time";
        roles[Amount] = "amount";
        roles[Type] = "type";
        roles[Price] = "price";
        roles[Action] = "receiver";
        roles[Tid] = "transaction_id";
        return roles;
    }
    else
    {
        QHash<int, QByteArray> roles;
        roles[Price] = "price";
        roles[Amount] = "amount";
        roles[Sum] = "sum";
        roles[Action] = "action";
        return roles;
    }
}

QVariant Table::get(int row)
{
    if (type==BALANCE)
    {
        if ((row<0)||(row>MAXCURR)) return(QVariant());
    }else
    {
        if ((row<0)||(row>rowCount())) return(QVariant());
    }
    if (type==MARKETS)
    {
        return(QVariant::fromValue(new ListObject(QString(base->currentExchange->marketsLong[row].c_str()))));
    }else if (type==MENU)
    {
        if (row==0) return(QVariant::fromValue(new ListObject((base->translations[50]))));
        return(QVariant::fromValue(new ListObject((base->translations[base->menuitem[row]]))));
    }else if (type==LEVERAGE)
    {
        return(QVariant::fromValue(new ListObject((base->translations[36]+" "+to_string1(base->levs[row])))));
    }
    else if (type==DEPOSITCURRENCIES)
    {
        Currency ret;
        QVariantMap map;

        int j,i;

        j=0;
        for (i=0;i<row;i++)
        {
            if (base->currentExchange->currencies[j].deposit) j++;
        }
        ret=base->currentExchange->currencies[j];
        map["name"]=ret.name.c_str();
        return(map);
    }
    else if (type==WITHDRAWCURRENCIES)
    {
        Currency ret;
        QVariantMap map;

        if (row<(int)rowCount()-2) {
            int j,i;

            j=0;
            for (i=0;i<row;i++)
            {
                if (base->currentExchange->currencies[j].deposit) j++;
            }
            ret=base->currentExchange->currencies[j];
        }
        else if (row<(int)rowCount()-1) ret.name=base->trans(138).toStdString();
        else ret.name=base->trans(129).toStdString();
        map["name"]=ret.name.c_str();
        return(map);
    }else if (type==BALANCE)
    {
        QVariantMap map;
        vector<Currency>::iterator it;

        for(it=base->currentExchange->currencies.begin();it!=base->currentExchange->currencies.end();++it) if (row==it->type) break;
        if (it==base->currentExchange->currencies.end()) return(QVariant());
        map["name"]=it->name.c_str();
        map["value"]=formatCurr(base->currentExchange->balance.balance[it->type],it->type);
        return(map);
    }else if (type==EXCHANGES)
    {
        return(QVariant::fromValue(new ListObject((base->exchanges[row]->name.c_str()))));
    }else if (type==ORDER)
    {
        return(QVariant::fromValue(new ListObject((base->trans(row+85)))));
    }else if (type==ORDERTYPE)
    {
        if (row==0) return(QVariant::fromValue(new ListObject((base->trans(91)))));
        else if (row==(int)(base->currentExchange->currencies.size()+1)) return(QVariant::fromValue(new ListObject(base->trans(137))));
        else if (row==(int)(base->currentExchange->currencies.size()+2)) return(QVariant::fromValue(new ListObject(base->trans(128))));
        else return(QVariant::fromValue(new ListObject((base->trans(92)+" "+base->currentExchange->currencies[row-1].name.c_str()))));
    }else if ((type==ORDERS)||(type==OPENSWAPS))
    {
        QVariantMap map;
        QString s;

        foreach(int k, roleNames().keys()) {
            s =  data(index(row, 0), k).toString();
            map[roleNames().value(k)] = s.toLower();
        }
        return map;
    }else if (type==OPENPOSITIONS)
    {
        return(data(index(row,0),Opened));
    }else if (type==WITHDRAWALACCOUNTS)
    {
        if ((row<0)||(row>=rowCount())||(rowCount()==0)) return(QVariant::fromValue(new ListObject(base->trans(118))));
        return(QVariant::fromValue(new ListObject(base->currentExchange->withdrawalaccounts[base->currentExchange->withdrawalcurrency][row].name.c_str()+QString(" - ")+base->currentExchange->withdrawalaccounts[base->currentExchange->withdrawalcurrency][row].account.c_str())));
    }else if (type==TIMEFRAMES)
    {
        if (base->currentExchange->id==BITMASZYNA) return(QVariant::fromValue(new ListObject(base->timeframes_bitmaszyna[row])));
        else return(QVariant::fromValue(new ListObject(base->timeframes[row])));
    }
    return(QVariant());
}

QString Table::formatCurr(double var,int type) const
{
    if ((type==PLN)||(type==EUR)) return(to_string2(var));
    else return(to_stringd(var));
}

QVariant Table::data(const QModelIndex &index, int role) const
{
    Offer ret;

    if ((index.row()<0)||(index.row()>rowCount())) return(QVariant());
    if (type==BID)
    {
        //logError("bid at "+to_stringl(index.row())+"\n");
        base->mutex_list.lock();
        ret=base->currentExchange->currentTables->bids[index.row()];
        base->mutex_list.unlock();

    }else if (type==ASK)
    {
        //logError("ask at "+to_stringl(index.row())+"\n");
        base->mutex_list.lock();
        if (base->reverse) ret=base->currentExchange->currentTables->asks[base->currentExchange->currentTables->asks.size()-index.row()-1];
        else ret=base->currentExchange->currentTables->asks[index.row()];
        base->mutex_list.unlock();
    }else if (type==OPENSWAPS)
    {
        if (role==Tid) return(QString(base->currentExchange->swaps[index.row()].id.c_str()));
        else if (role==Price) return(to_stringd(base->currentExchange->swaps[index.row()].rate));
        else if (role==Amount) return(to_stringd(base->currentExchange->swaps[index.row()].amount));
        else if (role==Earnings) return(to_stringd(base->currentExchange->swaps[index.row()].earnings));
    }
    else if (type==OPENPOSITIONS)
    {
        QVariantMap map;
        QDateTime time;

        //logError(to_string2(index.row())+"\n");
        time.setMSecsSinceEpoch((int64_t)base->currentExchange->positions[index.row()].time*1000ULL);
        map["time"]=time.toString("yyyy-MM-dd hh:mm:ss");
        map["rateProfit"]=to_string2(base->currentExchange->positions[index.row()].rateProfit);
        map["rateLoss"]=to_string2(base->currentExchange->positions[index.row()].rateLoss);
        map["opened"]=to_string2(base->currentExchange->positions[index.row()].fiatOpened);
        map["total"]=to_string2(base->currentExchange->positions[index.row()].fiatTotal);
        if (base->currentExchange->positions[index.row()].type==BUY) map["type"]=base->translations[40];
        else map["type"]=base->translations[41];
        //map["price"]=base->translations[43]+": "+to_string2(base->currentExchange->positions[index.row()].price)+"\n";
        map["price"]=to_string2(base->currentExchange->positions[index.row()].price);
        map["rateClose"]=base->translations[44]+": <b>"+to_string2(base->currentExchange->positions[index.row()].rateClose)+"</b>";
        map["profit"]=base->translations[46]+": <b>"+to_stringd(base->currentExchange->positions[index.row()].profit)+"</b>(<b>"+to_string2(base->currentExchange->positions[index.row()].profitPercentage)+"</b>)";
        map["fees"]=base->translations[45]+": <b>"+to_stringd(base->currentExchange->positions[index.row()].fees)+"</b>";
        map["leverage"]=base->translations[36]+": <b>"+to_string2(base->currentExchange->positions[index.row()].leverage)+"</b>";
        map["security"]=base->translations[42]+": <b>"+to_stringd(base->currentExchange->positions[index.row()].security)+"</b>";
        map["tid"]=base->currentExchange->positions[index.row()].id.c_str();
        return(map);
    }else if (type==MENU)
    {
        return(base->translations[base->menuitem[index.row()]]);
    }else if (type==LEVERAGE)
    {
        return(base->translations[36]+" "+to_string1(base->levs[index.row()]));
    }
    else if (type==MARKETS)
    {
        return(QString(base->currentExchange->marketsLong[index.row()].c_str()));
    }else if (type==EXCHANGES)
    {
        return(QString(base->exchanges[index.row()]->name.c_str()));
    }else if (type==ORDERS)
    {
        ret=base->currentExchange->orders[index.row()];
    }
    else if (type==DEPOSITCURRENCIES)
    {
        Currency ret;
        int i,j;

        j=0;
        for (i=0;i<index.row();i++)
        {
            if (base->currentExchange->currencies[j].deposit) j++;
        }
        ret=base->currentExchange->currencies[j];
        if ((role==Curr)||(role==Price)){
            return(ret.name.c_str());
        }
    }
    else if (type==WITHDRAWCURRENCIES)
    {
        Currency ret;
        if (index.row()<(int)rowCount()-2) {
            int i,j;

            j=0;
            for (i=0;i<index.row();i++)
            {
                if (base->currentExchange->currencies[j].deposit) j++;
            }
            ret=base->currentExchange->currencies[j];
        }
        else if (index.row()<(int)rowCount()-1) ret.name=base->trans(138).toStdString();
        else ret.name=base->trans(129).toStdString();
        if ((role==Curr)||(role==Price)){
            return(ret.name.c_str());
        }
    }else if (type==BALANCE)
    {
        Currency ret;

        ret=base->currentExchange->currencies[index.row()];
        if ((role==Curr)||(role==Price)) return(ret.name.c_str());
        else if (role==Available) return(formatCurr(base->currentExchange->balance.balance[ret.type],ret.type));
        else if (role==Blocked) return(formatCurr(base->currentExchange->balance.blocked[ret.type],ret.type));
        else if (role==Total) return(formatCurr(base->currentExchange->balance.balance[ret.type]+base->currentExchange->balance.blocked[ret.type],ret.type));
        return("");
    }else if (type==LASTTRADES)
    {
        base->mutex_list.lock();
        ret=base->currentExchange->trades[index.row()];
        base->mutex_list.unlock();
        if (role==Time) {
            QDateTime time;
            time.setMSecsSinceEpoch((int64_t)ret.time*1000ULL);
            return(time.toString("MM-dd hh:mm:ss"));
        }
        else if (role==Type) {
            if (ret.type==0) return(base->translations[93]+":");
            else return(base->translations[94]+":");
        }
        if (role==Price) return(ret.getPrice());
        else if (role==Amount) return(ret.getAmount());
        else if (role==Sum) return(ret.format(ret.amount*ret.price));
        else return("");
    }
    else if (type==ORDER)
    {
        return(base->trans(85+index.row()));
    }
    else if (type==ORDERTYPE)
    {
        if (index.row()==0) return(base->trans(91));
        else if (index.row()==(int)(base->currentExchange->currencies.size()+1)) return(base->trans(137));
        else if (index.row()==(int)(base->currentExchange->currencies.size()+2)) return(base->trans(128));
        else return(base->trans(92)+" "+base->currentExchange->currencies[index.row()-1].name.c_str());
    }else if (type==HISTORY)
    {
        History ret;

        ret=base->currentExchange->historyTable[index.row()];
        if (role==Time) {
            QDateTime time;
            time.setMSecsSinceEpoch((int64_t)ret.time*1000ULL);
            return(time.toString("MM-dd hh:mm:ss"));
        }
        if (base->ordertype==INTERNALTRANSFERS)
        {
            if (role==Price) return(ret.currency.c_str());
            else if (role==Amount) return(to_stringd(ret.amount));
            else if (role==Type) {
                if (ret.type=="sent") return(base->trans(132)+" "+ret.receiver.c_str());
                else return(base->trans(133)+" "+ret.receiver.c_str());
            }
            else if (role==Action) return(ret.currency.c_str());
            else return("");
        }else if (base->ordertype==WITHDRAWALS)
        {
            if (role==Price) return(ret.currency.c_str());
            else if (role==Amount) return(to_stringd(ret.amount));
            else if (role==Type) return((ret.type+": "+ret.receiver).c_str());
            else if (role==Action) return(ret.currency.c_str());
            else return("");
        }else
        {
            if (role==Price) return(to_string2(ret.rate));
            else if (role==Amount) return(to_string2(ret.amount));
            else if (role==Type) return((ret.type+":").c_str());
            else if (role==Action) return(ret.receiver.c_str());
            else return("");
        }
    }else if (type==WITHDRAWALACCOUNTS)
    {
        if ((index.row()<0)||(index.row()>=rowCount())||(rowCount()==0)) return(QVariant::fromValue(new ListObject(base->trans(118))));
        return(base->currentExchange->withdrawalaccounts[base->currentExchange->withdrawalcurrency][index.row()].name.c_str()+QString(" - ")+base->currentExchange->withdrawalaccounts[base->currentExchange->withdrawalcurrency][index.row()].account.c_str());
    }else if (type==TIMEFRAMES)
    {
        if (base->currentExchange->id==BITMASZYNA) return(base->timeframes_bitmaszyna[index.row()]);
        else return(base->timeframes[index.row()]);
    }
    if (role==Price) return(ret.getPrice());
    else if (role==Amount) return(ret.getAmount());
    else if (role==Sum) return(ret.format(ret.amount*ret.price));
    else if (role==Action){
        if (type==ORDERS) return("bin");
        else if (type==BID)
        {
            if (base->currentExchange->logged) {
                if (ret.sum<base->currentExchange->balance.balance[currency(base->cmarket,FIRST)]) return("buy");
                else return("disabled");
            }
            else return("");
        }else if (type==ASK)
        {
            if (base->currentExchange->logged) {
                if (ret.sumLower<base->currentExchange->balance.balance[currency(base->cmarket,SECOND)]) return("buy");
                else return("disabled");
            }
            else return("");
        }
        else if ((type==BID)||(type==ASK)) {
            if (base->currentExchange->logged) return("buy");
            else return("");
        }
    }
    else if (role==Type){
        if (ret.type==0) return(base->translations[99]);
        else return(base->translations[100]);
    }else if (role==Tid)
    {
        return(ret.id.c_str());
    }
    return("");
}

int Table::rowCount(const QModelIndex &) const
{
    if (type==BID) return(base->currentExchange->currentTables->bids.size());
    else if (type==ASK) return(base->currentExchange->currentTables->asks.size());
    else if (type==MARKETS) return(base->currentExchange->marketsLong.size());
    else if (type==MENU) return(MENUITEMSCOUNT);
    else if (type==EXCHANGES) return(base->exchanges.size());
    else if (type==ORDERS) return(base->currentExchange->orders.size());
    else if (type==BALANCE) return(base->currentExchange->currencies.size());
    else if (type==DEPOSITCURRENCIES) {
        unsigned int i,j;

        if (!base->isLogged()) return(0);
        j=0;
        for (i=0;i<base->currentExchange->currencies.size();i++)
        {
            if(base->currentExchange->currencies[i].deposit) j++;
        }
        return(j);
    }else if (type==WITHDRAWCURRENCIES) {
        unsigned int i,j;

        if (!base->isLogged()) return(0);
        j=0;
        for (i=0;i<base->currentExchange->currencies.size();i++)
        {
            if(base->currentExchange->currencies[i].deposit) j++;
        }
        return(j+2);
    }
    else if (type==ORDERTYPE) return(base->currentExchange->currencies.size()+3);
    else if (type==LASTTRADES) {
        if (base->limitRange==-1) return(base->currentExchange->trades.size());
        else return(base->limitRange);
    }
    else if (type==LEVERAGE) return(base->levs.size());
    else if (type==OPENPOSITIONS) return(base->currentExchange->positions.size());
    else if (type==OPENSWAPS) return(base->currentExchange->swaps.size());
    else if (type==ORDER) return(2);
    else if (type==HISTORY) return(base->currentExchange->historyTable.size());
    else if (type==WITHDRAWALACCOUNTS) {
        if (!base->isLogged()) return(0);
        else {
            if ((base->currentExchange->withdrawalcurrency<0)||(base->currentExchange->withdrawalcurrency>=MAXCURR)) return(0);
            return(base->currentExchange->withdrawalaccounts[base->currentExchange->withdrawalcurrency].size());
        }
    }
    else if (type==TIMEFRAMES) {
        if (base->currentExchange->id==BITMASZYNA) return(base->timeframes_bitmaszyna.size());
        else return(base->timeframes.size());
    }
    else return(0);
}

int Table::columnCount(const QModelIndex &) const
{
    return(1);
    /*    if (type==LASTTRADES) return(5);
    else if (type==OPENSWAPS) return(3);
    else if (type==OPENPOSITIONS) return(1);
    else return(4);*/
}

bool Table::insertRows(int startRow,int endRow,const QModelIndex &parent)
{
    beginInsertRows(parent, startRow, endRow);
    endInsertRows();
    return(true);
}

bool Table::removeRows(int startRow,int endRow,const QModelIndex &parent)
{
    beginRemoveRows(parent, startRow, endRow);
    endRemoveRows();
    return(true);
}
