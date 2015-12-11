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
import QtMultimedia 5.4
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: mainwindow
    color: "#f5f4f2"

    signal handlerLoader(string name)
    signal refresh()
    signal update()
    signal clear()

    property string pass

    function showError()
    {
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
    }

    function masked(object,txt)
    {
        if ((object.text===txt)&&(object.focus)) object.text=""
        if ((object.text==="")&&(!object.focus)) object.text=txt
    }

    function maskedPassword(object,txt)
    {
        if ((object.text===txt)&&(object.focus)) {
            object.echoMode=TextInput.Password
            object.text=""
        }
        if ((object.text==="")&&(!object.focus)) {
            object.echoMode=TextInput.Normal
            object.text=txt
        }
    }

    function parse(tag)
    {
        var i,j

        if (pageLoader.item===null) return
        i=tag.search("BitmarketKey:")
        j=tag.search(",")
        if ((i<0)||(j<0)) return
        pageLoader.item.fields.key.text=tag.substring(i+13,j)
        i=tag.search("BitmarketSecret:")
        j=tag.length
        pageLoader.item.fields.secret.text=tag.substring(i+16,j)
        base.setKey(pageLoader.item.fields.key.text)
        base.setSecret(pageLoader.item.fields.secret.text)
        base.savekeys()
    }

    function changeScreen(k)
    {
        menuWindow.activek=k
        switch(k)
        {
        case 0:
            ex.visible=true
            markets.visible=true
            //            pageLoader.source="qrc:///Leverage.qml"
            pageLoader.source="qrc:///Trade.qml"
            break
        case 1:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Last.qml"
            break
        case 2:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Orders.qml"
            break
        case 3:
            ex.visible=true
            markets.visible=false
            pageLoader.source="qrc:///Deposit.qml"
            break
        case 4:
            ex.visible=true
            markets.visible=false
            pageLoader.source="qrc:///Withdraw.qml"
            break
        case 5:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Alerts.qml"
            break
        case 6:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Chart.qml"
            break
        case 7:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Leverage.qml"
            break
        case 8:
            ex.visible=true
            markets.visible=true
            pageLoader.source="qrc:///Swap.qml"
            break
        case 9:
            ex.visible=true
            markets.visible=false
            pageLoader.source="qrc:///Keys.qml"
            break
        case 10:
            ex.visible=false
            markets.visible=false
            pageLoader.source="qrc:///Settings.qml"
            break
        case 11:
            Qt.quit()
            break
        }
        menuWindow.visible=false
        touch.z=-2
    }

    StatusBar {
        style: StatusBarStyle {
            background: Rectangle {
                color: "#41bb1a"
            }
        }
    }

    Item
    {
        id: view
        anchors.fill: parent

        MDialog {
            id: errorDialog
            title: base.trans(114)
            text: ""
            type: "ok"
        }

        MDialog {
            id: executeConfirmation
            title: base.trans(20)
            text: ""
            type: "execute"
            onAccepted: {
                console.log("Accepted")
                pageLoader.item.accepted()
            }
            onRejected: {
                console.log("Rejected")
                pageLoader.item.rejected()
            }
        }

        MDialog {
            id: swapConfirmation
            title: base.trans(20)
            text: ""
            type: "execute"
            onAccepted: {
                console.log("Accepted")
                pageLoader.item.accepted()
            }
            onRejected: {
                console.log("Rejected")
                pageLoader.item.rejected()
            }
        }

        MDialog {
            id: withdrawalConfirmation
            title: base.trans(20)
            text: ""
            type: "execute"
            onAccepted: {
                console.log("Accepted")
                pageLoader.item.accepted()
            }
            onRejected: {
                console.log("Rejected")
                pageLoader.item.rejected()
            }
        }

        MDialog {
            id: executeLeverageConfirmation
            title: base.trans(20)
            text: ""
            type: "execute"
            onAccepted: {
                console.log("Accepted")
                pageLoader.item.accepted()
            }
            onRejected: {
                console.log("Rejected")
                pageLoader.item.rejected()
            }
        }

        MDialog {
            id: modifyLeverageConfirmation
            title: base.trans(20)
            text: ""
            type: "execute"
            onAccepted: {
                console.log("Accepted2")
                pageLoader.item.accepted2()
            }
            onRejected: {
                console.log("Rejected2")
                pageLoader.item.rejected2()
            }
        }

        Image
        {
            x: Math.round(40*base.scalex())
            y: Math.round(40*base.scaley())
            z: 11
            source: "qrc:///images/menu.png"
            width: Math.round(80*base.scalex())
            height: Math.round(54*base.scaley())
        }

        Rectangle
        {
            property int activek

            id: menuWindow
            visible: false
            anchors.fill: parent
            color: "#41bb19"
            opacity: 0.97
            z: 10
            activek: 0

            Login
            {
                id: loginfield
                visible: !base.isLogged()
                x: Math.round(110*base.scalex())
                y: Math.round(25*base.scaley())
                z: 15
                button.width: Math.round(260*base.scalex())
                outer: true
            }

            MainMenuItem
            {
                id: menu0
                k: 0
                source: "qrc:///images/trade.png"
            }

            MainMenuItem
            {
                id: menu1
                k: 1
                source: "qrc:///images/last.png"
            }

            MainMenuItem
            {
                id: menu2
                k: 2
                source: "qrc:///images/orders.png"
            }

            MainMenuItem
            {
                id: menu3
                k: 3
                source: "qrc:///images/payin.png"
            }

            MainMenuItem
            {
                id: menu4
                k: 4
                source: "qrc:///images/payout.png"
            }

            MainMenuItem
            {
                id: menu5
                k: 5
                source: "qrc:///images/alerts.png"
            }

            MainMenuItem
            {
                id: menu6
                k: 6
                source: "qrc:///images/charts.png"
            }

            MainMenuItem
            {
                id: menu7
                k: 7
                source: "qrc:///images/leverage.png"
            }

            MainMenuItem
            {
                id: menu8
                k: 8
                source: "qrc:///images/swaps.png"
            }

            MainMenuItem
            {
                id: menu9
                k: 9
                source: "qrc:///images/keys.png"
            }

            MainMenuItem
            {
                id: menu10
                k: 10
                source: "qrc:///images/settings.png"
            }

            MainMenuItem
            {
                id: menu11
                k: 11
                source: "qrc:///images/exit.png"
            }

            Rectangle
            {
                y: Math.round((250+125*parent.activek)*base.scaley())
                width: Math.round(11*base.scalex())
                height: Math.round(125*base.scaley())
                color: "#ffffff"
            }
        }

        Component.onCompleted: {
            changeScreen(0)
        }

        MultiPointTouchArea {
            id: touch
            anchors.fill: parent
            z: -2
            touchPoints: [
                TouchPoint { id: point1 },
                TouchPoint { id: point2 }
            ]
            onPressed:
            {
                //base.logError(point1.x+" "+point1.y+"\n")
                if ((point1.y<85*base.scaley())&&(!menuWindow.visible))
                {
                    menuWindow.visible=true
                    loginfield.visible=!base.isLogged()
                    z=12;
                }
                else if (menuWindow.visible)
                {
                    var i

                    for(i=0;i<12;i++)
                    {
                        if ((point1.y>=(285+125*i)*base.scaley())&&(point1.y<=(285+125*(i+1))*base.scaley())) break;
                    }
                    if (i<12)
                    {
                        changeScreen(i)
                    }
                    else if ((point1.y<85*base.scaley())&&(!base.isLogged())&&(point1.x>120*base.scaley())&&(point1.x<400*base.scaley()))
                    {
                        menuWindow.visible=false
                        z=-2;
                        pass=loginfield.text
                        pageLoader.item.login()
                    }
                    else if ((point1.y>85*base.scaley())&&(point1.y<250*base.scaley())&&(!base.isLogged())&&(base.isEncrypted())&&(point1.x>50*base.scaley())&&(point1.x<600*base.scaley()))
                    {
                        loginfield.password.forceActiveFocus();
                    }
                    else
                    {
                        menuWindow.visible=false
                        z=-2;
                    }
                }
            }
        }

        //ColumnLayout
        Item
        {
            //anchors.fill: parent
            //height: base.getHeight()

            Rectangle
            {
                x: 0
                y: 0
                width: base.getWidth()
                height: Math.round(135*base.scaley())
                color: "#41bb1a"

                InfoBanner {
                    id: messages
                    objectName: "messages"
                }

                Item {
                    id: newdata
                    objectName: "newdata"
                    function refreshdata()
                    {
                        pageLoader.item.newdata()
                    }
                }

                Timer {
                    id: loadInitial
                    running: false
                    repeat: false
                    onTriggered: {
                        base.changeEx("Bitmarket")
                        base.checkAlerts()
                        if (pageLoader.item!=null) {
                            pageLoader.item.clear()
                            pageLoader.item.refresh()
                        }
                        loadUpdate.start()
                    }
                }

                Timer {
                    id: loadUpdate
                    running: false
                    repeat: true
                    interval: 5000
                    onTriggered: {
                        base.checkAlerts()
                        update()
                        pageLoader.item.update()
                    }
                }

                MListButton {
                    id: ex

                    property bool first:true

                    x: Math.round(160*base.scalex())
                    y: Math.round(25*base.scaley())
                    z: 10
                    model: modelexchanges
                    bcolor: "transparent"
                    inner.color: "#ffffff"

                    onNameChanged: {
                        if (!first)
                        {
                            base.changeEx(name)
                            markets.popup.view.currentIndex=0
                            markets.popup.ref()
                            if (pageLoader.item!=null) {
                                pageLoader.item.clear()
                                pageLoader.item.refresh()
                            }
                        }else {
                            loadInitial.start()
                            first=false
                        }
                        if (name==="Bitmarket") image="qrc:///images/bitmarket.png"
                        else if (name==="Bitmaszyna") image="qrc:///images/bitmaszyna.png"
                    }
                }

                MListButton {
                    id: markets

                    property bool first:true

                    x: Math.round(450*base.scalex())
                    y: Math.round(25*base.scaley())
                    z: 10
                    model: modelmarkets
                    onNameChanged: {
                        if (!first)
                        {
                            base.changeMarket(name)
                            if (pageLoader.item!=null) {
                                pageLoader.item.refresh()
                                pageLoader.item.clear()
                            }
                        }else first=false
                    }
                }
            }

            Loader {
                id: pageLoader
                z: -1
                Layout.fillHeight: true
            }

            Connections {
                target: pageLoader.item
                onHandlerLoader: {
                    pageLoader.source=name;
                }
            }
        }
    }
}
