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
import QtQuick.Layouts 1.1

Frame {
    property double bidScroll
    property double askScroll
    property double bidFlick
    property double askFlick
    property bool newpos

    property bool side
    property bool reverse
    property int row

    onClear: {
        reverse=base.getReverse();
        tradeClear()
        if (reverse) {
            asktable.positionViewAtRow(asktable.rowCount-1, ListView.Contain)
            newpos=true
        }
    }

    onRefresh: {
        account.visible=(base.isLogged())? true : false
        loginfield2.visible=(base.isLogged())? false : true
        account.fees.text=base.getFees()
        getMarketInfo()
    }

    onUpdate: {
        base.getdepth()
    }

    onNewdata: {
        newpos=false
        bidScroll=bidtable.__verticalScrollBar.value
        askFlick=asktable.flickableItem.verticalVelocity
        bidFlick=bidtable.flickableItem.verticalVelocity
        if (asktable.__verticalScrollBar.value>asktable.__verticalScrollBar.maximumValue-200) newpos=true
        askScroll=asktable.__verticalScrollBar.value
        base.updateTables()
        bidtable.__verticalScrollBar.value=bidScroll
        if ((newpos)&&(reverse)) asktable.positionViewAtRow(asktable.rowCount-1, ListView.Contain)
        else asktable.__verticalScrollBar.value=askScroll
        asktable.flickableItem.cancelFlick()
        if (askFlick!=0) asktable.flickableItem.flick(0,-askFlick)
        bidtable.flickableItem.cancelFlick()
        if (bidFlick!=0) bidtable.flickableItem.flick(0,-bidFlick)
        mselect(3)
        refresh()
    }

    onAccepted:{
        if (!base.execute(price.text,amount.text,!buy.checked))
        {
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }else
        {
            base.getFunds()
            base.getdepth()
        }
        refresh()
    }

    /*MDialog {
        id: executeConfirmation
        //title: base.trans(20)
        //text: ""
        //standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            console.log("Accepted")
            if (!base.execute(price.text,amount.text,!buy.checked))
            {
                errorDialog.text=base.getLastError()
                errorDialog.visible=true
            }
            refresh()
        }
        //onRejected: {
        //    console.log("Rejected")
        //}
        //visible: false
    }*/

    function makeLogin() {
        bidScroll=bidtable.__verticalScrollBar.value
        askScroll=asktable.__verticalScrollBar.value
        if (base.login(pass)) {
            loginfield2.visible=false
            account.fees.text=base.getFees()
            account.visible=true
            bidtable.__verticalScrollBar.value=bidScroll
            asktable.__verticalScrollBar.value=askScroll
            refresh()
        }else {
            errorDialog.title=base.trans(18)
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
            refresh()
        }
    }

    Component.onCompleted: clear()

    function tablesClear()
    {
        bidtable.norec=true
        bidtable.selection.clear()
        bidtable.norec=false
        asktable.norec=true
        asktable.selection.clear()
        asktable.norec=false
    }

    function tradeClear()
    {
        tablesClear()
        order.price.text=base.trans(13)
        order.amount.text=base.trans(14)
    }

    function makeselect(type)
    {
        if ((buy.checked)&&(!sell.checked)) side=true
        else if ((!buy.checked)&&(sell.checked)) side=false
        else return
        if (base.isLogged())
        {
            if (side)
            {
                if ((order.price.text>0)&&(order.amount.text*order.price.text>modelbalance.get(base.getCurrId(2))['value']-0.01))
                {
                    order.amount.text=((modelbalance.get(base.getCurrId(2))['value']-0.01)/order.price.text).toFixed(8)
                }
            }else if (!side)
            {
                if (order.amount.text>modelbalance.get(base.getCurrId(1))['value'])
                {
                    order.amount.text=modelbalance.get(base.getCurrId(1))['value']
                }
            }
        }
        row=base.getRow(order.price.text,order.amount.text,side)
        if (row<0)
        {
            tablesClear()
            return
        }
        if (side)
        {
            tablesClear()
            asktable.norec=true
            asktable.selection.select(row,asktable.rowCount-1)
            asktable.norec=false
        }else if (!side)
        {
            tablesClear()
            bidtable.norec=true
            bidtable.selection.select(0,row)
            bidtable.norec=false
        }

    }

    function mselect(row,table)
    {
        if (table===asktable)
        {
            buy.checked=true
            sell.checked=false
            order.price.text=base.buyPrice(row);
            order.amount.text=base.buyAmount(row);
        }else if (table===bidtable)
        {
            sell.checked=true
            buy.checked=false
            order.price.text=base.sellPrice(row)
            order.amount.text=base.sellAmount(row)
        }
        makeselect(3);
    }

    function getMarketInfo()
    {
        marketInfo.buyprice.infodown.text=base.getMarketInfo(0)
        marketInfo.sellprice.infodown.text=base.getMarketInfo(1)
        marketInfo.lastprice.infodown.text=base.getMarketInfo(2)
        marketInfo.maxprice.infodown.text=base.getMarketInfo(3)
        marketInfo.minprice.infodown.text=base.getMarketInfo(4)
        marketInfo.volume.infodown.text=base.getMarketInfo(5)
    }

    ColumnLayout {
        id: tradeFrame
        y: Math.round(130*base.scaley())

        Rectangle
        {
            y: Math.round(135*base.scaley())
            height: Math.round(1135*base.scaley())

            OfferTable
            {
                id: asktable
                model: modelask
            }

            ExclusiveGroup { id: ttype }

            Rectangle
            {
                id: order

                property alias price : price
                property alias amount : amount
                property alias buy : buy
                property alias sell : sell

                x: 0
                y: Math.round(520*base.scaley())
                Rectangle
                {
                    x: Math.round(10*base.scalex())
                    y: Math.round(10*base.scaley())
                    width: Math.round(152*base.scalex())
                    height: Math.round(85*base.scaley())
                    color: "#41be1c"
                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: Math.round(60*base.scalex())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: base.trans(10)
                        font.pixelSize: Math.round(30*base.scalex())
                        color: "#ffffff"
                    }
                    RadioButton {
                        id:buy
                        anchors.leftMargin: Math.round(5*base.scaley())
                        anchors.fill: parent
                        exclusiveGroup: ttype
                        checked: true
                        onCheckedChanged: mselect(0)
                        style: RadioButtonStyle{}
                    }
                }

                Rectangle
                {
                    x: Math.round(172*base.scalex())
                    y: Math.round(10*base.scaley())
                    width: Math.round(152*base.scalex())
                    height: Math.round(85*base.scaley())
                    color: "#cf0302"
                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: Math.round(60*base.scalex())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: base.trans(11)
                        font.pixelSize: Math.round(30*base.scalex())
                        color: "#ffffff"
                    }
                    RadioButton {
                        id:sell
                        anchors.leftMargin: Math.round(5*base.scaley())
                        anchors.fill: parent
                        exclusiveGroup: ttype
                        onCheckedChanged: mselect(0)
                        style: RadioButtonStyle{}
                    }
                }

                MText {
                    id: price
                    x: Math.round(334*base.scalex())
                    y: Math.round(10*base.scaley())
                    width: Math.round(238*base.scalex())
                    height: Math.round(85*base.scaley())
                    text: base.trans(13)
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Math.round(30*base.scalex())
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onFocusChanged: masked(this,base.trans(13))
                    onTextChanged: mselect(1)
                }

                MText {
                    id: amount
                    x: Math.round(582*base.scalex())
                    y: Math.round(10*base.scaley())
                    width: Math.round(238*base.scalex())
                    height: Math.round(85*base.scaley())
                    text: base.trans(14)
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Math.round(30*base.scalex())
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onFocusChanged: masked(this,base.trans(14))
                    onTextChanged: mselect(2)
                }

                MButton {
                    x: Math.round(830*base.scalex())
                    y: Math.round(10*base.scaley())
                    text: base.trans(12)
                    onClicked: {                        
                        if ((base.isLogged())&&(price.text>0)&&(amount.text>0))
                        {
                            if ((base.getFirstCurrency()==="BTC")&&(amount.text<0.005))
                            {
                                errorDialog.text=base.trans(115)
                                errorDialog.visible=true
                            }else
                            {
                                executeConfirmation.text=base.trans(21)+" "+(buy.checked?base.trans(23):base.trans(24))+" "+amount.text+" "+base.getFirstCurrency()+" "+base.trans(22)+" "+price.text+" ?"
                                executeConfirmation.visible=true
                            }
                        }
                    }
                }
            }

            OfferTable
            {
                id: bidtable
                isbid: true
                y: Math.round(625*base.scaley())
                model: modelbid
            }
        }

        MarketInfo
        {
            id: marketInfo
        }

        Login{
            id: loginfield2
            x: Math.round(370*base.scalex())
            y: Math.round(1500*base.scaley())
            button.onClicked: {
                pass=loginfield2.text
                login()
            }
        }

        Rectangle
        {
            id: account

            property alias balance : balance
            property alias fees : fees

            y: Math.round(1370*base.scaley())
            x: 0
            visible: (base.isLogged())? true : false
            width: base.getWidth()
            height: Math.round(300*base.scalex())
            color: "#f5f4f2"

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                id: fees
                font.pixelSize: Math.round(35*base.scalex())
                text: base.getFees()
            }

            TableView {
                property bool norec : false

                id: balance
                width: parent.width
                height: Math.round(68*4*base.scaley())
                //Layout.fillHeight: true
                x: 0
                y: Math.round(55*base.scaley())
                TableViewColumn {role: "currency"; width: Math.round(140*base.scalex()); title: base.trans(98)}
                TableViewColumn {role: "available"; width: Math.round(290*base.scalex()); title: base.trans(6) }
                TableViewColumn {role: "blocked"; width: Math.round(290*base.scalex()); title: base.trans(7) }
                TableViewColumn {role: "total"; width: Math.round(290*base.scalex()); title: base.trans(8) }
                model: modelbalance
                style: TableViewStyle {
                    frame: Rectangle {
                        border{
                            width: 0
                        }
                    }
                }
                headerDelegate:Rectangle {
                    height: 68*base.scaley()
                    width: parent.width
                    color: "#f5f4f2"
                    Rectangle
                    {
                        x: Math.round(5*base.scalex())
                        y: Math.round(5*base.scalex())
                        height: 58*base.scaley()
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
                    height: 68*base.scaley()
                    color: (styleData.row%2==0)? "#ffffff" :"#f4f3f1"
                }
                itemDelegate: Item {
                    Text {
                        width: parent.width
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value
                        font.pixelSize: Math.round(35*base.scalex())
                        color: "#000000"
                    } // text
                } // Item
                selectionMode: SelectionMode.NoSelection
            }
        }
    }
}
