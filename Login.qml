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

Item{
    property alias button : loginButton
    property alias text : password.text
    property alias displaytext : loginButton.text
    property alias password : password
    property alias outer : outer.visible
    property alias img : img
    visible: !base.isLogged()

    Rectangle
    {
        id: background

        width: base.getWidth()
        height: base.getHeight()
        color: "#41bb19"
        visible: img.visible
    }

    Image
    {
        id: img

        x: Math.round(112*base.scalex())
        y: Math.round(500*base.scaley())
        z: 11
        source: (ex.name==="Bitmarket")?"qrc:///images/bitmarket_login.png":"qrc:///images/bitmaszyna_login.png"
        width: Math.round(831*base.scalex())
        height: Math.round(816*base.scaley())
        visible: false
    }

    MText {
        x: 0
        y: Math.round(300*base.scaley())
        width: base.getWidth()
        horizontalAlignment: Text.AlignHCenter
        text: base.trans(123)
        bcolor: "#41bb1a"
        tcolor: "#ffffff"
        readOnly: true
        font.pixelSize: Math.round(50*base.scalex())
        visible: img.visible
    }

    MText {
        x: 0
        y: Math.round(1400*base.scaley())
        width: base.getWidth()
        horizontalAlignment: Text.AlignHCenter
        text: base.trans(124)
        bcolor: "#41bb1a"
        tcolor: "#ffffff"
        readOnly: true
        font.pixelSize: Math.round(35*base.scalex())
        visible: img.visible
    }

    MButton {
        x: Math.round(50*base.scalex())
        y:
        {
            if (img.visible) Math.round(1500*base.scalex())
            else 0
        }
        id: loginButton
        text: base.trans(9)
        background:
        {
            if (img.visible) "#ffffff"
            else "#41bb1a"
        }
        color:
        {
            if (img.visible) "#000000"
            else "#ffffff"
        }
        width:
        {
            if (img.visible) Math.round(base.getWidth()-100*base.scalex())
            else Math.round(240*base.scalex())
        }
        Rectangle
        {
            id: outer
            visible: false
            x: Math.round(50*base.scalex())
            z: 1
            anchors.fill: parent
            color: "transparent"
            border.width: Math.ceil(1*base.scalex())
            border.color: "#ffffff"
            radius: Math.round(8*base.scalex())
        }
    }

    MText {
        id: password
        x: Math.round(0*base.scalex())
        y: Math.round(100*base.scaley())
        width: Math.round(360*base.scalex())
        horizontalAlignment: Text.AlignLeft
        text: base.trans(58)
        visible: base.isEncrypted()
        font.pixelSize: Math.round(35*base.scalex())
        onFocusChanged: maskedPassword(this,base.trans(58))
    }
}
