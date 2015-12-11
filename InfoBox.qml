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
import QtQuick.Controls 1.3

Rectangle {
    property alias infoup : infoup
    property alias infodown : infodown

    width: Math.round(347*base.scalex())
    height: Math.round(100*base.scaley())
    color: "#ffffff"

    Text {
        id: infoup
        y: Math.round(3*base.scaley())
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
        color: "#ffffff"
        font.pixelSize: Math.round(35*base.scalex())
    }

    Text {
        id: infodown
        y: Math.round(50*base.scaley())
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
        text: "--"
        color: "#ffffff"
        font.bold: true
        font.pixelSize: Math.round(35*base.scalex())
    }
}
