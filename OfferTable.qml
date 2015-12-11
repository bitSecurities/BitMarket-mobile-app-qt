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
import QtQuick.Dialogs 1.1

TableView {
    property bool norec : false
    property bool isbid : false
    property int rowh : Math.round(85*base.scaley())

    id: table
    headerVisible: false
    width: base.getWidth()
    height: Math.round(520*base.scaley())
    TableViewColumn {role: "price"; width: Math.round(280*base.scalex()) }
    TableViewColumn {role: "amount"; width: Math.round(280*base.scalex())   }
    TableViewColumn {role: "sum"; width: Math.round(280*base.scalex()) }
    TableViewColumn {role: "action"; width: Math.round(170*base.scalex()) }
    style: TableViewStyle {
        frame: Rectangle {
            border{
                width: 0
            }
        }
    }
    rowDelegate: Rectangle {
        height: rowh
        color: (styleData.row%2==0)? "#f5f4f2" : "#ffffff"
    }
    itemDelegate: Item {
        property double pasekwidth : 180

        Text {
            width: parent.width
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            text: styleData.value
            font.pixelSize: Math.round(35*base.scalex())
            color: (styleData.selected)? "#888888" : "#000000"
            visible: (styleData.role==="action")? false : true
        } // text
        Image {
            x: Math.round(80*base.scalex())
            y: Math.round(15*base.scaley())
            width: Math.round(53*base.scalex())
            height: Math.round(53*base.scaley())
            fillMode: Image.PreserveAspectFit
            smooth: true
            source: {
                if (styleData.value==="buy") "qrc:///images/buy.png";
                else if (styleData.value==="disabled") "qrc:///images/buy_disabled.png";
                else if (styleData.value==="bin") "qrc:///images/bin.png";
                else ""
            }
            visible: (styleData.role==="action")? true : false
        }
        Rectangle
        {
            x: Math.round(120*base.scalex())
            y: Math.round(65*base.scaley())
            width: Math.round((pasekwidth-pasekwidth*(styleData.value/(table.isbid?base.getmaxbidvalue():base.getmaxaskamount())))*base.scalex())
            height: Math.round(8*base.scaley())
            color: (styleData.row%2==0)? "#ffffff" :"#f4f3f1"
            visible: {
                if (table.isbid) (styleData.role==="sum")
                else (styleData.role==="amount")
            }
        }
        Rectangle
        {
            x: Math.round((120+pasekwidth-pasekwidth*(styleData.value/(table.isbid?base.getmaxbidvalue():base.getmaxaskamount())))*base.scalex())
            y: Math.round(65*base.scaley())
            width: Math.round((pasekwidth*(styleData.value/(table.isbid?base.getmaxbidvalue():base.getmaxaskamount())))*base.scalex())
            height: Math.round(8*base.scaley())
            color: "#41b919"
            visible: {
                if (table.isbid) (styleData.role==="sum")
                else (styleData.role==="amount")
            }
        }
    } // Item
    selectionMode: SelectionMode.SingleSelection
    selection{
       onSelectionChanged: {
            if ((!norec)&&(currentRow<rowCount)&&(currentRow>=0)) mselect(currentRow,this)
        }
    }
}
