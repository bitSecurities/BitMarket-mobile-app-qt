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

#ifndef TABLE_H
#define TABLE_H
#include <QObject>
#include <QAbstractTableModel>
#include "basedata.h"
#include "Base.h"

class ListObject : public QObject
{
    Q_OBJECT
    QString nn;

    Q_PROPERTY(QString name READ name NOTIFY notif)
    Q_PROPERTY(QString value READ value NOTIFY notif)
public:

    ListObject(QString name)
    {
        this->nn=name;
    }

    QString name()
    {
        return(nn);
    }

    QString value()
    {
        return(nn);
    }
signals:
    void notif();
};

class BaseData;
class Table : public QAbstractTableModel
{
    Q_OBJECT

    BaseData *base;
    enum Role {
        Price=Qt::UserRole+1,
        Amount,
        Sum,
        Action,
        Type,
        Time,
        Tid,
        Curr,
        Available,
        Blocked,
        Total,
        Opened,
        Leverage,
        Security,
        Index,
        Earnings,
        FiatOpened,
        FiatTotal,
        TakeProfit,
        StopLoss
    };

public:
    QString dd;
    int type;

    explicit Table(BaseData *base,int type);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent= QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    Q_INVOKABLE QVariant get(int row);
    QString formatCurr(double,int) const;
    //QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows(int startRow,int endrow,const QModelIndex &parent = QModelIndex());
    bool removeRows(int startRow,int endrow,const QModelIndex &parent = QModelIndex());

protected:
  std::vector<std::vector<float> > Columns;
};

#endif // TABLE_H
