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

import QtQuick 2.0

Item {
    id: button

    signal clicked

    property alias background: back.color
    property string text
    property color color: "#ffffff"

    width : Math.round(240*base.scalex())
    height: Math.round(85*base.scaley())

    Rectangle {
        id: back
        anchors.fill: parent
        color: "#41bb1a"
    }

    MouseArea {
        id: mouseRegion
        anchors.fill: parent
        onClicked: { button.clicked(); }
    }

    Text {
        id: btnText
        anchors.fill: parent
        anchors.margins: Math.round(5*base.scalex())
        text: button.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: button.color
        font.pixelSize: Math.round(35*base.scalex())
    }
}
