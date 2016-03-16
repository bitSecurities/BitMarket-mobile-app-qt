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
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Item {
    id: dialog
    signal accepted()
    signal rejected()
//    property alias title: title.text
    property alias text: text.text
    property string type
    visible: false
    z: 99

    function touch()
    {
        dialog.visible=false
    }

    Rectangle {
        width: base.getWidth()
        height: base.getHeight()
        color: "#000000"
        opacity: 0.75
    }

    Rectangle {
        x: Math.round(40*base.scalex())
        y: Math.round(20*base.scalex())
        z: 1
        width: Math.round(1000*base.scalex())
        height: Math.round(1710*base.scalex())
        color: "#ffffff"
        Text{
            id: title
            x: Math.round(20*base.scalex())
            width: parent.width-Math.round(40*base.scalex())
            height: Math.round(170*base.scalex())
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: Math.round(45*base.scalex())
            color: "#41bb1a"
            text: base.trans(121)
        }
        Rectangle {
            y: Math.round(170*base.scalex())
            z: 1
            width: parent.width
            height: parent.height-y
            color: "#41bb1a"
            Text {
                id: text
                x: Math.round(20*base.scalex())
                //y: Math.round(100*base.scaley())
                width: parent.width-Math.round(35*base.scalex())
                height: parent.height-Math.round(200*base.scalex())
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                font.pixelSize: Math.round(34*base.scalex())
                color: "#ffffff"
                text: base.trans(122)
            }
            MButton {
                x: Math.round(380*base.scalex())
                y: Math.round(1320*base.scalex())
                height: Math.round(140*base.scaley())
                text: "OK"
                background: "#ffffff"
                color: "#41bb1a"
                visible: true
                onClicked: {
                    touch()
                    accepted()
                }
            }
        }
    }
}
