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

#ifndef WATERSITESENGINE_H
#define WATERSITESENGINE_H
 
#include <Plasma/DataEngine>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomElement>

class WaterSitesEngine : public Plasma::DataEngine
{
    Q_OBJECT
 
    public:
        static const QString DEFAULT_SERVER;
        static const QString DEFAULT_FORMAT;
        static const int DEFAULT_MIN_POLLING = 15;

        static const QString PREFIX_NET;  // key naming scheme
        static const QString PREFIX_XML;
        //static const QString PREFIX_QUERYINFO; 
        //static const QString PREFIX_TIMESERIES;
        //static const QString PREFIX_SOURCEINFO;
        //static const QString PREFIX_VARIABLE;
        //static const QString PREFIX_VALUES;

        WaterSitesEngine(QObject* parent, const QVariantList& args);

    public slots:
        void dataFetchComplete(QNetworkReply *reply);
 
    protected:
        bool sourceRequestEvent(const QString& name);
        bool updateSourceEvent(const QString& source);

    private:
        QNetworkAccessManager *manager;
        QMap<QNetworkReply*, QString> *replies;

        void extractData( QString &request, QByteArray &bytes );
        //void extractQueryInfo( QString &request, QDomElement *element );
        //void extractTimeSeries( QString &request, QDomElement *element );
        //void extractSeriesSourceInfo( QString &request, QString &prefix, QDomElement *timeSeries );
        //void extractSeriesVariable( QString &request, QString &prefix, QDomElement *timeSeries );
        //void extractSeriesValues( QString &request, QString &prefix, QDomElement *timeSeries );

};
 
#endif
