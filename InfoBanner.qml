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

import QtQuick 2.2

Loader {
    id: messages

    function displayMessage(message) {
        messages.source = "";
        messages.source = Qt.resolvedUrl("InfoBannerComponent.qml");
        messages.item.message = message;
        messages.state = "show"
        timer.start()
    }

    width: parent.width*0.8
    x: parent.width*0.1
    y: Math.round(800*base.scaley())
    z: 15
    opacity: 1
    state: "hidden"

    Timer {
            id: timer

            interval: 1000
            repeat: false
            onTriggered: {
                messages.state = "hidden"
            }
        }

    states: [
        State {
            name: "show"
            PropertyChanges { target: messages; opacity: 1 }
        },
        State {
            name: "hidden"
        }
    ]

    transitions: Transition {
        from: "show"; to: "hidden"
        NumberAnimation { target: messages; property: "opacity"; to: 0; duration: 2000}
    }
}
