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

    onUpdate: {
        refresh()
    }

    onClear:
    {
        alertActive.checked=base.getActive()
        bidabove.text=base.getBidAbove()
        bidbelow.text=base.getBidBelow()
        askabove.text=base.getAskAbove()
        askbelow.text=base.getAskBelow()
    }

    Item
    {
        id : alertsFrame

        CheckBox {
            id: alertActive
            x:Math.round(20*base.scalex())
            y:Math.round(200*base.scaley())
            checked: base.getActive()
            style: CheckBoxStyle {
                label: Text {
                    font.pixelSize: Math.round(35*base.scalex())
                    color: "#000000"
                    horizontalAlignment: Text.AlignLeft
                    text: base.trans(71)
                }
            }
        }

        MText {
            id: bidabove
            x:Math.round(20*base.scalex())
            y:Math.round(400*base.scaley())
            width: Math.round(1000*base.scalex())
            text: base.getBidAbove()
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            onFocusChanged: masked(this,base.trans(69))
        }

        MText {
            id: askbelow
            x:Math.round(20*base.scalex())
            y:Math.round(600*base.scaley())
            width: Math.round(1000*base.scalex())
            text: base.getAskBelow()
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            onFocusChanged: masked(this,base.trans(70))
        }

        MText {
            id: bidbelow
            x:Math.round(20*base.scalex())
            y:Math.round(800*base.scaley())
            width: Math.round(1000*base.scalex())
            text: base.getBidBelow()
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            onFocusChanged: masked(this,base.trans(72))
        }

        MText {
            id: askabove
            x:Math.round(20*base.scalex())
            y:Math.round(1000*base.scaley())
            width: Math.round(1000*base.scalex())
            text: base.getAskAbove()
            horizontalAlignment: Text.AlignLeft
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            onFocusChanged: masked(this,base.trans(73))
        }

        MButton {
            id: saveAlerts
            x:Math.round(20*base.scalex())
            y:Math.round(1200*base.scaley())
            text: base.trans(51)
            onClicked:
            {
                base.saveAlerts(alertActive.checked,bidabove.text,bidbelow.text,askabove.text,askbelow.text)
                messages.displayMessage(base.trans(74));
            }
        }
    }
}
