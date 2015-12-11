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
import QtQuick.Controls.Styles 1.3

TextField {
    id: text
    x: 0
    y: 0
    width: Math.round(240*base.scalex())
    height: Math.round(85*base.scaley())
    horizontalAlignment: Text.AlignRight
    font.pixelSize: Math.round(35*base.scalex())
    style: TextFieldStyle {
        textColor: "#000000"
        background: Rectangle {
            border.width: 0
            color: "#ffffff"
        }
    }
    Menu {
        id: myContextMenu
        title: "Edit"

        MenuItem {
            text: "Paste"
            onTriggered: paste()
        }

        MenuItem {
            text: "Copy"
            onTriggered: copy()
        }

        MenuItem {
            text: "Cut"
            onTriggered: cut()
        }
    }
    MouseArea {
        anchors.fill: text
        hoverEnabled: true
        onPressed: text.forceActiveFocus();
        onPressAndHold: myContextMenu.popup()
    }
}
