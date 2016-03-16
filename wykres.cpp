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
    brushBid=new QBrush(QColor("#cadff5"));
    brushAsk=new QBrush(QColor("#c9f787"));
}

double Wykres::danetowykresx(double x)
{
    return(frame.x+(boundingRect().width()-frame.x-frame2.x)*((double)(x-zakres.xstart)/(double)(zakres.xend-zakres.xstart)));
}

double Wykres::danetowykresy(double y)
{
    return(frame.y+(boundingRect().height()-frame.y-frame2.y)*((y-zakres.ystart)/(zakres.yend-zakres.ystart)));
}

double Wykres::wykrestodanex(double x)
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
    double i,diff,start;
    vector<Ohlc>::iterator it;
    QString txt;

    painter->setFont(font);
    QFontMetrics fm=painter->fontMetrics();
    if (base->timeframe>0)
    {
        if (base->swieczki.size()==0) return;
        if (base->timeframe<3600) podzial.x=base->timeframe*base->podzial;
        else podzial.x=base->timeframe*base->podzial*2.4;
        zakres.xend=base->swieczki.front().t;
        zakres.xstart=zakres.xend-base->timeframe*base->ileswieczek;
        zakres.ystart=0;
        zakres.yend=0;
        i=0;
        for(it=base->swieczki.begin();it!=base->swieczki.end();++it) {
            if ((zakres.yend>it->l)||(zakres.yend==0)) zakres.yend=it->l;
            if (zakres.ystart<it->h) zakres.ystart=it->h;
            if (i>base->ileswieczek) break;
            else i++;
        }
        podzial.y=round((zakres.ystart-zakres.yend)/10.0);
        if (podzial.y==0) {
            podzial.y=1;
            if (base->cmarket=="KBMBTC")
            {
                podzial.y=0.02;
                zakres.ystart+=0.01;
                zakres.yend-=0.01;
            } else if (base->cmarket=="LTCBTC")
            {
                podzial.y=0.001;
                zakres.ystart+=0.001;
                zakres.yend-=0.001;

            }else
            {
                zakres.ystart+=4;
                zakres.yend-=4;
            }
        }
        zakres.yend-=podzial.y;
        zakres.ystart+=podzial.y;
        frame.x=100*base->scalex();
        frame.y=20*base->scaley();
        frame2.x=10*base->scalex();
        frame2.y=60*base->scaley();
        swieczka.w=(danetowykresx(base->timeframe)-danetowykresx(0))*0.6;
        start=zakres.xstart-(long)zakres.xstart%(long)(podzial.x)+podzial.x;
        diff=danetowykresx(start+podzial.x)-danetowykresx(start);
        for(i=danetowykresx(start);i<=danetowykresx(zakres.xend);i+=diff)
        {
            txt=formatDate(round(wykrestodanex(i)));
            painter->setPen(*penText);
            painter->drawText((int)round(i-fm.width(txt)/2.0),(int)round(boundingRect().height()-frame2.y),(int)round(i+fm.width(txt)/2.0),(int)round(boundingRect().height()),0,txt);
            painter->setPen(*pen);
            painter->drawLine(round(i),frame.y,round(i),boundingRect().height()-frame2.y);
        }
        if (podzial.y>=1) start=zakres.yend-(int)zakres.yend%(int)(podzial.y)+podzial.y;
        else start=zakres.yend;
        diff=danetowykresy(start)-danetowykresy(start+podzial.y);
        for(i=danetowykresy(start);i>=danetowykresy(zakres.ystart);i-=diff)
        {
            double dd;

            painter->setPen(*penText);
            if ((base->cmarket=="KBMBTC")||(base->cmarket=="LTCBTC")) dd=wykrestodaney(i);
            else dd=round(wykrestodaney(i));
            painter->drawText((int)0,(int)round(i-fm.height()/2.0),(int)120*base->scalex(),(int)(i+fm.height()/2.0),0,QString::number(dd));
            painter->setPen(*pen);
            painter->drawLine(frame.x,round(i),boundingRect().width()-frame2.x,round(i));
        }
        for(it=base->swieczki.begin();it!=base->swieczki.end();++it) {
            if (danetowykresx(it->t)<frame.x+swieczka.w) break;
            rysujSwieczke(painter,*it);
        }
    }else
    {
        vector<Depth>::iterator it;
        QPainterPath path,path2;
        QPolygon poly;
        double dziel,dziel2;

        painter->setFont(font);
        QFontMetrics fm=painter->fontMetrics();
        frame.x=100*base->scalex();
        frame.y=20*base->scaley();
        frame2.x=10*base->scalex();
        frame2.y=60*base->scaley();
        zakres.xstart=base->depthCenter-base->depthCenter*base->depthRange;
        zakres.xend=base->depthCenter+base->depthCenter*base->depthRange;
        if (zakres.xend<1) dziel=4.0;
        else dziel=8.0;
        dziel2=14.0;
        zakres.ystart=base->depthMax*1.1;
        zakres.yend=0;
        podzial.y=(zakres.ystart-zakres.yend)/dziel2;
        if (podzial.y>1) podzial.y=round(podzial.y);
        podzial.x=(zakres.xend-zakres.xstart)/dziel;
        if (podzial.x>0.5) podzial.x=round(podzial.x);
        if (podzial.x>100) podzial.x-=((int)podzial.x)%100;
        else if (podzial.x>10) podzial.x-=((int)podzial.x)%10;
        if (zakres.xstart>1000) start=zakres.xstart-((int)zakres.xstart)%100+100;
        else if (zakres.xstart>100) start=zakres.xstart-((int)zakres.xstart)%10+10;
        else start=zakres.xstart;
        if (zakres.xstart>1) start=ceil(zakres.xstart);
        diff=danetowykresx(start+podzial.x)-danetowykresx(start);
        for(i=danetowykresx(start);i<=danetowykresx(zakres.xend);i+=diff)
        {
            txt=QString::number(wykrestodanex(i));
            painter->setPen(*penText);
            painter->drawText((int)round(i-fm.width(txt)/2.0),(int)round(boundingRect().height()-frame2.y),(int)round(i+fm.width(txt)/2.0),(int)round(boundingRect().height()),0,txt);
            painter->setPen(*pen);
            painter->drawLine(round(i),frame.y,round(i),boundingRect().height()-frame2.y);
        }
        start=zakres.yend;
        diff=danetowykresy(start)-danetowykresy(start+podzial.y);
        for(i=danetowykresy(start);i>=danetowykresy(zakres.ystart);i-=diff)
        {
            double dd;

            painter->setPen(*penText);
            dd=wykrestodaney(i);
            painter->drawText((int)0,(int)round(i-fm.height()/2.0),(int)120*base->scalex(),(int)(i+fm.height()/2.0),0,QString::number(dd));
            painter->setPen(*pen);
            painter->drawLine(frame.x,round(i),boundingRect().width()-frame2.x,round(i));
        }
        painter->setClipRect(frame.x,frame.y,boundingRect().width()-frame2.x-frame.x,boundingRect().height()-frame2.y-frame.y);
        poly.append(QPoint(frame.x,boundingRect().height()-frame2.y));
        it=base->depthbid.begin();
        poly.append(QPoint(round(danetowykresx(it->price)),boundingRect().height()-frame2.y));
        for(it=base->depthbid.begin();it!=base->depthbid.end();++it)
        {
            poly.append(QPoint(round(danetowykresx(it->price)),round(danetowykresy(it->value))));
            if (round(danetowykresx(it->price))<frame.x) break;
        }
        painter->drawPolygon(poly);
        path.addPolygon(poly);
        painter->fillPath(path, *brushBid);
        poly.clear();
        poly.append(QPoint(boundingRect().width()-frame2.x,boundingRect().height()-frame2.y));
        it=base->depthask.begin();
        poly.append(QPoint(round(danetowykresx(it->price)),boundingRect().height()-frame2.y));
        for(it=base->depthask.begin();it!=base->depthask.end();++it)
        {
            poly.append(QPoint(round(danetowykresx(it->price)),round(danetowykresy(it->value))));
            if (round(danetowykresx(it->price))>boundingRect().width()-frame2.x) break;
        }
        path2.addPolygon(poly);
        painter->fillPath(path2, *brushAsk);
    }
}
