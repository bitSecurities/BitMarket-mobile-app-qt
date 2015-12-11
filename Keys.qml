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

Frame {
    property alias fields : fields

    Component.onCompleted: clear()

    onClear: {
        fields.up()
    }

    onRefresh: {
        parse(base.getscannedtxt())
    }

    onUpdate: refresh()

    Fields
    {
        id: fields

        action.text: base.trans(17)
        action.onClicked: handlerLoader("qrc:///Cam.qml")
        actionExternal.text: base.trans(90)
        actionExternal.onClicked: base.scanner()
    }
}
