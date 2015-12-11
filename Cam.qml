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

import QtMultimedia 5.4
import QtQuick 2.1
import QtQuick.Controls 1.3
import QZXing 2.3

Frame {
    id: cameraUI
    width: Math.round(1030*base.scalex())
    height: Math.round(1920*base.scaley())

    property alias fields:fields

    onRefresh: {
        fields.up()
    }

    onUpdate: {
        refresh()
    }

    Component.onCompleted: {
        refresh()
        timer.start()
    }

    Fields
    {
        id: fields
        z: 10

        action.text: "Stop"
        action.onClicked: handlerLoader("qrc:///Keys.qml")
        actionExternal.text: base.trans(88)
        actionExternal.onClicked: {
            if (camera.flash.mode===Camera.FlashOff) camera.flash.mode=Camera.FlashVideoLight
            else camera.flash.mode=Camera.FlashOff
        }
    }

    MButton{
        id: focus
        x: Math.round(420*base.scalex())
        y: Math.round(600*base.scaley())
        text: base.trans(89)
        onClicked:{
            camera.unlock()
            camera.searchAndLock();
        }
    }

    Camera {
        id: camera
        captureMode: Camera.CaptureStillImage
        onCameraStatusChanged: {
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
        running: false
        repeat: true
        onTriggered: {
            camera.unlock()
            camera.searchAndLock();
            camera.imageCapture.capture()
        }
    }

    VideoOutput {
        x: Math.round(25*base.scalex())
        y: Math.round(800*base.scaley())
        width: parent.width
        height: Math.round(parent.height-900*base.scaley())

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
            parse(tag)
        }
        onDecodingFinished: {
            base.logError("Decoding finished " + (succeeded==true ? "successfully" :    "unsuccessfully") )
        }
    }
}
