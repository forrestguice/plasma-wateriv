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

#include "watersitesengine.h"
#include "sitesrequest.h"
#include "watersitesdata.h"
#include "watersitesdata_mapper.h"
#include "watersitesdata_statecodes.h"
#include "watersitesdata_agencycodes.h"

const QString WaterSitesEngine::DEFAULT_SERVER = "http://waterservices.usgs.gov/nwis/site";
const QString WaterSitesEngine::DEFAULT_FORMAT = "mapper,1.0";

const QString WaterSitesEngine::PREFIX_NET = "net_";
const QString WaterSitesEngine::PREFIX_XML = "xml_";
const QString WaterSitesEngine::PREFIX_SITE = "site_";
const QString WaterSitesEngine::PREFIX_STATECODE = "statecode_";
const QString WaterSitesEngine::PREFIX_COUNTYCODE = "countycode_";
const QString WaterSitesEngine::PREFIX_AGENCYCODE = "agencycode_";
const QString WaterSitesEngine::PREFIX_SITETYPE = "sitetypecode_";

/**
   The WaterSites data engine accesses data using the USGS Sites Web Service
   http://waterservices.usgs.gov/nwis/site. The web service accepts a request
   url (almost identical to the requests accepted by the IV/DV web services)
   and returns a resulting list of sites in the Mapper format (simple XML).
   The WaterSites data engine parses the resulting list of sites and makes it
   available to plasmoids through the site_<code> keys which can be
   displayed using a ListView.

   Request Examples: stateCd=AZ     .. returns info on all sites within AZ
                     sites=09506000 .. returns info on single site 09506000 
                     09506000       .. returns info on single site 09506000 

   See http://waterservices.usgs.gov/rest/Site-Test-Tool.html for help forming
   valid requests.
*/

/**
    Data Keys:

      engine_version      : int

      net_date            : QDateTime
      net_url             : QString
      net_request         : QString
      net_request_isvalid : bool
      net_request_error   : QString
      net_isvalid         : bool
      net_error           : QString

      xml_isvalid         : bool
      xml_error_msg       : QString
      xml_error_line      : int
      xml_error_column    : int

      site_count          : int
      site_#_<code>       :  QHash<QString, QVariant>
                             keys: code      : QString
                                   name      : QString
                                   agency    : QString
                                   latitude  : QString
                                   longitude : QString
                                   cat       : QString
*/
 
WaterSitesEngine::WaterSitesEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    setMinimumPollingInterval(WaterSitesEngine::DEFAULT_MIN_POLLING);

    manager = new QNetworkAccessManager(this);
    replies = new QMap<QNetworkReply*, QString>();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(remoteDataFetchComplete(QNetworkReply*)));
}

/**
   This function runs when a source is requested for the first time.
*/
bool WaterSitesEngine::sourceRequestEvent(const QString &source)
{
    setData(source, DataEngine::Data());
    updateSourceEvent(source);
    return true;
}
 
/**
   This function runs when a source requests an update; it initiates a fetch
   of the requested data. Execution continues afterward in dataFetchComplete.
*/
bool WaterSitesEngine::updateSourceEvent(const QString &source)
{
    // check to see if source already exists; exit early if it does
    Plasma::DataContainer *container = containerForSource(source); 
    if (container != 0  && 
        container->data().contains("net_isvalid") && 
        container->data().contains("net_date") &&
        container->data()["net_isvalid"].toBool() == true)
    {
        QDateTime requestDate = container->data()["net_date"].toDateTime();
        qDebug() << "watersites: exiting updateSourceEvent early: successfully downloaded data for this source exists: " << requestDate;
        return true;
    }

    // source DNE or was previously requested and dl failed
    QString errorMsg;
    bool dataIsRemote = true;
    QString requestUrl = SitesRequest::requestForSource(source, errorMsg, dataIsRemote);
    if (requestUrl == "-1")
    {
        invalidRequest(source, requestUrl, errorMsg);
        return true;    // invalid request

    } else if (dataIsRemote) {
        requestRemoteData(source, requestUrl);
        return false;   // network request

    } else {
        requestLocalData(source, requestUrl);
        return true;    // local request
    }
}

