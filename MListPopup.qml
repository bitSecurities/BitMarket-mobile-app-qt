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

Popup {
    id: cameraListPopup
    z: 10

    property alias model : view.model
    property variant currentName : model.get(view.currentIndex).name
    property variant currentItem : model.get(view.currentIndex)
    property alias view : view

    property int itemWidth : Math.round(260*base.scalex())
    property int itemHeight : Math.round(130*base.scaley())

    width: itemWidth + view.anchors.margins*2
    height: view.count * itemHeight + view.anchors.margins*2

    signal selected

    function ref()
    {
          currentName=model.get(view.currentIndex).name
          currentItem=model.get(view.currentIndex)
    }

    ListView {
        id: view
        anchors.fill: parent
        anchors.margins: Math.round(4*base.scalex())
        snapMode: ListView.SnapOneItem
        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "#ffffff"; radius: Math.round(15*base.scalex()) }
        currentIndex: 0

        delegate: Item {
            width: cameraListPopup.itemWidth
            height: cameraListPopup.itemHeight

            Text {
                id: nametxt
                text: name
                anchors.fill: parent
                anchors.margins: Math.round(4*base.scalex())
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                color: (view.currentIndex==index)? "#41bb1a":"#ffffff"
                font.pixelSize: Math.round(35*base.scalex())
                visible: ((text==="Bitmarket")||(text==="Bitmaszyna"))?false:true
            }

            Image {
                id: image
                x: Math.round(20*base.scalex())
                y: Math.round(35*base.scaley())
                width: Math.round(210*base.scalex())
                height: Math.round(56*base.scaley())
                source:
                {
                    if (nametxt.text==="Bitmarket")
                    {
                        if (view.currentIndex==index) "qrc:///images/bitmarket.png"
                        else "qrc:///images/bitmarketwhite.png"
                    }else if (nametxt.text==="Bitmaszyna")
                    {
                        if (view.currentIndex==index) "qrc:///images/bitmaszyna.png"
                        else "qrc:///images/bitmaszynawhite.png"
                    }else ""
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    view.currentIndex = index
                    cameraListPopup.selected(name)
                    ref()
               }
            }
        }
    }
}
