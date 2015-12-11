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

import QtQuick 2.0
import "qqr.js" as QRCodeBackend

Canvas {
    id: canvas
    // background colour to be used
    property color background : "#ededed"
    // foreground colour to be used
    property color foreground : "black"
    // ECC level to be applied (e.g. L, M, Q, H)
    property string level : "L"
    // value to be encoded in the generated QR code
    property string value : ""

    onPaint : {
        var qr = QRCodeBackend.get_qr()
        qr.canvas({
            background : canvas.background,
            canvas : canvas,
            value: canvas.value,
            foreground : canvas.foreground,
            level : canvas.level,
            side : Math.min(canvas.width, canvas.height),
            value : canvas.value
        })
    }
    onHeightChanged : {
        requestPaint()
    }

    onWidthChanged : {
        requestPaint()
    }

    onBackgroundChanged : {
        requestPaint()
    }

    onForegroundChanged : {
        requestPaint()
    }

    onLevelChanged : {
        requestPaint()
    }

    onValueChanged : {
        requestPaint()
    }
}
