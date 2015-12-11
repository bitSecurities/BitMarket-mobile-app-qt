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
import QtQuick.Dialogs 1.1
import Wykres 1.0

Frame {    
    /*
    MultiPointTouchArea {
        anchors.fill: parent
        touchPoints: [
            TouchPoint { id: point1 },
            TouchPoint { id: point2 }
        ]
        onUpdated:
        {

        }
    }
*/
    onRefresh: {
        base.loadLast()
        chart.update()
        if (chart.rotation==0)
        {
            if (timeframe.name==="1 m") range.text="40 m"
            else if (timeframe.name==="4 m") range.text="2 h 20 m"
            else if (timeframe.name==="16 m") range.text="9 h 20 m"
            else if (timeframe.name==="2 h") range.text="3 d 8 h"
            else if (timeframe.name==="8 h") range.text="13 d 8 h"
            else if (timeframe.name==="1 d") range.text="40 d"
            else if (timeframe.name==="2 d") range.text="80 d"
            else if (timeframe.name==="4 d") range.text="160 d"
        }else
        {
            if (timeframe.name==="1 m") range.text="1 h"
            else if (timeframe.name==="4 m") range.text="4 h"
            else if (timeframe.name==="16 m") range.text="16 h"
            else if (timeframe.name==="2 h") range.text="5 d"
            else if (timeframe.name==="8 h") range.text="20 d"
            else if (timeframe.name==="1 d") range.text="60 d"
            else if (timeframe.name==="2 d") range.text="120 d"
            else if (timeframe.name==="4 d") range.text="240 d"
        }
    }

    onUpdate: {
        refresh()
    }

    ListModel {
        id: modeltimeframes

        ListElement {
            name: "1 m"
        }
        ListElement {
            name: "4 m"
        }
        ListElement {
            name: "16 m"
        }
        ListElement {
            name: "2 h"
        }
        ListElement {
            name: "8 h"
        }
        ListElement {
            name: "1 d"
        }
        ListElement {
            name: "2 d"
        }
        ListElement {
            name: "4 d"
        }
    }

    MButton
    {
        x: Math.round(20*base.scalex())
        y: Math.round(200*base.scaley())
        text: base.trans(111)
        onClicked:{
            if (chart.rotation==0) {
                chart.x=Math.round(-130*base.scalex())
                chart.y=Math.round(480*base.scalex())
                chart.width=Math.round(base.getHeight()-Math.round(500*base.scaley()))
                chart.height=Math.round(base.getWidth()-Math.round(20*base.scalex())*2)
                chart.rotation=90
                base.rotate(chart.rotation)
                chart.update()
                refresh()
            }
            else {
                chart.x=Math.round(20*base.scalex())
                chart.y=Math.round(320*base.scalex())
                chart.width=Math.round(base.getWidth()-Math.round(20*base.scalex())*2)
                chart.height=Math.round(base.getHeight()-Math.round(420*base.scaley()))
                chart.rotation=0
                base.rotate(chart.rotation)
                chart.update()
                refresh()
            }
        }
    }

    MListButton {
        id: timeframe

        property bool first:true

        x: Math.round(420*base.scalex())
        y: Math.round(200*base.scaley())
        z: 10
        view.currentIndex: 1
        model: modeltimeframes
        inner.border.width: 0
        inner.radius: 0
        onNameChanged: {
            if (!first)
            {
                base.changeTimeFrame(name)
                chart.update()
                refresh()
            }else first=false
        }
    }

    Text
    {
        id: range
        x: Math.round(820*base.scalex())
        y: Math.round(200*base.scaley())
        width : Math.round(240*base.scalex())
        height: Math.round(85*base.scaley())
        font.pixelSize: Math.round(35*base.scalex())
        color: "#000000"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Wykres {
        id: chart
        x: Math.round(20*base.scalex())
        y: Math.round(320*base.scaley())
        width: Math.round(base.getWidth()-x*2)
        height: Math.round(base.getHeight()-y-100*base.scaley())
    }

    Component.onCompleted: {
        chart.update()
        base.changeTimeFrame("4 m")
    }
}
