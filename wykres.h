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

#ifndef WYKRES_H
#define WYKRES_H

#include <QtQuick>
#include "basedata.h"
#include "Base.h"

class Zakres
{
public:
    long xstart,xend;
    double ystart,yend;
};

class Mpoint
{
public:
    double y;
    long x;
};

class Swieczka
{
public:
    double w;
};

class Wykres : public QQuickPaintedItem
{
    Q_OBJECT

    QBrush *brushRed,*brushGreen;
    QPen *pen,*penKnot,*penText;
    Mpoint frame,frame2,podzial;
    Zakres zakres;
    Swieczka swieczka;
    QFont font;

    public:
        Wykres(QQuickItem *parent = 0);
        void paint(QPainter *painter);
        double danetowykresx(long x);
        double danetowykresy(double y);
        long wykrestodanex(double x);
        double wykrestodaney(double y);
        void rysujSwieczke(QPainter *painter,Ohlc ohlc);
};

#endif // WYKRES_H
