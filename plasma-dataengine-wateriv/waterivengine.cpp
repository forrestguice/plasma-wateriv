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

#include "waterivengine.h"
#include "ivrequest.h"
#include "waterivdata_waterml.h"
#include <Plasma/DataContainer> //#include <KSystemTimeZones> //#include <KDateTime>

const QString WaterIVEngine::DEFAULT_SERVER_IV = "http://waterservices.usgs.gov/nwis/iv/";
const QString WaterIVEngine::DEFAULT_SERVER_DV = "http://waterservices.usgs.gov/nwis/dv/";
const QString WaterIVEngine::DEFAULT_FORMAT = "waterml,1.1";
const QString WaterIVEngine::DEFAULT_SERVER = DEFAULT_SERVER_IV;

/**
   This dataengine retrieves timeseries data from the USGS Instantaneous Values
   Web Service using REST. The web service returns the data as an XML 
   document using a set of tags called "WaterML". The dataengine reads these
   tags (DOM) and makes the data available to plasmoids using a simple
   naming scheme for available keys.

   ----------------------
   The source name can be:
   ----------------------
   1) a site code, or comma separated list of site codes (up to 100).  Data will
      be requested from the Instantaneous Values [IV] service.
      See http://wdr.water.usgs.gov/nwisgmap/index.html.

   2) a request string specifying the data to retrieve (the part after 
      the ? in a complete request url). Data will be requested from the 
      Instantaneous Values [IV] service.
      See http://waterservices.usgs.gov/rest/IV-Test-Tool.html.
      Example: sites=01646500&parameterCd=00060,00065

   3) a complete request url specifying the data to retrieve 
      (see http://waterservices.usgs.gov/rest/IV-Test-Tool.html)
      Example: http://waterservices.usgs.gov/nwis/iv?sites=01646500&parameterCd=00060,00065

   4) a psuedo url composed of [IV|DV]?partialurl. Using IV will request
      data from the Instantaneous Values service. Using DV will request
      data from the Daily Values service.
      Examples: DV?sites=01646500  ..  IV?sites=01646500

   Using (1) is a convenient way to get recent data by site code.
   Using (2) allows more control over the data that is requested.
   Using (3) allows data to be requested from an alternate url.

   ----------------------------
   All requests will result in:
   ----------------------------
   1) A set of queryinfo describing the request
   2) one or more timeseries, each containing:
    --> 3) sourceinfo
    --> 4) variable info
    --> 5) a set of datetime:value pairs

   See "Naming scheme for available keys" in the next block of comments.
*/

const QString WaterIVEngine::PREFIX_NET = "net_";
const QString WaterIVEngine::PREFIX_XML = "xml_";
const QString WaterIVEngine::PREFIX_QUERYINFO = "queryinfo_";
const QString WaterIVEngine::PREFIX_TOC = "toc_";
const QString WaterIVEngine::PREFIX_SERIES = "series_";
/**
  ---------------------------------------
   Available data:
  ---------------------------------------

  --> engine_version                                : int

  --> net_url                                       : QString
  --> net_request                                   : QString
  --> net_request_isvalid                           : bool
  --> net_request_error                             : QString
  --> net_isvalid                                   : bool
  --> net_error                                     : int

  --> xml_isvalid                                    : bool
  --> xml_schema                                     : QString
  --> xml_error_msg                                  : QString
  --> xml_error_line                                 : int
  --> xml_error_column                               : int

  --> queryinfo_url                                  : QString
  --> queryinfo_time                                 : QDateTime
  --> queryinfo_notes                                : QMap<String, QVariant> (by title)
  --> queryinfo_location                             : QString
  --> queryinfo_variable                             : QString

  --> toc_#                                          : QMap<QString, QVariant>
  --> toc_count                                      : int

  --> series_<siteCode>_name                         : QString
  --> series_<siteCode>_code                         : QString
  --> series_<siteCode>_properties                   : QMap<String, QVariant> (by name)
  --> series_<siteCode>_latitude                     : float
  --> series_<siteCode>_longitude                    : float
  --> series_<siteCode>_timezone_name                : QString
  --> series_<siteCode>_timezone_offset              : ...
  --> series_<siteCode>_timezone_daylight            : bool
  --> series_<siteCode>_timezone_daylight_name       : QString
  --> series_<siteCode>_timezone_daylight_offset     : ...

  --> series_<siteCode>_<paramCode>_name             : QString
  --> series_<siteCode>_<paramCode>_code             : QString
  --> series_<siteCode>_<paramCode>_code_attributes  : QMap<QString, QVariant> (by attrib)
  --> series_<siteCode>_<paramCode>_description      : QString
  --> series_<siteCode>_<paramCode>_valuetype        : QString
  --> series_<siteCode>_<paramCode>_unitcode         : QString
  --> series_<siteCode>_<paramCode>_nodatavalue      : double

  --> series_<siteCode>_<paramCode>_<statCode>_name                        : QString
  --> series_<siteCode>_<paramCode>_<statCode>_code                        : int
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_id               : int
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_description      : QString
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_all              : QMap<QString, QList<QVariant>>
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_qualifiers       : QMap<QString, QList<QVariant>>
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_recent_value     : double
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_recent_date      : QDateTime
  --> series_<siteCode>_<paramCode>_<statCode>_<methodId>_recent_qualifier : QString


  Complex Key Contents:

  --> toc_#      : QMap<QString, QVariant>
                    : site       : QString
                    : variable   : QString
                    : statistic  : QString
                    : method     : QString

  The components of the toc_# can assembled into keys using 
  this pattern: series_<site>_<variable>_<statistic>_<methodId>

  --> all        : QMap<QString, QList<QVariant>> containing all values.
                 : the map key is the datetime string of the value.
                 : each QList contains two values:
                 : [0] the value (e.g. 200)
                 : [1] value qualifiers by code (e.g. "P" is provisional)

  --> qualifiers  : a QMap<QString, QList<QVariant>> of qualifiers (by code)
                  : the map key is the qualifier code (e.g. "P")
                  : each QList contains 5 values:
                     : [0] qualifier id (e.g. 0 : an integer id)
                     : [1] qualifier code (e.g. "P" same as map key)
                     : [2] qualifier description (e.g. "Provisional)
                     : [3] qualifier network (?)
                     : [4] qualifier vocabulary (?)
*/