/**
    Request for data was invalid.
*/
void WaterSitesEngine::invalidRequest( const QString &source, const QString &requestUrl, QString &errorMsg )
{
    qDebug() << errorMsg;     
    QVariant resultsDate = QDateTime::currentDateTime();
    setData(source, I18N_NOOP("engine_version"), WaterSitesEngine::VERSION_ID);
    setData(source, I18N_NOOP(PREFIX_NET + "request"), requestUrl);
    setData(source, I18N_NOOP(PREFIX_NET + "request_isvalid"), false);
    setData(source, I18N_NOOP(PREFIX_NET + "request_error"), errorMsg);
    setData(source, I18N_NOOP(PREFIX_NET + "isvalid"), false);
    setData(source, I18N_NOOP(PREFIX_NET + "date"), resultsDate);
}

/**
    Request the data from over the network; resume processing in remoteDataFetchComplete.
*/
void WaterSitesEngine::requestRemoteData( const QString &source, const QString &requestUrl )
{
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(requestUrl)));
    replies->insert(reply, source);
}

/**
    Request the data from the local filesystem.
*/
void WaterSitesEngine::requestLocalData( const QString &source, const QString &requestUrl )
{
    setData(source, I18N_NOOP("engine_version"), WaterSitesEngine::VERSION_ID);
    setData(source, I18N_NOOP(PREFIX_NET + "request"), source);
    setData(source, I18N_NOOP(PREFIX_NET + "request_isvalid"), true);
    setData(source, I18N_NOOP(PREFIX_NET + "isvalid"), true);
    setData(source, I18N_NOOP(PREFIX_NET + "url"), requestUrl);

    QFile file(requestUrl);
    if (not file.open(QFile::ReadOnly))
    {
        qDebug() << "watersites: local open failed:" << requestUrl;
        return;
    }

    qDebug() << "watersites: local open complete";
    QByteArray bytes = file.readAll();
    extractData(source, bytes);
}

/**
   This slot runs after each download request completes. It checks for and
   handles redirection, then passes the returned bytes to extractData()
*/
void WaterSitesEngine::remoteDataFetchComplete(QNetworkReply *reply)
{
    QString source = replies->take(reply);

    QUrl requestUrl = reply->request().url();   // check for redirection
    QVariant redirectVariant = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl redirectUrl = redirectVariant.toUrl();
    if (!redirectUrl.isEmpty() && redirectUrl != requestUrl)
    {
        QNetworkReply *reply2 = manager->get(QNetworkRequest(redirectUrl));
        replies->insert(reply2, source);       // redirected
        reply->deleteLater();
        return;
    }

    bool noNetErrors = (reply->error() == QNetworkReply::NoError);

    QStringList request_parts = requestUrl.toString().split("?");
    setData(source, I18N_NOOP("engine_version"), WaterSitesEngine::VERSION_ID);
    setData(source, I18N_NOOP(PREFIX_NET + "url"), QUrl(request_parts.at(0)));
    setData(source, I18N_NOOP(PREFIX_NET + "request"), request_parts.at(1));
    setData(source, I18N_NOOP(PREFIX_NET + "request_isvalid"), true);
    setData(source, I18N_NOOP(PREFIX_NET + "isvalid"), noNetErrors);

    if (noNetErrors)
    {
        qDebug() << "watersites: download complete";
        QByteArray bytes = reply->readAll();
        extractData(source, bytes);

    } else {
        qDebug() << "watersites: download failed";
        setData(source, I18N_NOOP(PREFIX_NET + "error"), reply->error());
    }
    reply->deleteLater();
}

/**
    Attempt to extract the data in bytes that resulted from request using format.
*/
void WaterSitesEngine::extractData( const QString &source, QByteArray &bytes )
{
    QVariant resultsDate = QDateTime::currentDateTime();
    WaterSitesData *reader = SitesRequest::formatForSource(source);
    reader->extractData(this, source, bytes);
    setData(source, I18N_NOOP(PREFIX_NET + "date"), resultsDate);
    delete reader;
}

/**
*/
void WaterSitesEngine::setEngineData( QString source, QString key, QVariant value )
{
    setData(source, key, value);
}

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(watersites, WaterSitesEngine)
 
#include "watersitesengine.moc"
