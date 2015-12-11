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
    visible: !base.isLogged()

    MButton {
        x: Math.round(50*base.scalex())
        id: loginButton
        text: base.trans(9)
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
