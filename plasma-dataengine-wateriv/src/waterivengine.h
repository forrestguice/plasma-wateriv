/**
    Copyright 2012 Forrest Guice
    This file is part of WaterIV DataEngine.

    WaterIV DataEngine is free software: you can redistribute it and/or 
    modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WaterIV DataEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WaterIV DataEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WATERIVENGINE_H
#define WATERIVENGINE_H
 
#include <Plasma/DataEngine>
#include <QtNetwork>
#include <QDomElement>

class WaterIVEngine : public Plasma::DataEngine
{
    Q_OBJECT
 
    public:
        /** 
           USGS Instantaneous Values Web Service address 
        */
        static const QString DEFAULT_SERVER;

        /** 
           Minimum polling interval (minutes). Do not set to less than 15 min.
           An hour is probably an even better minimum.
        */
        static const int DEFAULT_MIN_POLLING = 15;

        /** 
           Naming scheme for returned data 
        */
        static const QString PREFIX_NET;
        static const QString PREFIX_XML;
        static const QString PREFIX_QUERYINFO; 
        static const QString PREFIX_TIMESERIES;
        static const QString PREFIX_SOURCEINFO;
        static const QString PREFIX_VARIABLE;
        static const QString PREFIX_VALUES;

        WaterIVEngine(QObject* parent, const QVariantList& args);

    public slots:
       void dataFetchComplete(QNetworkReply *reply);
 
    protected:
        bool sourceRequestEvent(const QString& name);
        bool updateSourceEvent(const QString& source);

    private:
        QNetworkAccessManager *manager;
        QMap<QNetworkReply*, QString> *replies;

        static bool hasMajorFilter( const QString &request );
        static bool isSiteCode( const QString &request );
        static bool isStateCode( const QString &request );
        static bool isHucCode( const QString &request );
        static bool isBBoxCode( const QString &request );
        static bool isCountyCode( const QString &request );

        void extractData( QString &request, QByteArray &bytes );
        void extractQueryInfo( QString &request, QDomElement *element );
        void extractTimeSeries( QString &request, QDomElement *element );
        void extractSeriesSourceInfo( QString &request, QString &prefix, QDomElement *timeSeries );
        void extractSeriesVariable( QString &request, QString &prefix, QDomElement *timeSeries );
        void extractSeriesValues( QString &request, QString &prefix, QDomElement *timeSeries );
};
 
#endif
