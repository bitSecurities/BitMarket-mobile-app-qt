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
    property int selected
    property bool got
    property string apiFunction

    onRefresh:
    {
        if (base.isLogged()) {
            if (selected===0) {
                if (!got)
                {
                    base.getOpenOrders()
                    got=true
                }else
                {
                    orderstable.visible=true
                    historytable.visible=false
                    withdrawalstable.visible=false
                }
            }
            else if (selected===modelordertype.rowCount()-2)
            {
                orderstable.visible=false
                historytable.visible=false
                withdrawalstable.visible=true
            }
            else {
                //base.getHistory(selected-1)
                orderstable.visible=false
                historytable.visible=true
                withdrawalstable.visible=false
            }
        }
    }

    function apiSuccess()
    {
        if (apiFunction=="cancel")
        {
            apiFunction="orders"
            got=false
            refresh()
        }
        else if (apiFunction=="history")
        {
            apiFunction=""
            base.emitHistoryRefresh();
            refresh()
        }
        else if (selected===0) {
            base.emitRefresh()
            orderstable.visible=true
            orderstable.update()
            historytable.visible=false
            withdrawalstable.visible=false
        }
    }

    function apiFailed()
    {
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
        refresh()
    }

    onUpdate:
    {
        refresh()
    }

    onClear:
    {
        loginField.visible=!base.isLogged()
        ordersFrame.visible=base.isLogged()
        refresh()
    }

    MDialog {
        id: cancelConfirmation
        title: base.trans(25)
        text: ""
        onAccepted: {
            console.log("Accepted")
            apiFunction="cancel";
            base.cancelOrder(orderstable.lastTid)
            refresh()
        }
        onRejected: {
            console.log("Rejected")
        }
        visible: false
    }

    function loginSuccess()
    {
        loginField.visible=false
        ordersFrame.visible=true
        refresh()
    }

    function loginFailed()
    {
        refresh()
        errorDialog.title=base.trans(18)
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
    }

    Component.onCompleted:
    {
        got=false
    }

    Login {
        id: loginField
        img.visible: true
        button.onClicked: {
            pass=loginField.text
            login()
        }
    }

    Item
    {
        id : ordersFrame
        visible: base.isLogged()

        MListButton {
            x: 330*base.scalex()
            y: 250*base.scaley()
            z: 10
            width: 440*base.scalex()
            popup.itemWidth: 440*base.scalex()
            id: ranges
            model: modelordertype
            inner.border.width: 0
            inner.radius: 0
            onNameChanged: {
                selected=view.currentIndex
                if (base.isLogged()) {
                    if(selected!==0) {
                        apiFunction="history"
                        base.getHistory(selected-1)
                    }
                    refresh();
                }
            }
        }

        TableView {
            id: orderstable

            property bool norec : false
            property string lastTid

            headerVisible: false
            width: base.getWidth()
            height: Math.round(1300*base.scaley())
            y: Math.round(400*base.scaley())
            TableViewColumn {role: "type"; width: 140*base.scalex() }
            TableViewColumn {role: "price"; width: 260*base.scalex() }
            TableViewColumn {role: "amount"; width: 260*base.scalex()   }
            TableViewColumn {role: "sum"; width: 260*base.scalex() }
            TableViewColumn {role: "action"; width: 120*base.scalex() }
            model: modelorders
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
                    horizontalAlignment: Text.AlignRight
                    text: ((styleData.value==="bin")? "":styleData.value)+((styleData.role==="type")?":":"")
                    font.pixelSize: Math.round(30*base.scalex())
                    color: "#000000"
                } // text
                Image {
                    x: Math.round(50*base.scalex())
                    y: Math.round(12*base.scaley())
                    width: Math.round(53*base.scalex())
                    height: Math.round(53*base.scaley())
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: (styleData.value==="bin")? "qrc:///images/cancel.png":""
                }
            } // Item
            Component.onCompleted: {
                refresh()
            }
            selectionMode: SelectionMode.SingleSelection
            selection{
                onSelectionChanged: {
                    if ((!norec)&&(currentRow<rowCount)&&(currentRow>=0))
                    {
                        cancelConfirmation.text=base.trans(26)+" "+model.get(currentRow)["tid"]+" "+base.trans(27)+" "+model.get(currentRow)["type"]+" "+model.get(currentRow)["amount"]+" "+base.getFirstCurrency()+" "+base.trans(30)+" "+model.get(currentRow)["price"]+"?"
                        cancelConfirmation.visible=true
                        lastTid=model.get(currentRow)["tid"]
                        norec=true
                        selection.clear()
                        norec=false
                    }
                }
            }
        }

        TableView {
            id: historytable

            property bool norec : false
            property string lastTid

            visible: false
            headerVisible: false
            width: base.getWidth()
            height: Math.round(1300*base.scaley())
            y: Math.round(400*base.scaley())
            TableViewColumn {role: "time"; width: 240*base.scalex() }
            TableViewColumn {role: "type"; width: 260*base.scalex() }
            TableViewColumn {role: "amount"; width: 280*base.scalex()   }
            TableViewColumn {role: "price"; width: 280*base.scalex() }
            model: modelhistory
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
                    horizontalAlignment: (styleData.role==="type")?Text.AlignHCenter:Text.AlignRight
                    text: ((styleData.value==="bin")? "":styleData.value)
                    font.pixelSize: Math.round(30*base.scalex())
                    color: "#000000"
                } // text
            } // Item
            selectionMode: SelectionMode.NoSelection
        }

        TableView {
            id: withdrawalstable

            property bool norec : false
            property string lastTid

            visible: false
            headerVisible: false
            width: base.getWidth()
            height: Math.round(1300*base.scaley())
            y: Math.round(400*base.scaley())
            TableViewColumn {role: "time"; width: 240*base.scalex() }
            TableViewColumn {role: "type"; width: 400*base.scalex() }
            TableViewColumn {role: "amount"; width: 260*base.scalex()   }
            TableViewColumn {role: "price"; width: 100*base.scalex() }
            model: modelhistory
            style: TableViewStyle {
                frame: Rectangle {
                    border{
                        width: 0
                    }
                }
            }
            rowDelegate: Rectangle {
                height: Math.round(160*base.scaley())
                color: (styleData.row%2==0)? "#e9f3fd" : "white"
            }
            itemDelegate: Item {
                Text {
                    width: parent.width
                    height: parent.height/2
                    y: (styleData.role==="type")?parent.height/2:0
                    x: (styleData.role==="type")?-230*base.scalex():0
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: (styleData.role==="type")?Text.AlignHLeft:Text.AlignRight
                    text: styleData.value
                    font.pixelSize: Math.round(30*base.scalex())
                    color: "#000000"
                } // text
            } // Item
            selectionMode: SelectionMode.NoSelection
        }
    }
}