WaterIVEngine::WaterIVEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    setMinimumPollingInterval(WaterIVEngine::DEFAULT_MIN_POLLING * 60000);

    manager = new QNetworkAccessManager(this);
    replies = new QMap<QNetworkReply*, QString>();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dataFetchComplete(QNetworkReply*)));
}

/**
   This function runs when a source is requested for the first time.
*/
bool WaterIVEngine::sourceRequestEvent(const QString &source)
{
    setData(source, DataEngine::Data());
    updateSourceEvent(source);
    return true;
}
 
/**
   This function runs when a source requests an update; it initiates a fetch
   of the requested data. Execution continues afterward in dataFetchComplete.
*/
bool WaterIVEngine::updateSourceEvent(const QString &source)
{
    QString errorMsg;
    bool dataIsRemote = true;
    QString requestUrl = IVRequest::requestForSource(source, errorMsg, dataIsRemote);
    if (requestUrl == "-1")
    {
        //qDebug() << errorMsg;
        setData(source, I18N_NOOP("engine_version"), WaterIVEngine::VERSION_ID);
        setData(source, I18N_NOOP(PREFIX_NET + "request_isvalid"), false);
        setData(source, I18N_NOOP(PREFIX_NET + "request_error"), errorMsg);
        setData(source, I18N_NOOP(PREFIX_TOC + "count"), 0);
        return true;
    }

    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(requestUrl)));
    replies->insert(reply, source);
    return false;
}

/**
   This slot runs after each download request completes. It checks for and
   handles redirection, then passes the returned bytes to extractData()
*/
void WaterIVEngine::dataFetchComplete(QNetworkReply *reply)
{
    QString request = replies->take(reply);

    // check for redirection
    QUrl requestUrl = reply->request().url();
    QVariant redirectVariant = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl redirectUrl = redirectVariant.toUrl();
    if (!redirectUrl.isEmpty() && redirectUrl != requestUrl)
    {
        // qDebug() << "Redirecting: " << redirectUrl << "( " << requestUrl << ")";
        QNetworkReply *reply2 = manager->get(QNetworkRequest(redirectUrl));
        replies->insert(reply2, request);
        reply->deleteLater();
        return;
    }

    // data - request url
    QStringList request_parts = requestUrl.toString().split("?");
    setData(request, I18N_NOOP("engine_version"), WaterIVEngine::VERSION_ID);
    setData(request, I18N_NOOP(PREFIX_NET + "request_isvalid"), true);
    setData(request, I18N_NOOP(PREFIX_NET + "url"), QUrl(request_parts.at(0)));
    setData(request, I18N_NOOP(PREFIX_NET + "request"), request_parts.at(1));

    // data - check for retrieval errors
    if (reply->error() != QNetworkReply::NoError)
    {
        // qDebug() << "download failed";
        Plasma::DataContainer *container = containerForSource(request);
        if (container == 0 || not container->data().contains(PREFIX_TOC + "count"))
        {
            // qDebug() << "timeseries unset; setting to 0";
            setData(request, I18N_NOOP(PREFIX_TOC + "count"), 0);
        }

        setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_NET + "error"), reply->error());
        reply->deleteLater();
        return;
    }

    // qDebug() << "download complete";
    setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), true);
    QByteArray bytes = reply->readAll();
    reply->deleteLater();

    WaterIVData *reader = IVRequest::formatForSource(request, request_parts.at(1));
    reader->extractData(this, request, bytes);
    delete reader;
}

void WaterIVEngine::setEngineData( QString source, QString key, QVariant value )
{
    setData( source, key, value );
}

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(wateriv, WaterIVEngine)
 
#include "waterivengine.moc"
