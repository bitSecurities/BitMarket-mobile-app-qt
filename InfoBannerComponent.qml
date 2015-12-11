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

import QtQuick 2.2

Item {
    id: banner

    property alias message : messageText.text

    height: Math.round(100*base.scaley())

    Rectangle {
        id: background

        anchors.fill: banner
        color: "#444444"
        smooth: true
        opacity: 0.8
        border.color: "black"
        border.width: 2
        radius: 5
    }

    Text {
        font.pixelSize: Math.round(40*base.scaley())
        renderType: Text.QtRendering
        width: Math.round(150*base.scalex())
        height: Math.round(40*base.scaley())
        id: messageText

        anchors.fill: banner
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap

        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            messages.state = "hidden"
        }
    }
}
