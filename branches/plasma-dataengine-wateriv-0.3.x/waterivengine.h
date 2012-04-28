/**
    Copyright 2012 Forrest Guice
    This file is part of Plasma-WaterIV.

    Plasma-WaterIV is free software: you can redistribute it and/or 
    modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Plasma-WaterIV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Plasma-WaterIV.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WATERIVENGINE_H
#define WATERIVENGINE_H
 
#include <Plasma/DataEngine>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomElement>

class WaterIVEngine : public Plasma::DataEngine
{
    Q_OBJECT
 
    public:
        /** 
            VERSION_ID gets incremented every time compatibility is broken.
            Plasmoids can get this value with 'engine_version'.
        */
        static const int VERSION_ID = 4;   // 4 : 0.3.1 + 
                                           // 3 : 0.3.0
                                           // 2 : 0.2.1
                                           // 1 : 0.2.0 
                                           // 0 : 0.1.0

        static const QString DEFAULT_SERVER;
        static const QString DEFAULT_SERVER_IV;  // IV is Instantaneous Values
        static const QString DEFAULT_SERVER_DV;  // DV is Daily Values
        static const QString DEFAULT_FORMAT;
        static const int DEFAULT_MIN_POLLING = 15;

        static const QString PREFIX_NET;  // key naming scheme
        static const QString PREFIX_XML;
        static const QString PREFIX_QUERYINFO; 
        static const QString PREFIX_SERIES;
        static const QString PREFIX_TOC;

        static const int MAX_VALUES = 3;  //672 max accumulated values (1wk@15min)

        WaterIVEngine(QObject* parent, const QVariantList& args);
        void setEngineData( QString source, QString key, QVariant value );

    public slots:
       void dataFetchComplete(QNetworkReply *reply);
 
    protected:
        bool sourceRequestEvent(const QString& name);
        bool updateSourceEvent(const QString& source);

    private:
        QNetworkAccessManager *manager;
        QMap<QNetworkReply*, QString> *replies;
};
 
#endif
