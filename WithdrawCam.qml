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
import QtMultimedia 5.4
import QZXing 2.3

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
            base.refreshCurrencies()
            currency.popup.ref()
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
            closeScanner()
        }
        i=tag.search("litecoin:")
        if ((i>=0)&&(currency.name==="LTC")) {
            j=tag.search("\\?")
            if (j<0) j=tag.length
            withdrawAddress.text=tag.substring(i+8,j)
            closeScanner()
        }
    }

    function checkmax()
    {
        if (!base.isLogged) return
        if ((parseFloat(withdrawAmount.text)>0)&&(modelbalance.get(base.stringtocur(currency.name))!==null)&&(parseFloat(withdrawAmount.text)>modelbalance.get(base.stringtocur(currency.name)).value)) withdrawAmount.text=modelbalance.get(base.stringtocur(currency.name)).value
    }

    function closeScanner()
    {
        idx=currency.view.currentIndex
        addr=withdrawAddress.text
        handlerLoader("qrc:///Withdraw.qml")
    }

    function makeBalance()
    {
        return(modelbalance.get(base.stringtocur(currency.name)).value+" "+currency.name)
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
            x: Math.round(420*base.scalex())
            y: Math.round(100*base.scaley())
            z: 11
            model: modeldepositcurrencies
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
                if (name==="EUR") withdrawSwift.visible=true
                else withdrawSwift.visible=false
                if (name==="PLN") {
                    if (ex.name==="Bitmarket") withdrawNote.visible=true
                    isfast.visible=true
                }
                else {
                    withdrawNote.visible=false
                    isfast.visible=false
                }
                if ((name==="EUR")||(name==="PLN"))
                {
                    if (addr!="") withdrawAddress.text=addr
                    else withdrawAddress.text=base.trans(106)
                    withdrawAddress.mtext=base.trans(106)
                    actionExternal.visible=false
                }else
                {
                    if (addr!="") withdrawAddress.text=addr
                    else withdrawAddress.text=base.trans(67)
                    withdrawAddress.mtext=base.trans(67)
                    if (ex.name==="Bitmarket") actionExternal.visible=true
                }
                base.setWithdrawalCurrencyName(name)
                chooseaccount.popup.view.currentIndex=0
                chooseaccount.popup.ref()
                if (ex.name==="Bitmaszyna") withdrawAddress.text=base.getWithdrawalAccount(chooseaccount.view.currentIndex)
                withdrawAmount.text=base.trans(14)
                withdrawAddress.text=base.trans(67);
            }
        }

        MListButton {
            id: chooseaccount
            x: Math.round(120*base.scalex())
            y: Math.round(200*base.scaley())
            width: Math.round(860*base.scalex())
            popup.itemWidth: Math.round(860*base.scalex())
            model: modelwithdrawalaccounts
            inner.border.width: 0
            inner.radius: 0
            visible: (ex.name==="Bitmaszyna")?true:false
            //            visible: false
            onNameChanged:
            {
                withdrawAddress.text=base.getWithdrawalAccount(chooseaccount.view.currentIndex)
            }
        }

        MText {
            id: withdrawAmount
            x:Math.round(20*base.scalex())
            y:Math.round(400*base.scaley())
            z:-1
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
            z:-1
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
            id: withdrawSwift
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
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
            y:Math.round(600*base.scaley())
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
            y:Math.round(300*base.scaley())
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
            y:Math.round(700*base.scaley())
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
            y: Math.round(900*base.scaley())
            width: Math.round(300*base.scalex())
            text: base.trans(125)
            onClicked: {
                closeScanner()
            }
        }

        MButton {
            id: flash
            x: Math.round(750*base.scalex())
            y: Math.round(900*base.scaley())
            width: Math.round(300*base.scalex())
            text: base.trans(88)
            onClicked: {
                if (camera.flash.mode===Camera.FlashOff) camera.flash.mode=Camera.FlashVideoLight
                else camera.flash.mode=Camera.FlashOff
            }
        }

        MButton{
            id: focus
            x: Math.round(420*base.scalex())
            y: Math.round(900*base.scaley())
            text: base.trans(89)
            onClicked:{
                camera.unlock()
                camera.searchAndLock();
            }
        }

        MButton{
            id: action
            x: Math.round(420*base.scalex())
            y: Math.round(800*base.scaley())
            text: base.trans(65)
            onClicked: {
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
                        else if ((currency.name==="BTC")||(currency.name==="LTC")) withdrawalConfirmation.text=base.trans(76)+" "+withdrawAmount.text+" "+currency.name+" "+base.trans(83)+" "+withdrawAddress.text+"?";
                        withdrawalConfirmation.visible=true
                    }
                }
            }
        }

        Camera {
            id: camera
            captureMode: Camera.CaptureStillImage
            onCameraStatusChanged: {
                base.changeLocale()
                withdrawBalance.text=makeBalance()
                base.logError(camera.cameraStatus)
            }
            focus {
                focusMode: Camera.FocusContinuous
                focusPointMode: Camera.FocusPointCenter
            }
            imageCapture {
                onImageSaved: {
                    base.logError("Path: "+path)
                    decoder.decodeImageQML(path);
                    base.remove(path);
                }
            }
            flash.mode: Camera.FlashOff
        }

        Timer {
            id: timer
            interval: 5000
            running: true
            repeat: true
            onTriggered: {
                base.changeLocale();
                camera.unlock()
                camera.searchAndLock();
                camera.imageCapture.capture()
            }
        }

        VideoOutput {
            x: Math.round(20*base.scalex())
            y: Math.round(1000*base.scaley())
            width: Math.round(base.getWidth()-40*base.scalex())
            height: Math.round(800*base.scaley())
            source: camera
            focus : visible
            autoOrientation: true
        }

        QZXing{
            property int i
            property int j

            id: decoder
            enabledDecoders: QZXing.DecoderFormat_QR_CODE
            onDecodingStarted: {
                base.logError("decoding:")
            }
            onTagFound: {
                base.logError("tag: "+tag)
                parseKrypto(tag)
            }
            onDecodingFinished: {
                base.logError("Decoding finished " + (succeeded==true ? "successfully" :    "unsuccessfully") )
            }
        }
    }
}
