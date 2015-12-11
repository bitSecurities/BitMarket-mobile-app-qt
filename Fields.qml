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

Item {
    id: fields
    property alias action : action
    property alias actionExternal : actionExternal
    property alias key : key
    property alias secret : secret

    function up()
    {
        key.text=base.getKey()
        if (key.text!=="") secret.text="***"
        else {
            key.text=base.trans(15)
            secret.text=base.trans(16)
        }
    }

    MText {
        id: key
        x:Math.round(40*base.scalex())
        y:Math.round(200*base.scaley())
        width: Math.floor(1000*base.scalex())
        horizontalAlignment: Text.AlignLeft
        text: base.trans(15)
        font.pixelSize: Math.round(35*base.scalex())
        onFocusChanged: masked(this,base.trans(15))
    }

    MText {
        id: secret
        x:Math.round(40*base.scalex())
        y:Math.round(300*base.scaley())
        width: Math.round(1000*base.scalex())
        horizontalAlignment: Text.AlignLeft
        text: base.trans(16)
        font.pixelSize: Math.round(35*base.scalex())
        onFocusChanged: masked(this,base.trans(16))
    }

    MButton {
        id: saveKeys
        x: Math.round(420*base.scalex())
        y: Math.round(450*base.scaley())
        text: base.trans(51)
        onClicked: {
            base.setKey(key.text)
            if (secret.text!=="***") base.setSecret(secret.text)
            messages.displayMessage(base.trans(68));
        }
    }

    MButton {
        id: action
        x: Math.round(50*base.scalex())
        y: Math.round(600*base.scaley())
        text: base.trans(17)
        onClicked: null
    }

    MButton {
        id: actionExternal
        x: Math.round(750*base.scalex())
        y: Math.round(600*base.scaley())
        width: Math.round(300*base.scalex())
        text: base.trans(90)
        onClicked: null
    }
}
