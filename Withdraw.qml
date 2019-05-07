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
    property string apiFunction

    onAccepted: {
        var note,method
        console.log("Accepted")
        if (currency.name===base.trans(129))
        {
            apiFunction="withdraw"
            base.transfer(withdrawAddress.text,currencyInternal.name,withdrawAmount.text)
        }else
        {
            var curr;

            if (withdrawNote.text!==base.trans(79)) note=withdrawNote.text
            else note=""
            curr=currency.name
            if (currency.name===base.trans(138)) {
                method=2 //ATM withdrawal
                curr="PLN"
            }
            else if (isfast.checked) method=1; //fast withdrawal
            else method=0; //normal withdrawal
            apiFunction="withdraw2"
            if (curr==="XRP") base.withdraw(withdrawAmount.text,curr,withdrawAddress.text,withdrawTag.text,note,method)
            else base.withdraw(withdrawAmount.text,curr,withdrawAddress.text,withdrawSwift.text,note,method)
        }
    }

    function loginSuccess()
    {
        loginField.visible=false
        withdrawFrame.visible=true
        base.refreshCurrencies()
        currency.popup.ref()
        withdrawBalance.text=makeBalance()
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
        if ((apiFunction=="withdraw")||(apiFunction=="withdraw2"))
        {
            if (apiFunction=="withdraw") successDialog.text=withdrawAmount.text+" "+currencyInternal.name+" "+base.trans(134)+" "+withdrawAddress.text
            else if (apiFunction=="withdraw2") successDialog.text=base.trans(142)
            successDialog.visible=true
            apiFunction="funds"
            base.getFunds()
        }
        else if (apiFunction=="funds") withdrawBalance.text=makeBalance()
    }

    function apiFailed()
    {
        errorDialog.title=base.trans(18)
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
        refresh()
    }

    function parseKrypto(tag)
    {
        var i,j

        i=tag.search("bitcoin:")
        if (i>=0) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            currency.name="BTC"
            currency.view.currentIndex=2
            currency.popup.ref()
            withdrawAddress.text=tag.substring(i+8,j)
        }
        i=tag.search("litecoin:")
        if (i>=0) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            currency.name="LTC"
            currency.view.currentIndex=3
            currency.popup.ref()
            withdrawAddress.text=tag.substring(i+9,j)
        }
        i=tag.search("bitcoincash:")
        if (i>=0) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            currency.name="BCC"
            currency.view.currentIndex=4
            currency.popup.ref()
            withdrawAddress.text=tag.substring(i+12,j)
        }
        i=tag.search("bitcoingold:")
        if (i>=0) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            currency.name="BTG"
            currency.view.currentIndex=5
            currency.popup.ref()
            withdrawAddress.text=tag.substring(i+12,j)
        }
    }

    function checkmax()
    {
        if (!base.isLogged) return
        if ((parseFloat(withdrawAmount.text)>0)&&(modelbalance.get(base.stringtocur(currency.name))!==null)&&(parseFloat(withdrawAmount.text)>modelbalance.get(base.stringtocur(currency.name)).value)) withdrawAmount.text=modelbalance.get(base.stringtocur(currency.name)).value
    }

    function makeBalance()
    {
        if (currency.name===base.trans(138)) return(modelbalance.get(base.stringtocur("PLN")).value+" PLN")
        else if (currency.name===base.trans(129)) return(modelbalance.get(base.stringtocur(currencyInternal.name)).value+" "+currencyInternal.name)
        else return(modelbalance.get(base.stringtocur(currency.name)).value+" "+currency.name)
    }

    function openScanner()
    {
        idx=currency.view.currentIndex
        addr=withdrawAddress.text
        handlerLoader("qrc:///WithdrawCam.qml")
    }

    onRefresh: parseKrypto(base.getscannedtxt())

    onUpdate: refresh()

    onClear:
    {
        loginField.visible=!base.isLogged()
        withdrawFrame.visible=base.isLogged()
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
        id : withdrawFrame
        visible: base.isLogged()
        y: Math.round(70*base.scaley())

        MListButton {
            id: currency
            x: Math.round(300*base.scalex())
            y: Math.round(100*base.scaley())
            z: 11
            width: 500*base.scalex()
            popup.itemWidth: 500*base.scalex()
            popup.z: 11
            model: modelwithdrawcurrencies
            view.currentIndex: {
                if (idx==-1)
                {
                    if (base.isLogged())
                    {
                        if (ex.name==="Bitmarket") 2
                        else 1
                    }else 0
                }
                else idx
            }
            inner.border.width: 0
            inner.radius: 0
            onNameChanged: {
                withdrawAddress.visible=true
                withdrawBalance.visible=true
                currencyInternal.visible=true
                action.visible=true
                withdrawAmount.visible=true
                if (name==="EUR") withdrawSwift.visible=true
                else withdrawSwift.visible=false
                withdrawBalance.visible=true
                currencyInternal.visible=false
                actionExternal.visible=false
                action.text=base.trans(65)
                if (ex.name==="Bitmaszyna") chooseAccount.visible=true
                if (name==="PLN") {
                    if (ex.name==="Bitmarket") withdrawNote.visible=true
                    isfast.visible=true
                }
                else {
                    withdrawNote.visible=false
                    isfast.visible=false
                }
                if (name==="XRP") withdrawTag.visible=true
                else withdrawTag.visible=false
                withdrawBalance.text=makeBalance()
                if ((name==="EUR")||(name==="PLN"))
                {
                    //if (addr!="") withdrawAddress.text=addr
                    //else
                    withdrawAddress.text=base.trans(106)
                    withdrawAddress.mtext=base.trans(106)
                }else if (name===base.trans(129))
                {
                    withdrawAddress.text=base.trans(130)
                    withdrawAddress.mtext=base.trans(130)
                    withdrawBalance.visible=true
                    currencyInternal.visible=true
                    action.text=base.trans(131)
                    if (ex.name==="Bitmaszyna")
                    {
                        withdrawAddress.visible=false;
                        withdrawBalance.visible=false
                        currencyInternal.visible=false;
                        action.visible=false;
                        withdrawAmount.visible=false
                        chooseAccount.visible=false
                    }
                }else if (name===base.trans(138))
                {
                    withdrawAddress.visible=true
                    withdrawAddress.text=base.trans(139)
                    withdrawAddress.mtext=base.trans(139)
                    withdrawBalance.visible=true
                    currencyInternal.visible=false
                    withdrawNote.visible=true
                    action.text=base.trans(65)
                    if (ex.name==="Bitmaszyna")
                    {
                        withdrawAddress.visible=false
                        withdrawBalance.visible=false
                        currencyInternal.visible=false
                        action.visible=false
                        withdrawNote.visible=false
                        withdrawAmount.visible=false
                        chooseAccount.visible=false
                    }
                }
                else
                {
                    //if (addr!="") withdrawAddress.text=addr
                    //else
                    withdrawAddress.text=base.trans(67)
                    withdrawAddress.mtext=base.trans(67)
                    if (ex.name==="Bitmarket") actionExternal.visible=true
                }
                base.setWithdrawalCurrencyName(name)
                chooseAccount.popup.view.currentIndex=0
                chooseAccount.popup.ref()
                if (ex.name==="Bitmaszyna") withdrawAddress.text=base.getWithdrawalAccount(chooseAccount.view.currentIndex)
                withdrawAmount.text=base.trans(14)
            }
        }

        MListButton {
            id: chooseAccount
            x: Math.round(120*base.scalex())
            y: Math.round(200*base.scaley())
            width: Math.round(860*base.scalex())
            popup.itemWidth: Math.round(860*base.scalex())
            model: modelwithdrawalaccounts
            inner.border.width: 0
            inner.radius: 0
            visible: (ex.name==="Bitmaszyna")?true:false
            onNameChanged:
            {
                withdrawAddress.text=base.getWithdrawalAccount(chooseAccount.view.currentIndex)
            }
        }

        MListButton {
            id: currencyInternal
            x: Math.round(420*base.scalex())
            y: Math.round(200*base.scaley())
            z: 10
            model: modelbalance
            visible: false
            inner.border.width: 0
            inner.radius: 0
            onNameChanged:
            {
                withdrawBalance.text=makeBalance();
            }
        }

        MText {
            id: withdrawAmount
            x:Math.round(20*base.scalex())
            y:Math.round(400*base.scaley())
            width: Math.round(400*base.scalex())
            text: base.trans(14)
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: {
                masked(this,base.trans(14))
            }
            onDisplayTextChanged:
            {
                checkmax()
            }
        }

        Text
        {
            id: withdrawBalance

            x:Math.round(500*base.scalex())
            y:Math.round(420*base.scaley())
            width: Math.round(400*base.scalex())
            height: Math.round(85*base.scaley())
            horizontalAlignment: Text.AlignRight
            font.pixelSize: Math.round(35*base.scalex())
            text: makeBalance()
        }

        MText {
            property string mtext

            id: withdrawAddress
            x:Math.round(20*base.scalex())
            y:Math.round(500*base.scaley())
            width: Math.round(1000*base.scalex())
            text: {
                if (addr!="") addr
                else base.trans(67)
            }
            mtext: base.trans(67)
            readOnly: (ex.name==="Bitmaszyna")
            bcolor: (ex.name==="Bitmaszyna")?"#bbbbbb":"#ffffff"
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,mtext)
        }

        MText {
            property string mtext

            id: withdrawTag
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
            width: Math.round(1000*base.scalex())
            text: {
                if (addr!="") addr
                else base.trans(143)
            }
            mtext: base.trans(143)
            readOnly: (ex.name==="Bitmaszyna")
            bcolor: (ex.name==="Bitmaszyna")?"#bbbbbb":"#ffffff"
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,mtext)
        }

        MText {
            id: withdrawSwift
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
            width: Math.round(400*base.scalex())
            text: base.trans(78)
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Math.floor(35*base.scaley())
            onFocusChanged: masked(this,base.trans(78))
        }

        MText {
            id: withdrawNote
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
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
            y:Math.round(300*base.scaley())
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
            y:Math.round(700*base.scaley())
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
            y: Math.round(900*base.scaley())
            width: Math.round(300*base.scalex())
            text: base.trans(90)
            onClicked: {
                if (base.getVersion()===1) base.scanner() //android version
                else //desktop and iphone
                {
                    openScanner()
                }
            }
        }

        MButton{
            id: action
            x: Math.round(420*base.scalex())
            y: Math.round(800*base.scaley())
            text: base.trans(65)
            onClicked: {
                if (currency.name===base.trans(138))
                {
                    if ((withdrawAddress.text===base.trans(139))||(withdrawAmount.text<=0)) return
                    if (withdrawAmount.text%50!=0)
                    {
                        errorDialog.text=base.trans(141)
                        errorDialog.visible=true
                    }
                    else if ((base.isEncrypted())&&(!base.checkWithdrawalPass(withdrawPass.text)))
                    {
                        errorDialog.text=base.trans(63)
                        errorDialog.visible=true
                    }else
                    {
                        withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" PLN "+base.trans(140)+" "+withdrawAddress.text
                        withdrawalConfirmation.visible=true
                    }
                }
                else if (currency.name===base.trans(129))
                {
                    if ((base.isEncrypted())&&(!base.checkWithdrawalPass(withdrawPass.text)))
                    {
                        errorDialog.text=base.trans(63)
                        errorDialog.visible=true
                    }else
                    {
                        withdrawalConfirmation.text=base.trans(136)+" "+withdrawAmount.text+" "+currencyInternal.name+" "+base.trans(132)+" "+withdrawAddress.text
                        withdrawalConfirmation.visible=true
                    }
                }else
                {
                    if ((withdrawAddress.text===base.trans(67))||(withdrawAddress.text===base.trans(106))||(withdrawAmount.text<=0)) return
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
                            else withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" "+currency.name+" "+base.trans(83)+" "+withdrawAddress.text+"?";
                            withdrawalConfirmation.visible=true
                        }
                    }
                }
            }
        }
    }
}
