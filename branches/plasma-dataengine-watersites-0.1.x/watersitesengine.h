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
#include <Plasma/DataContainer>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomElement>

class WaterSitesEngine : public Plasma::DataEngine
{
    Q_OBJECT
 
    public:
        /** 
            VERSION_ID is incremented whenever compatibility breaks. Plasmoids
            can get this value with 'engine_version'.
        */
        static const int VERSION_ID = 0;    // 0 : 0.1.0

        static const QString DEFAULT_SERVER;
        static const QString DEFAULT_FORMAT;
        static const int DEFAULT_MIN_POLLING = 100;  // units: ms

        static const QString PREFIX_NET;  // key naming scheme
        static const QString PREFIX_XML;
        static const QString PREFIX_TOC;
        static const QString PREFIX_SITE;
        static const QString PREFIX_STATECODE;
        static const QString PREFIX_COUNTYCODE;
        static const QString PREFIX_AGENCYCODE;
        static const QString PREFIX_SITETYPE;

        WaterSitesEngine(QObject* parent, const QVariantList& args);
        void setEngineData( QString source, QString key, QVariant value );

    public slots:
        void remoteDataFetchComplete(QNetworkReply *reply);
 
    protected:
        bool sourceRequestEvent(const QString& name);
        bool updateSourceEvent(const QString& source);

    private:
        QNetworkAccessManager *manager;
        QMap<QNetworkReply*, QString> *replies;

        void invalidRequest( const QString &source, const QString &request, QString &errorMsg );
        void requestLocalData( const QString &source, const QString &request );
        void requestRemoteData( const QString &source, const QString &request );
        void extractData( const QString &source, QByteArray &bytes );

};
 
#endif
