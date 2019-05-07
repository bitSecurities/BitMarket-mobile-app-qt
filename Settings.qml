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
    property bool loaded:false

    function loginSuccess()
    {
        encryptPassword.visible=base.isLogged()
        refresh()
    }

    function loginFailed()
    {
        errorDialog.title=base.trans(18)
        errorDialog.text=base.getLastError()
        errorDialog.visible=true
        refresh()
    }

    ListModel {
        id: languagesmodel
        ListElement {
            name: "English"
            value: "English"
        }
        ListElement {
            name: "Polski"
            value: "Polish"
        }
    }

    MText
    {
        id: langt

        x: Math.round(50*base.scalex())
        y: Math.round(200*base.scaley())
        width: Math.round(base.getWidth()/2-60*base.scalex())
        height: Math.round(125*base.scaley())
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: base.trans(103)+":"
        readOnly: true
        font.pixelSize: Math.round(35*base.scalex())
    }

    MListButtonN {
        x: Math.round(base.getWidth()/2+10*base.scalex())
        y: Math.round(200*base.scaley())
        height: Math.round(125*base.scaley())
        width: Math.round(base.getWidth()/2-60*base.scalex())
        id: languages
        model: languagesmodel
        view.currentIndex: base.currentlang()
        onNameChanged: {
            if (loaded) {
                base.changelang(name)
                base.savekeys()
                loginfield.displaytext=base.trans(9)
                langt.text=base.trans(103)+":"
                lang2t.text=base.trans(104)+":"
                menu0.text=base.menu(menu0.k)
                menu1.text=base.menu(menu1.k)
                menu2.text=base.menu(menu2.k)
                menu3.text=base.menu(menu3.k)
                menu4.text=base.menu(menu4.k)
                menu5.text=base.menu(menu5.k)
                menu6.text=base.menu(menu6.k)
                menu7.text=base.menu(menu7.k)
                menu8.text=base.menu(menu8.k)
                menu9.text=base.menu(menu9.k)
                menu10.text=base.menu(menu10.k)
                menu11.text=base.menu(menu11.k)
                order.text=base.trans(85+order.view.currentIndex)
                moreInfo.text=base.trans(122)
                errorDialog.moreinfo=base.trans(121)
                errorDialog.title=base.trans(114)
                cryptButton.text=base.trans(60)
                password.text=base.trans(58)
                password.mtext=base.trans(58)
                confirmPassword.text=base.trans(59)
                confirmPassword.mtext=base.trans(59)
            }
        }
    }

    MText
    {
        id: lang2t

        x: Math.round(50*base.scalex())
        y: Math.round(350*base.scaley())
        width: Math.round(base.getWidth()/2-60*base.scalex())
        height: Math.round(125*base.scaley())
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: base.trans(104)+":"
        font.pixelSize: Math.round(35*base.scalex())
        readOnly: true
    }

    MListButtonN {
        id: order
        x: Math.round(base.getWidth()/2+10*base.scalex())
        y: Math.round(350*base.scaley())
        height: Math.round(125*base.scaley())
        width: Math.round(base.getWidth()/2-60*base.scalex())
        z: -1
        model: modelorder
        view.currentIndex: base.getReverse()
        onNameChanged: {
            base.setReverse(view.currentIndex)
        }
    }

    Item
    {
        y: Math.round(600*base.scaley())

        id: encryptPassword
        visible: base.isLogged()
        MText {
            id: password
            x: Math.round(340*base.scalex())
            y: Math.round(0*base.scaley())
            width: Math.round(400*base.scalex())
            height: Math.round(125*base.scaley())
            text: base.trans(58)
            mtext: base.trans(58)
            font.pixelSize: Math.round(35*base.scalex())
            onFocusChanged: maskedPassword(this,mtext)
        }

        MText {
            id: confirmPassword
            x: Math.round(340*base.scalex())
            y: Math.round(150*base.scaley())
            width: Math.round(400*base.scalex())
            height: Math.round(125*base.scaley())
            text: base.trans(59)
            mtext: base.trans(59)
            font.pixelSize: Math.round(35*base.scalex())
            onFocusChanged: maskedPassword(this,mtext)
        }

        MButton {
            id: cryptButton
            x:Math.round(340*base.scalex())
            y:Math.round(300*base.scaley())
            width: Math.round(400*base.scalex())
            height: Math.round(125*base.scaley())
            text: base.trans(60)
            onClicked: {
                if (password.text!=confirmPassword.text) {
                    errorDialog.text=base.trans(61)
                    errorDialog.visible=true
                }else {
                    base.encryptKeys(password.text)
                    errorDialog.text=base.trans(62)
                    errorDialog.visible=true
                }
            }
        }
    }

    Component.onCompleted:
    {
        loaded=true;
    }
}
