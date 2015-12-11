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

Frame {
    id: swapFrame

    onRefresh: {
        if (base.isLogged()) base.swapList(swapcurrency.name)
        if (ex.name==="Bitmaszyna")
        {
            loginField.visible=false
            swapsFrame.visible=false
            info.visible=true
        }else {
            loginField.visible=!base.isLogged()
            swapsFrame.visible=base.isLogged()
            info.visible=false
        }
    }

    onUpdate: {
        refresh()
    }

    onAccepted: {
        if (!base.swapClose(swapcurrency.name,swaptable.lastTid))
        {
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
        refresh()
    }

    function makeLogin()
    {
        if (base.login(pass)) {
            loginField.visible=false
            swapsFrame.visible=true
            refresh()
        }else {
            refresh()
            errorDialog.title=base.trans(18)
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
    }

    Component.onCompleted: {
        refresh()
    }

    ListModel {
        id: modelcurrency
        ListElement {
            name: "BTC"
        }
    }

    Text
    {
        id: info
        x:Math.round(20*base.scalex())
        y:Math.round(800*base.scaley())
        width: Math.round(base.getWidth()-40*base.scalex())
        text: base.trans(84)
        font.pixelSize: Math.round(35*base.scalex())
        horizontalAlignment: Text.AlignHCenter
        visible: false
    }

    Login {
        id: loginField
        x: Math.round(370*base.scalex())
        y: Math.round(600*base.scaley())
        button.onClicked: {
            pass=loginField.text
            login()
        }
    }

    Item
    {
        id : swapsFrame
        visible: base.isLogged()

        MListButtonN {
            id: swapcurrency
            x: Math.round(20*base.scalex())
            y: Math.round(200*base.scaley())
            z: 10
            model: modelcurrency
            onNameChanged: null
        }

        MText {
            id: price
            x:Math.round(20*base.scalex())
            y:Math.round(400*base.scaley())
            width: Math.round(600*base.scalex())
            text: base.trans(101)
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            font.pixelSize: Math.round(35*base.scalex())
            onFocusChanged: masked(this,base.trans(101))
        }

        MText {
            id: amount
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
            width: Math.round(600*base.scalex())
            text: base.trans(14)
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            font.pixelSize: Math.round(35*base.scalex())
            onFocusChanged: masked(this,base.trans(14))
        }

        MButton {
            id: action
            x: Math.round(20*base.scalex())
            y: Math.round(800*base.scaley())
            text: base.trans(12)
            onClicked: {
                if (!base.swapOpen(swapcurrency.name,price.text,amount.text)) showError()
                refresh()
            }
        }

        TableView {
            property bool norec : false
            property string lastTid

            id: swaptable
            width: base.getWidth()
            height: Math.round(600*base.scaley())
            y: Math.round(1000*base.scaley())
            TableViewColumn {role: "price"; width: Math.round(350*base.scalex()); title: base.trans(101) }
            TableViewColumn {role: "amount"; width: Math.round(350*base.scalex()); title: base.trans(14) }
            TableViewColumn {role: "earnings"; width: Math.round(350*base.scalex()); title: base.trans(102) }
            model: modelswaps
            style: TableViewStyle {
                frame: Rectangle {
                    border{
                        width: 0
                    }
                }
            }
            headerDelegate:Rectangle {
                height: Math.round(68*base.scaley())
                width: parent.width
                color: "#f5f4f2"
                Rectangle
                {
                    x: Math.round(5*base.scalex())
                    y: Math.round(5*base.scalex())
                    height: Math.round(58*base.scaley())
                    width: Math.round(parent.width-10*base.scalex())
                    color: "#ffffff"
                }
                Text {
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                } // text
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
                    horizontalAlignment: Text.AlignRight
                    text: styleData.value
                    font.pixelSize: Math.round(35*base.scalex())
                    color: (styleData.selected)? "#888888" : "#000000"
                } // text
            } // Item
            selectionMode: SelectionMode.SingleSelection
            selection{
                onSelectionChanged: {
                    if ((!norec)&&(currentRow<rowCount)&&(currentRow>=0))
                    {
                        lastTid=modelswaps.get(currentRow)["tid"]
                        swapConfirmation.text=base.trans(55)+" "+modelswaps.get(currentRow)["amount"]+" BTC "+base.trans(22)+" "+modelswaps.get(currentRow)["price"]+"?"
                        swapConfirmation.visible=true
                    }
                }
            }
        }
    }
}
