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

import QtQuick 2.1

Rectangle
{
    id: marketInfo
    width: base.getWidth()
    height: Math.round(210*base.scaley())
    color: "#f5f4f2"

    property alias buyprice:buyprice
    property alias sellprice:sellprice
    property alias lastprice:lastprice
    property alias maxprice:maxprice
    property alias minprice:minprice
    property alias volume:volume

    InfoBox
    {
        id: buyprice
        x: Math.round(10*base.scalex())
        color: "#41bb1a"
        infoup.text: base.trans(93)+":"
    }

    InfoBox
    {
        id: sellprice
        x: Math.round(367*base.scalex())
        color: "#d00403"
        infoup.text: base.trans(94)+":"
    }

    InfoBox
    {
        id: lastprice
        x: Math.round(724*base.scalex())
        infoup.color: "#000000"
        infodown.color: "#000000"
        infoup.text: base.trans(32)+":"
    }

    InfoBox
    {
        id: maxprice
        x: Math.round(10*base.scalex())
        y: Math.round(110*base.scaley())
        infoup.color: "#000000"
        infodown.color: "#000000"
        infoup.text: base.trans(33)+":"
    }

    InfoBox
    {
        id: minprice
        x: Math.round(367*base.scalex())
        y: Math.round(110*base.scaley())
        infoup.color: "#000000"
        infodown.color: "#000000"
        infoup.text: base.trans(34)+":"
    }

    InfoBox
    {
        id: volume
        x: Math.round(724*base.scalex())
        y: Math.round(110*base.scaley())
        infoup.color: "#000000"
        infodown.color: "#000000"
        infoup.text: base.trans(35)+":"
    }
}
