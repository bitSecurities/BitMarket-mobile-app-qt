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

Frame {
    onRefresh: {
        if (base.isLogged()) {
            details.text=base.getDeposit()
            if ((currencies.name==="PLN")||(currencies.name==="EUR")) details.horizontalAlignment=Text.AlignLeft
            else details.horizontalAlignment=Text.AlignHCenter
            generateQR()
        }
    }

    onUpdate: {
        refresh()
    }

    onClear:
    {
        loginField.visible=!base.isLogged()
        depositFrame.visible=base.isLogged()
        currencies.view.currentIndex=0;
        currencies.popup.ref()
        refresh()
    }

    function generateQR()
    {
        var add

        add="-"
        if (currencies.name==="BTC") add="bitcoin:"
        else if (currencies.name==="LTC") add="litecoin:"
        else if (currencies.name==="BCC") add="bitcoincash:"
        else if (currencies.name==="BTG") add="bitcoingold:"
        else if (currencies.name==="DOGE") add="doge:"
        else if (currencies.name==="LSK") add="lisk:"
        else if (currencies.name==="XRP") add=""
        if ((add!=="-")&&(base.getDeposit().indexOf("Error") !== 0)) {
            qrcode.visible=true
            qrcode.value=add+base.getDepositAccount()
        }
        else qrcode.visible=false;
    }

    function loginSuccess()
    {
        loginField.visible=false
        depositFrame.visible=true
        base.refreshCurrencies()
        currencies.popup.ref()
        base.deposit(currencies.name)
        refresh()
    }

    function loginFailed()
    {
        errorDialog.title=base.trans(18)
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
        refresh()
    }

    function apiSuccess()
    {
        refresh()
    }

    function apiFailed()
    {
        refresh()
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
        id : depositFrame
        visible: base.isLogged()

        MListButtonN {
            x: Math.round(420*base.scalex())
            y: Math.round(250*base.scaley())
            z: 10
            id: currencies
            model: modeldepositcurrencies
            view.currentIndex: (base.isLogged())?2:0
            onNameChanged: {
                if (base.isLogged()) {
                    base.deposit(name)
                    refresh()
                }
            }
        }

        Text
        {
            id: details
            width: Math.round(base.getWidth()-20*base.scalex())
            x: Math.round(10*base.scalex())
            y: Math.round(500*base.scaley())
            horizontalAlignment: Text.AlignLeft
            text: base.getDeposit()
            wrapMode: Text.WordWrap
            font.pixelSize: Math.round(35*base.scalex())
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    if ((currencies.name==="PLN")||(currencies.name==="EUR")) messages.displayMessage(base.trans(126));
                    else messages.displayMessage(base.trans(127));
                    base.copyAccount();
                }
            }
        }

        QRCode {
            id: qrcode
            x: Math.round(190*base.scalex())
            y: Math.round(870*base.scaley())
            z: -1
            width : Math.round(700*base.scalex())
            height : Math.round(750*base.scaley())
            value : base.getDeposit()
        }
    }
}
