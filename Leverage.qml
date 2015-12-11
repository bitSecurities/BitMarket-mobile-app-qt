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
    y: Math.round(135*base.scaley())

    property double scroll
    property double flick
    property var currentpos

    onAccepted: {
        if (!base.executeLeverage(price.text,amount.text,buy.checked?0:1,leverage.view.currentIndex,rateProfit.text,rateLoss.text))
        {
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
    }

    onAccepted2: {
        if (close.checked)
        {
            if(!base.marginClose(openPositions.lastTid,modifyAmount.text))
            {
                errorDialog.text=base.getLastError()
                errorDialog.visible=true
            }
        }else if (cancel.checked)
        {
            if (!base.marginCancel(openPositions.lastTid,modifyAmount.text))
            {
                errorDialog.text=base.getLastError()
                errorDialog.visible=true
            }
        }else if (modify.checked)
        {
            if (!base.marginModify(openPositions.lastTid,modifyPrice.text,modifyRateProfit.text,modifyRateLoss.text))
            {
                errorDialog.text=base.getLastError()
                errorDialog.visible=true
            }
        }
        update()
    }

    function updateValues()
    {
        modifyHide()
        if (openPositions.currentRow>=0)
        {
            modifyPrice.text=currentpos["price"]
            modifyRateProfit.text=currentpos["rateProfit"]
            modifyRateLoss.text=currentpos["rateLoss"]
        }
    }

    function checkMax()
    {
        if ((!isNaN(price.text))&&(!isNaN(amount.text)))
        {
            if (amount.text>(base.getAvLeverage()*price.text*base.getLeverage(leverage.view.currentIndex))) amount.text=(base.getAvLeverage()*price.text*base.getLeverage(leverage.view.currentIndex)).toFixed(2);
        }
    }

    function getLeverageInfo()
    {
        leverageAvailable.infodown.text=base.getLeverageInfo(0)
        leverageBlocked.infodown.text=base.getLeverageInfo(1)
        leverageBalance.infodown.text=base.getLeverageInfo(2)
        leverageProfit.infodown.text=base.getLeverageInfo(3)
        leverageProfitPercentage.infodown.text=base.getLeverageInfo(4)
        leverageValue.infodown.text=base.getLeverageInfo(5)
    }

    function getMarketInfo()
    {
        marketinfo.buyprice.infodown.text=base.getMarketInfo(0)
        marketinfo.sellprice.infodown.text=base.getMarketInfo(1)
        marketinfo.lastprice.infodown.text=base.getMarketInfo(2)
        marketinfo.maxprice.infodown.text=base.getMarketInfo(3)
        marketinfo.minprice.infodown.text=base.getMarketInfo(4)
        marketinfo.volume.infodown.text=base.getMarketInfo(5)
    }

    onClear: {
        amount.text=base.trans(14)+" "+base.trans(116)+" "+base.getSecondCurrency()
        update()
    }

    onRefresh: {
        if (base.isLogged()) {
            getLeverageInfo()
            getMarketInfo()
        }
        if (ex.name==="Bitmaszyna")
        {
            loginField.visible=false
            leverageFrame.visible=false
            info.visible=true
        }else {
            loginField.visible=!base.isLogged()
            leverageFrame.visible=base.isLogged()
            info.visible=false
        }
    }

    onUpdate: {
        scroll=openPositions.__verticalScrollBar.value
        flick=openPositions.flickableItem.verticalVelocity
        base.marginList()
        openPositions.__verticalScrollBar.value=scroll
        openPositions.flickableItem.cancelFlick()
        if (flick!=0) openPositions.flickableItem.flick(0,-flick)
        updateValues()
        refresh()
    }

    Component.onCompleted: {
        refresh()
    }

    function modifyHide()
    {
        if (modify.checked)
        {
            modifyAmount.visible=false
            modifyPrice.visible=true
            modifyRateProfit.visible=true
            modifyRateLoss.visible=true
        }else
        {
            modifyAmount.text=openPositions.fiatOpened
            modifyAmount.visible=true
            modifyPrice.visible=false
            modifyRateProfit.visible=false
            modifyRateLoss.visible=false
        }
    }

    Text
    {
        id: info
        x:Math.round(20*base.scalex())
        y:Math.round(800*base.scaley())
        width: Math.round(base.getWidth()-40*base.scalex())
        text: base.trans(84)
        font.pixelSize: Math.round(40*base.scalex())
        horizontalAlignment: Text.AlignHCenter
        visible: false
    }

    function makeLogin()
    {
        if (base.login(pass)) {
            loginField.visible=false
            leverageFrame.visible=true
            refresh()
        }else {
            refresh()
            errorDialog.title=base.trans(18)
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
    }

    Login {
        id: loginField
        x: Math.round(370*base.scalex())
        y: Math.round(300*base.scaley())
        button.onClicked: {
            pass=loginField.text
            login()
        }
    }

    Item
    {
        id : leverageFrame
        visible: base.isLogged()

        Rectangle
        {
            y: Math.round(10*base.scaley())

            InfoBox
            {
                id: leverageAvailable
                x: Math.round(10*base.scalex())
                color: "#41bb1a"
                infoup.text: base.trans(6)+":"
            }

            InfoBox
            {
                id: leverageBlocked
                x: Math.round(367*base.scalex())
                color: "#d00403"
                infoup.text: base.trans(7)+":"
            }

            InfoBox
            {
                id: leverageBalance
                x: Math.round(724*base.scalex())
                infoup.color: "#000000"
                infodown.color: "#000000"
                infoup.text: base.trans(105)+":"
            }

            InfoBox
            {
                id: leverageProfit
                x: Math.round(10*base.scalex())
                y: Math.round(110*base.scaley())
                infoup.color: "#000000"
                infodown.color: "#000000"
                infoup.text: base.trans(46)+":"
            }

            InfoBox
            {
                id: leverageProfitPercentage
                x: Math.round(367*base.scalex())
                y: Math.round(110*base.scaley())
                infoup.color: "#000000"
                infodown.color: "#000000"
                infoup.text: base.trans(95)+":"
            }

            InfoBox
            {
                id: leverageValue
                x: Math.round(724*base.scalex())
                y: Math.round(110*base.scaley())
                infoup.color: "#000000"
                infodown.color: "#000000"
                infoup.text: base.trans(96)+":"
            }
        }

        MarketInfo{
            id: marketinfo
            y: Math.round(230*base.scaley())
        }

        Rectangle {
            y: Math.round(450*base.scaley())

            MText {
                id: marginFunds
                x: Math.round(10*base.scalex())
                y: 0
                width: Math.round(445*base.scalex())
                text: base.trans(14)
                font.pixelSize: Math.round(35*base.scalex())
                onFocusChanged: masked(this,base.trans(14))
            }

            MButton {
                id: addAction
                y: 0
                x: Math.round(462*base.scalex())
                text: base.trans(53)
                width : Math.round(300*base.scalex())
                onClicked: {
                    base.marginBalanceAdd(marginFunds.text)
                    refresh()
                }
            }

            MButton {
                id: removeAction
                x: Math.round(772*base.scalex())
                width : Math.round(300*base.scalex())
                text: base.trans(54)
                onClicked: {
                    base.marginBalanceRemove(marginFunds.text)
                    refresh()
                }
            }
        }

        Item {
            id: leverageExecute
            y: Math.round(545*base.scaley())
            visible: true

            MListButton {
                id: leverage
                x: Math.round(10*base.scalex())
                width: Math.round(724*base.scalex())+Math.round(347*base.scalex())-Math.round(10*base.scalex())
                z: 10
                inner.border.width: 0
                inner.radius: 0
                view.currentIndex: 1
                model: modelleverage
                inner.color: "#414141"
            }

            ExclusiveGroup { id: ttype }

            Rectangle
            {
                x: Math.round(10*base.scalex())
                y: Math.round(95*base.scaley())
                width: Math.round(520*base.scalex())
                height: Math.round(85*base.scaley())
                color: "#41be1c"
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(60*base.scalex())
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: base.trans(40)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#ffffff"
                }
                RadioButton {
                    id: buy
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(10*base.scaley())
                    checked: true
                    exclusiveGroup: ttype
                    style: RadioButtonStyle{}
                }
            }

            Rectangle
            {
                x: Math.round(535*base.scalex())
                y: Math.round(95*base.scaley())
                width: Math.round(537*base.scalex())
                height: Math.round(85*base.scaley())
                color: "#cf0302"
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(60*base.scalex())
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: base.trans(41)
                    font.pixelSize: Math.round(35*base.scaley())
                    color: "#ffffff"
                }
                RadioButton {
                    id: sell
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(10*base.scalex())
                    exclusiveGroup: ttype
                    style: RadioButtonStyle{}
                }
            }

            Rectangle
            {
                y: Math.round(190*base.scaley())
                MText {
                    id: price
                    x: Math.round(10*base.scalex())
                    width: Math.floor(257*base.scalex())
                    text: base.trans(13)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(13))
                    onTextChanged: checkMax()
                }

                MText {
                    id: amount
                    x: Math.round(277*base.scalex())
                    width: Math.floor(257*base.scalex())
                    text: base.trans(14)+" "+base.trans(116)+" "+base.getSecondCurrency()
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    //text: "100.0"
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(14)+" "+base.trans(116)+" "+base.getSecondCurrency())
                    onTextChanged: checkMax()
                }

                MText {
                    id: rateProfit
                    x:Math.round(544*base.scalex())
                    width: Math.round(257*base.scalex())
                    text: base.trans(38)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(38))
                }

                MText {
                    id: rateLoss
                    x:Math.round(813*base.scalex())
                    width: Math.round(257*base.scalex())
                    text: base.trans(39)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(39))
                }
            }

            MButton {
                id: action
                x: Math.round(10*base.scalex())
                y: Math.round(285*base.scaley())
                width: Math.round(724*base.scalex())+Math.round(347*base.scalex())-Math.round(10*base.scalex())
                text: base.trans(12)
                onClicked: {
                    if ((base.getSecondCurrency()==="PLN")&&((isNaN(amount.text))||(amount.text<100)))
                    {
                        errorDialog.text=base.trans(117)
                        errorDialog.visible=true
                    }
                    else if ((!isNaN(amount.text))&&(!isNaN(price.text)))
                    {
                        executeLeverageConfirmation.text=base.trans(21)+" "+(buy.checked?base.trans(23):base.trans(24))+" "+base.getFirstCurrency()+" "+base.trans(52)+" "+price.text+" "+base.getSecondCurrency()+"/"+base.getFirstCurrency()+" "+base.trans(22)+" "+amount.text+" "+base.getSecondCurrency()+" ?"
                        executeLeverageConfirmation.visible=true
                    }
                }
            }
        }

        Item {
            id: leverageModify
            y: Math.round(545*base.scaley())
            visible: false

            ExclusiveGroup { id: mtype }

            Rectangle
            {
                x: Math.round(10*base.scalex())
                width: Math.round(347*base.scalex())
                height: Math.round(85*base.scaley())
                color: "#414141"
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(60*base.scalex())
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: base.trans(47)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#ffffff"
                }
                RadioButton {
                    id: modify
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(10*base.scalex())
                    checked: true
                    exclusiveGroup: mtype
                    onCheckedChanged: modifyHide()
                    style: RadioButtonStyle{}
                }
            }

            Rectangle
            {
                x: Math.round(367*base.scalex())
                width: Math.round(347*base.scalex())
                height: Math.round(85*base.scaley())
                color: "#414141"
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(60*base.scalex())
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: base.trans(48)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#ffffff"
                }
                RadioButton {
                    id: close
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(10*base.scalex())
                    checked: false
                    exclusiveGroup: mtype
                    onCheckedChanged: modifyHide()
                    style: RadioButtonStyle{}
                }
            }

            Rectangle
            {
                x: Math.round(724*base.scalex())
                width: Math.round(347*base.scalex())
                height: Math.round(85*base.scaley())
                color: "#414141"
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(60*base.scalex())
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: base.trans(56)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#ffffff"
                }
                RadioButton {
                    id: cancel
                    anchors.fill: parent
                    anchors.leftMargin: Math.round(10*base.scalex())
                    checked: false
                    exclusiveGroup: mtype
                    onCheckedChanged: modifyHide()
                    style: RadioButtonStyle{}
                }
            }

            Rectangle {
                x:Math.round(10*base.scalex())
                y:Math.round(95*base.scaley())
                id: modifyPrice
                property alias text:fieldPrice.text

                Text
                {
                    height: Math.round(85*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(13)+":"
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }

                MText
                {
                    id: fieldPrice
                    y:Math.round(95*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(13)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(13))
                }
            }

            Rectangle {
                id: modifyAmount
                x: Math.round(720*base.scalex())
                y: Math.round(95*base.scaley())
                property alias text:fieldAmount.text

                Text
                {
                    height: Math.round(85*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(14)+":"
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }

                MText {
                    id: fieldAmount
                    x: Math.round(10*base.scalex())
                    y: Math.round(95*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(14)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(14))
                }
            }

            Rectangle {
                id: modifyRateProfit
                x: Math.round(362*base.scalex())
                y: Math.round(95*base.scaley())
                property alias text:fieldProfit.text

                Text
                {
                    height: Math.round(85*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(38)+":"
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }

                MText {
                    id: fieldProfit
                    y: Math.round(95*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(38)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scalex())
                    onFocusChanged: masked(this,base.trans(38))
                }
            }

            Rectangle {
                id: modifyRateLoss
                x: Math.round(720*base.scalex())
                y: Math.round(95*base.scaley())
                property alias text:fieldLoss.text

                Text
                {
                    height: Math.round(85*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(39)+":"
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }
                MText {
                    id: fieldLoss
                    y: Math.round(95*base.scaley())
                    width: Math.round(345*base.scalex())
                    text: base.trans(39)
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    font.pixelSize: Math.round(35*base.scaley())
                    onFocusChanged: masked(this,base.trans(39))
                }
            }

            MButton {
                id: actionModify
                x: Math.round(10*base.scalex())
                y: Math.round(285*base.scaley())
                width: Math.round(524*base.scalex())
                text: base.trans(12)
                onClicked: {
                    if ((close.checked)&&(modifyAmount.text>=0))
                    {
                        modifyLeverageConfirmation.text=base.trans(55)+" "+modifyAmount.text+" "+base.getSecondCurrency()+" ?"
                        modifyLeverageConfirmation.visible=true
                    } else if ((cancel.checked)&&(modifyAmount.text>=0))
                    {
                        modifyLeverageConfirmation.text=base.trans(57)+" "+modifyAmount.text+" "+base.getSecondCurrency()+" ?"
                        modifyLeverageConfirmation.visible=true
                    }else if (modify.checked)
                    {
                        modifyLeverageConfirmation.text=base.trans(21)+" "+base.trans(87)+" "+openPositions.lastTid+" ?"
                        modifyLeverageConfirmation.visible=true
                    }

                }
            }

            MButton {
                id: newPosition
                x: Math.round(544*base.scalex())
                y: Math.round(285*base.scaley())
                width: Math.round(524*base.scalex())
                text: base.trans(49)
                onClicked: {
                    leverageModify.visible=false
                    leverageExecute.visible=true
                    openPositions.currentRow=-1
                    openPositions.selection.clear()
                }
            }
        }

        TableView {
            property string lastTid
            property string fiatOpened
            property string fiatTotal

            id: openPositions
            headerVisible: false
            TableViewColumn {role: "opened"; width: base.getWidth()-2 }
            model: modelpositions
            x: 0
            y: Math.round(915*base.scaley())
            z: -1
            width: base.getWidth()
            height: Math.round(750*base.scaley())
            style: TableViewStyle {
                frame: Rectangle {
                    border{
                        width: 0
                    }
                }
            }
            contentHeader: Rectangle {
                width: base.getWidth()
                height: Math.round(75*base.scaley())
                color: "#f5f4f2"
                Text {
                    x: Math.round(30*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: base.trans(91)+"/"+base.trans(8)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }
                Text {
                    x: Math.round(320*base.scalex())
                    width: Math.round(300*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: base.trans(97)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }
                Text {
                    x: Math.round(650*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: base.trans(38)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }
                Text {
                    x: Math.round(850*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: base.trans(39)
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                }
            }
            rowDelegate: Rectangle {
                width: base.getWidth()
                height: (styleData.row===openPositions.currentRow)?Math.round(250*base.scaley()):Math.round(75*base.scaley())
                color: (styleData.row%2==0)? "#f5f4f2" : "#ffffff"
            }
            itemDelegate: Item {
                height: (styleData.row===openPositions.currentRow)?Math.round(250*base.scaley()):Math.round(75*base.scaley())
                width: base.getWidth()
                Rectangle
                {
                    y: Math.round(75*base.scaley())
                    visible: (styleData.row===openPositions.currentRow)
                    color: "#41bb1a"
                    width: base.getWidth()
                    height: Math.round(175*base.scaley())
                    Text {
                        y: Math.round(15*base.scaley())
                        width: Math.round(300*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['time']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(300*base.scalex())
                        y: Math.round(15*base.scaley())
                        width: Math.round(200*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: base.trans(43)+": <b>"+styleData.value['price']+"</b>"
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(525*base.scalex())
                        y: Math.round(15*base.scaley())
                        width: Math.round(200*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['rateClose']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(750*base.scalex())
                        y: Math.round(15*base.scaley())
                        width: Math.round(300*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['profit']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(0*base.scalex())
                        y: Math.round(80*base.scaley())
                        width: Math.round(300*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['leverage']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(300*base.scalex())
                        y: Math.round(80*base.scaley())
                        width: Math.round(300*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['security']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                    Text {
                        x: Math.round(700*base.scalex())
                        y: Math.round(80*base.scaley())
                        width: Math.round(300*base.scalex())
                        height: Math.round(75*base.scaley())
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: styleData.value['fees']+""
                        font.pixelSize: Math.round(25*base.scalex())
                        color: "#ffffff"
                    }
                }
                Text {
                    x: Math.round(30*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value['opened']+"/"+styleData.value['total']
                    font.pixelSize: Math.round(35*base.scalex())
                    color: (styleData.selected)? "#888888" : "#000000"
                }
                Text {
                    x: Math.round(320*base.scalex())
                    width: Math.round(300*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value['type']+""
                    font.pixelSize: Math.round(35*base.scalex())
                    color: (styleData.selected)? "#888888" : "#000000"
                }
                Text {
                    x: Math.round(650*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value['rateProfit']+""
                    font.pixelSize: Math.round(35*base.scalex())
                    color: (styleData.selected)? "#888888" : "#000000"
                }
                Text {
                    x: Math.round(850*base.scalex())
                    width: Math.round(200*base.scalex())
                    height: Math.round(75*base.scaley())
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value['rateLoss']+""
                    font.pixelSize: Math.round(35*base.scalex())
                    color: (styleData.selected)? "#888888" : "#000000"
                }
            } // Item
            selectionMode: SelectionMode.SingleSelection
            selection{
                onSelectionChanged: {
                    if ((currentRow<rowCount)&&(currentRow>=0))
                    {
                        leverageExecute.visible=false
                        leverageModify.visible=true
                        currentpos=modelpositions.get(currentRow)
                        openPositions.lastTid=currentpos["tid"]
                        openPositions.fiatOpened=currentpos["opened"]
                        openPositions.fiatTotal=currentpos["total"]
                        updateValues()
                    }
                }
            }
        }
    }
}
