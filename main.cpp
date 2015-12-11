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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "table.h"
#include "basedata.h"
#include "QZXing/QZXing.h"
#include "wykres.h"

BaseData *base;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QString models[]={"modelbid","modelask","modelmarkets","modelexchanges","modelorders","modelbalance","modellast","modelmenu","modelleverage","modelpositions","modelswaps","modelorder","modelhistory","modelordertype"};

    setlocale(LC_NUMERIC,"en_US.UTF-8");
    QZXing::registerQMLTypes();
    base=new BaseData();
    for(int i=0;i<MAXMODELS;i++) {
        base->model[i]=new Table(base,i);
        engine.rootContext()->setContextProperty(models[i], base->model[i]);
    }
    qmlRegisterType<Wykres>("Wykres", 1, 0, "Wykres");
    app.setFont(QFont("qrc:///fonts/MyriadPro-Regular.otf"));
    engine.rootContext()->setContextProperty("base", base);
    engine.load(QUrl("qrc:///main.qml"));
    QList<QObject*>  temp = engine.rootObjects();
    QObject *topLevel = temp.value(0);
    base->msgObject = topLevel->findChild<QObject*>("messages");
    base->newDataObject = topLevel->findChild<QObject*>("newdata");
    base->window = qobject_cast<QQuickWindow *>(topLevel);
    if ( !base->window ) {
        qWarning("Error: Your root item has to be a Window.");
        return -1;
    }
#ifdef ANDROID
    base->window->show();
#else
    base->window->resize(base->width,base->height);
    base->window->show();
#endif
    return app.exec();
}
