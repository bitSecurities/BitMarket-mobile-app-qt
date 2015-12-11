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

#include "wykres.h"

extern BaseData *base;

Wykres::Wykres(QQuickItem *)
{
    pen=new QPen(QColor("#d9d9d9"));
    penKnot=new QPen(QColor("#797c81"));
    penText=new QPen(QColor("#000000"));
    font.setPixelSize(35*base->scaley());
    QLinearGradient green;
    green.setColorAt(0, QColor("#a1ea98"));
    green.setColorAt(1, QColor("#62ae56"));
    QLinearGradient red;
    red.setColorAt(0, QColor("#f78181"));
    red.setColorAt(1, QColor("#c34e47"));
    brushRed=new QBrush(red);
    brushGreen=new QBrush(green);
}

double Wykres::danetowykresx(long x)
{
    return(frame.x+(boundingRect().width()-frame.x-frame2.x)*((double)(x-zakres.xstart)/(double)(zakres.xend-zakres.xstart)));
}

double Wykres::danetowykresy(double y)
{
    return(frame.y+(boundingRect().height()-frame.y-frame2.y)*((y-zakres.ystart)/(zakres.yend-zakres.ystart)));
}

long Wykres::wykrestodanex(double x)
{
    return(zakres.xstart+(zakres.xend-zakres.xstart)*((x-frame.x)/(boundingRect().width()-frame.x-frame2.x)));
}

double Wykres::wykrestodaney(double y)
{
    return(zakres.ystart+(zakres.yend-zakres.ystart)*((y-frame.y)/(boundingRect().height()-frame.y-frame2.y)));
}

void Wykres::rysujSwieczke(QPainter *painter,Ohlc ohlc)
{
    painter->setPen(*penKnot);
    if (ohlc.o<ohlc.c)
    {
        painter->setBrush(*brushGreen);
        painter->drawLine(round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.h)),round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.c)));
        painter->drawLine(round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.o)),round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.l)));
        painter->drawRect(round(danetowykresx(ohlc.t)-swieczka.w/2.0),round(danetowykresy(ohlc.c)),round(swieczka.w),round(danetowykresy(ohlc.o)-danetowykresy(ohlc.c)));
    }else if (ohlc.o>ohlc.c)
    {
        painter->setBrush(*brushRed);
        painter->drawLine(round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.h)),round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.o)));
        painter->drawLine(round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.c)),round(danetowykresx(ohlc.t)),round(danetowykresy(ohlc.l)));
        painter->drawRect(round(danetowykresx(ohlc.t)-swieczka.w/2.0),round(danetowykresy(ohlc.o)),round(swieczka.w),round(danetowykresy(ohlc.c)-danetowykresy(ohlc.o)));
    }else painter->drawLine(round(danetowykresx(ohlc.t)-swieczka.w/2.0),round(danetowykresy(ohlc.o)),round(danetowykresx(ohlc.t)+swieczka.w/2.0),round(danetowykresy(ohlc.o)));
}

QString formatDate(long t)
{
    QDateTime time;
    time.setMSecsSinceEpoch((int64_t)t*1000ULL);
    if (base->timeframe>=3600) return(time.toString("yyyy-MM-dd hh:mm"));
    else return(time.toString("hh:mm"));
}

