/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Item {
    id: listButton
    property alias name : popup.currentName
    property alias model : popup.model
    property alias popup : popup
    property alias view : popup.view
    property alias image : image.source
    property alias bcolor : button.color
    property alias text : button.text
    property alias inner : inner

    width : Math.round(260*base.scalex())
    height: Math.round(85*base.scaley())
    visible: true

    MButton {
        id: button
        anchors.fill: parent
        text: popup.currentItem.name
        onClicked: popup.toggle()
        background: "transparent"
        z: 10
    }

    Rectangle
    {
        id: inner
        z: 0
        anchors.fill: parent
        color: "#41bb1a"
        border.width: Math.ceil(1*base.scalex())
        border.color: "#ffffff"
        radius: Math.round(8*base.scalex())
    }

    Image {
        id: image
        x: Math.round(25*base.scalex())
        y: Math.round(15*base.scaley())
        width: Math.round(210*base.scalex())
        height: Math.round(56*base.scaley())
    }

    MListPopup {
        id: popup
        anchors.top: parent.bottom
        anchors.topMargin: 4*base.scaley()
        visible: opacity > 0
        onSelected: {
            popup.toggle()
        }
    }
}
