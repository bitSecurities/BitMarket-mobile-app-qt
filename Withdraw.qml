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

    onAccepted: {
        var note
        console.log("Accepted")
        if (withdrawNote.text!==base.trans(79)) note=withdrawNote.text;
        else note="";
        if (!base.withdraw(withdrawAmount.text,currency.name,withdrawAddress.text,withdrawSwift.text,note,isfast.checked))
        {
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
    }

    function makeLogin() {
        if (base.login(pass)) {
            loginField.visible=false
            withdrawFrame.visible=true
            refresh()
        }else {
            refresh()
            errorDialog.title=base.trans(18)
            errorDialog.text=base.getLastError()
            errorDialog.visible=true
        }
    }

    function parseKrypto(tag)
    {
        var i,j

        i=tag.search("bitcoin:")
        if ((i>=0)&&(currency.name==="BTC")) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            withdrawAddress.text=tag.substring(i+8,j)
        }
        i=tag.search("litecoin:")
        if ((i>=0)&&(currency.name==="LTC")) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            withdrawAddress.text=tag.substring(i+8,j)
        }
    }

    onRefresh: parseKrypto(base.getscannedtxt())

    onUpdate: refresh()

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
        id : withdrawFrame
        visible: base.isLogged()

        MListButton {
            id: currency
            x: Math.round(420*base.scalex())
            y: Math.round(400*base.scaley())
            model: modelbalance
            view.currentIndex: 2
            inner.border.width: 0
            inner.radius: 0
            onNameChanged: {
                if (name==="EUR") withdrawSwift.visible=true
                else withdrawSwift.visible=false
                if (name==="PLN") {
                    withdrawNote.visible=true
                    isfast.visible=true
                }
                else {
                    withdrawNote.visible=false
                    isfast.visible=false
                }
                if ((name==="EUR")||(name==="PLN"))
                {
                    withdrawAddress.text=base.trans(106)
                    withdrawAddress.mtext=base.trans(106)
                    actionExternal.visible=false
                }else
                {
                    withdrawAddress.text=base.trans(67)
                    withdrawAddress.mtext=base.trans(67)
                    actionExternal.visible=true
                }
            }
        }

        MText {
            id: withdrawAmount
            x:Math.round(20*base.scalex())
            y:Math.round(700*base.scaley())
            z:-1
            width: Math.round(400*base.scalex())
            text: base.trans(14)
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,base.trans(14))
        }

        MText {
            property string mtext

            id: withdrawAddress
            x:Math.round(20*base.scalex())
            y:Math.round(800*base.scaley())
            z:-1
            width: Math.round(1000*base.scalex())
            text: base.trans(67)
            mtext: base.trans(67)
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,mtext)
        }

        MText {
            id: withdrawSwift
            x:Math.round(20*base.scalex())
            y:Math.round(900*base.scaley())
            z:-1
            width: Math.round(400*base.scalex())
            text: base.trans(78)
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,base.trans(78))
        }

        MText {
            id: withdrawNote
            x:Math.round(20*base.scalex())
            y:Math.round(900*base.scaley())
            z:-1
            width: Math.round(1000*base.scalex())
            text: base.trans(79)
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,base.trans(79))
        }

        MText {
            id: withdrawPass
            visible: base.isEncrypted()
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
            z:-1
            width: Math.round(500*base.scalex())
            text: base.trans(58)
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: maskedPassword(this,base.trans(58))
        }

        CheckBox
        {
            id: isfast
            x:Math.round(20*base.scalex())
            y:Math.round(1000*base.scaley())
            z:-1
            width: Math.round(1000*base.scalex())
            checked: false
            style: CheckBoxStyle {
                label: Text {
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                    horizontalAlignment: Text.AlignLeft
                    text: base.trans(80)
                }
            }
        }

        MButton {
            id: actionExternal
            x: Math.round(20*base.scalex())
            y: Math.round(1200*base.scaley())
            width: Math.round(300*base.scalex())
            text: base.trans(90)
            onClicked: base.scanner()
        }

        MButton{
            id: action
            x: Math.round(420*base.scalex())
            y: Math.round(1200*base.scaley())
            text: base.trans(65)
            onClicked: {
                if ((base.isEncrypted())&&(!base.checkWithdrawalPass(withdrawPass.text)))
                {
                    errorDialog.text=base.trans(63)
                    errorDialog.visible=true
                }else
                {
                    if (!base.testWithdraw(withdrawAmount.text,currency.name,withdrawAddress.text,withdrawSwift.text,"",isfast.checked))
                    {
                        errorDialog.text=base.getLastError()
                        errorDialog.visible=true
                    }else
                    {
                        if (currency.name==="PLN") withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" "+currency.name+" "+base.trans(77)+" "+withdrawAddress.text+" "+base.trans(81)+" "+base.getLastFee()+" "+currency.name+"?";
                        else if (currency.name==="EUR") withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" "+currency.name+" "+base.trans(77)+" "+withdrawAddress.text+" "+base.trans(82)+" "+withdrawSwift.text+" "+base.trans(81)+" "+base.getLastFee()+" "+currency.name+"?";
                        else if ((currency.name==="BTC")||(currency.name==="LTC")) withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" "+currency.name+" "+base.trans(83)+" "+withdrawAddress.text+"?";
                        withdrawalConfirmation.visible=true
                    }
                }
            }
        }
    }
}
