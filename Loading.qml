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
    id: loading
    signal show()
    signal hide()
    visible: false
    z: 99

    onShow:
    {
        visible=true
        mouse.enabled=true
    }

    onHide:
    {
        visible=false
        mouse.enabled=false
    }

    Rectangle {
        width: base.getWidth()
        height: base.getHeight()
        color: "#000000"
        opacity: 0.75
        MouseArea {
            id:mouse
            anchors.fill: parent
            enabled:false
        }
    }

    Rectangle {
        x: (base.getWidth()-Math.round(200*base.scalex()))/2
        y: (base.getHeight()-Math.round(200*base.scalex()))/2
        z: 1
        width: Math.round(200*base.scalex())
        height: Math.round(200*base.scalex())
        color: "transparent"
        AnimatedImage{
            id: title
            x: Math.round(20*base.scalex())
            width: parent.width
            height: parent.height
            source: "qrc:///images/loading.gif"
        }
    }
}
