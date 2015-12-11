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
import QtQuick.Controls.Styles 1.3

Frame {
    property int skip: 3

    onUpdate: {
        if (skip<3) skip++
        else base.loadLast()
    }

    onNewdata: {
        var scroll,flick

        scroll=lasttradestable.__verticalScrollBar.value
        flick=lasttradestable.flickableItem.verticalVelocity
        base.updateLast()
        lasttradestable.__verticalScrollBar.value=scroll
        lasttradestable.flickableItem.cancelFlick()
        if (flick!==0) lasttradestable.flickableItem.flick(0,-flick)
        refresh()
    }

    ListModel {
        id: rangemodel
        ListElement {
            name: "---"
        }
        ListElement {
            name: "30 m"
        }
        ListElement {
            name: "90 m"
        }
        ListElement {
            name: "4 h"
        }
        ListElement {
            name: "24 h"
        }
        ListElement {
            name: "48 h"
        }
    }

    MListButton {
        x: Math.round(420*base.scalex())
        y: Math.round(250*base.scaley())
        z: 10
        id: ranges
        model: rangemodel
        inner.border.width: 0
        inner.radius: 0
        onNameChanged: {
            base.changeRange(name)
        }
    }

    TableView {
        id: lasttradestable

        visible: true
        headerVisible: false
        width: base.getWidth()
        height: Math.round(1300*base.scaley())
        y: Math.round(400*base.scaley())
        TableViewColumn {role: "time"; width: Math.round(225*base.scalex()) }
        TableViewColumn {role: "type"; width: Math.round(160*base.scalex()) }
        TableViewColumn {role: "price"; width: Math.round(160*base.scalex()) }
        TableViewColumn {role: "amount"; width: Math.round(230*base.scalex()) }
        TableViewColumn {role: "total"; width: Math.round(185*base.scalex()) }
        model: modellast
        style: TableViewStyle {
            frame: Rectangle {
                border{
                    width: 0
                }
            }
        }
        rowDelegate: Rectangle {
            height: Math.round(80*base.scaley())
            color: (styleData.row%2==0)? "#e9f3fd" : "white"
        }
        itemDelegate: Item {
            Text {
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: {
                    if (styleData.role==="time") return(Text.AlignLeft)
                    else if (styleData.role==="type") return(Text.AlignHCenter)
                    else return(Text.AlignRight)
                }
                text: styleData.value
                font.pixelSize: Math.round(25*base.scalex())
                color: "#000000"
            } // text
        } // Item
        selectionMode: SelectionMode.NoSelection
    }
}