void Wykres::paint(QPainter *painter)
{
    double i,diff;
    vector<Ohlc>::iterator it;
    QString txt;
    long start;

    if (base->swieczki.size()==0) return;
    painter->setFont(font);
    QFontMetrics fm=painter->fontMetrics();
    //log(("paint1 timeframe: "+QString::number(base->timeframe)+" "+QString::number(base->swieczki.size())+"\n").toStdString());
    if (base->timeframe<3600) podzial.x=base->timeframe*base->podzial;
    else podzial.x=base->timeframe*base->podzial*2.4;
    zakres.xend=base->swieczki.front().t;
    zakres.xstart=zakres.xend-base->timeframe*base->ileswieczek;
    //log(("paint1 timeframe: "+QString::number(zakres.xstart)+" "+QString::number()+"\n").toStdString());
    zakres.ystart=0;
    zakres.yend=0;
    i=0;
    for(it=base->swieczki.begin();it!=base->swieczki.end();++it) {
        //if ((zakres.xstart>it->t)||(zakres.xstart==0)) zakres.xstart=it->t;
        //if (zakres.xend<it->t) zakres.xend=it->t;
        if ((zakres.yend>it->l)||(zakres.yend==0)) zakres.yend=it->l;
        if (zakres.ystart<it->h) zakres.ystart=it->h;
        if (i>base->ileswieczek) break;
        else i++;
    }
    podzial.y=round((zakres.ystart-zakres.yend)/10.0);
    if (podzial.y==0) {
        podzial.y=1;
        zakres.ystart+=4;
        zakres.yend-=4;
    }
    //log(QString::number(podzial.y).toStdString());
    zakres.yend-=podzial.y;
    zakres.ystart+=podzial.y;
    frame.x=100*base->scalex();
    frame.y=20*base->scaley();
    frame2.x=10*base->scalex();
    frame2.y=60*base->scaley();
    swieczka.w=(danetowykresx(base->timeframe)-danetowykresx(0))*0.6;
    //log("ww: "+QString::number(swieczka.w)+" "+QString::number(danetowykresx(base->timeframe))+" "+QString::number(danetowykresx(0))+" "+QString::number(frame.x)+" "+QString::number(boundingRect().width())+" "+QString::number(frame2.x));
    start=zakres.xstart-(long)zakres.xstart%(long)(podzial.x)+podzial.x;
    diff=danetowykresx(start+podzial.x)-danetowykresx(start);
    //log(("paint2 timeframe: "+QString::number(danetowykresx(start))+" "+QString::number(danetowykresx(zakres.xend))+"\n").toStdString());
    for(i=danetowykresx(start);i<=danetowykresx(zakres.xend);i+=diff)
    {
        //log(("paint6 timeframe: "+QString::number(i)+" "+QString::number(diff)+"\n").toStdString());
        txt=formatDate(round(wykrestodanex(i)));
        painter->setPen(*penText);
        painter->drawText((int)round(i-fm.width(txt)/2.0),(int)round(boundingRect().height()-frame2.y),(int)round(i+fm.width(txt)/2.0),(int)round(boundingRect().height()),0,txt);
        painter->setPen(*pen);
        painter->drawLine(round(i),frame.y,round(i),boundingRect().height()-frame2.y);
    }
    start=zakres.yend-(int)zakres.yend%(int)(podzial.y)+podzial.y;
    diff=danetowykresy(start)-danetowykresy(start+podzial.y);
    //log(("paint3 timeframe: "+QString::number(base->timeframe)+" "+QString::number(base->swieczki.size())+"\n").toStdString());
    for(i=danetowykresy(start);i>=danetowykresy(zakres.ystart);i-=diff)
    {
        painter->setPen(*penText);
        painter->drawText((int)0,(int)round(i-fm.height()/2.0),(int)120*base->scalex(),(int)(i+fm.height()/2.0),0,QString::number(round(wykrestodaney(i))));
        painter->setPen(*pen);
        painter->drawLine(frame.x,round(i),boundingRect().width()-frame2.x,round(i));
    }
    //log(("paint4 timeframe: "+QString::number(base->timeframe)+" "+QString::number(base->swieczki.size())+"\n").toStdString());
    for(it=base->swieczki.begin();it!=base->swieczki.end();++it) {
        if (danetowykresx(it->t)<frame.x+swieczka.w) break;
        rysujSwieczke(painter,*it);
        //log((QString::number(it->o)+"\n").toStdString());
    }
    //log(("paint5 timeframe: "+QString::number(base->timeframe)+" "+QString::number(swieczka.w)+"\n").toStdString());
}
