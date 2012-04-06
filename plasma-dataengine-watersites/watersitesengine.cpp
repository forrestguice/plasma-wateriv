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

//#include <Plasma/DataContainer> //#include <QDate> //#include <QTime> 
//#include <KSystemTimeZones> //#include <KDateTime>

const QString WaterSitesEngine::DEFAULT_SERVER = "http://waterservices.usgs.gov/nwis/site";
const QString WaterSitesEngine::DEFAULT_FORMAT = "mapper,1.0";

const QString WaterSitesEngine::PREFIX_NET = "net_";
const QString WaterSitesEngine::PREFIX_XML = "xml_";
const QString WaterSitesEngine::PREFIX_SITE = "site_";

/**
    Data Keys:

      engine_version      : int

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

      site_<CODE>          :  QHash<QString, QVariant>
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
    setMinimumPollingInterval(WaterSitesEngine::DEFAULT_MIN_POLLING * 60000);

    manager = new QNetworkAccessManager(this);
    replies = new QMap<QNetworkReply*, QString>();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dataFetchComplete(QNetworkReply*)));
}

/**
   This function runs when a source is requested for the first time.
*/
bool WaterSitesEngine::sourceRequestEvent(const QString &source)
{
    return updateSourceEvent(source);
}
 
/**
   This function runs when a source requests an update; it initiates a fetch
   of the requested data. Execution continues afterward in dataFetchComplete.
*/
bool WaterSitesEngine::updateSourceEvent(const QString &source)
{
    setData(source, "engine_version", WaterSitesEngine::VERSION_ID);
    QString errorMsg;
    QString requestUrl = SitesRequest::requestForSource(source, errorMsg);

    if (requestUrl == "-1")
    {
        qDebug() << errorMsg;
        setData(request, I18N_NOOP(PREFIX_NET + "request_isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_NET + "request_error"), errorMsg);
        return false;
    }

    setData(request, I18N_NOOP(PREFIX_NET + "request_isvalid"), true);
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(requestUrl)));
    replies->insert(reply, source);
    return false;
}

/**
   This slot runs after each download request completes. It checks for and
   handles redirection, then passes the returned bytes to extractData()
*/
void WaterSitesEngine::dataFetchComplete(QNetworkReply *reply)
{
    QString request = replies->take(reply);

    // check for redirection
    QUrl requestUrl = reply->request().url();
    QVariant redirectVariant = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl redirectUrl = redirectVariant.toUrl();
    if (!redirectUrl.isEmpty() && redirectUrl != requestUrl)
    {
        // redirected - create a new QNetworkRequest
        QNetworkReply *reply2 = manager->get(QNetworkRequest(redirectUrl));
        replies->insert(reply2, request);
        reply->deleteLater();
        return;
    }

    // data - request url
    QStringList request_parts = requestUrl.toString().split("?");
    setData(request, I18N_NOOP(PREFIX_NET + "url"), QUrl(request_parts.at(0)));
    setData(request, I18N_NOOP(PREFIX_NET + "request"), request_parts.at(1));

    // data - check for retrieval errors
    if (reply->error() != QNetworkReply::NoError)
    {
        setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_NET + "error"), reply->error());
        reply->deleteLater();
        return;
    }

    setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), true);
    QByteArray bytes = reply->readAll();
    reply->deleteLater();

    extractData(request, bytes);
}

/**
*/
void WaterSitesEngine::extractData( QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        if (document.tagName() == "mapper")
        {
            // appears valid - extract data
            setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), true);
            extractSites(request, &document);

        } else {
            // invalid - return error data
            setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
            setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), "XML Error: format is not mapper.");
            setData(request, I18N_NOOP(PREFIX_XML + "error_line"), -1);
            setData(request, I18N_NOOP(PREFIX_XML + "error_column"), -1);
        }

    } else {
        // invalid xml - return error data
        setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), errorMsg);
        setData(request, I18N_NOOP(PREFIX_XML + "error_line"), errorLine);
        setData(request, I18N_NOOP(PREFIX_XML + "error_column"), errorColumn);
    }
}

/**
*/
void WaterSitesEngine::extractSites( QString &request, QDomElement *document )
{
    int siteCount = 0;

    QDomNodeList sitesList = document->elementsByTagName("sites");
    int num_sitescomponents = sitesList.length();
    for (int i=0; i<num_sitescomponents; i++)
    {
        QDomElement sites = sitesList.at(i).toElement();
        QDomNodeList siteList = sites.elementsByTagName("site");

        int num_sites = siteList.length();
        siteCount += num_sites;
        for (int j=0; j<num_sites; j++)
        {
            QDomElement site = siteList.at(i).toElement();
  
            QHash<QString,QVariant> siteHash;
            siteHash["code"] = site.attribute("sno", "-1");
            siteHash["name"] = site.attribute("sna", "-1");
            siteHash["agency"] = site.attribute("agc", "");
            siteHash["latitude"] = site.attribute("lat", "-1");
            siteHash["longitude"] = site.attribute("lng", "-1");
            siteHash["cat"] = site.attribute("cat", "");

            setData(request, PREFIX_SITE + siteHash["code"].toString(), siteHash);
        }
    }

    QDomNodeList cSiteList = document->elementsByTagName("colocated_sites");
    int num_csitesets = cSiteList.length();
    for (int i=0; i<num_csitesets; i++)
    {
        QDomElement csite = cSiteList.at(i).toElement();
        QDomNodeList siteList = csite.elementsByTagName("site");

        int num_sites = siteList.length();
        siteCount += num_sites;
        for (int j=0; j<num_sites; j++)
        {
            QDomElement site = siteList.at(j).toElement();

            QHash<QString,QVariant> siteHash;
            siteHash["code"] = site.attribute("sno", "-1");
            siteHash["name"] = site.attribute("sna", "-1");
            siteHash["agency"] = site.attribute("agc", "");
            siteHash["latitude"] = site.attribute("lat", "-1");
            siteHash["longitude"] = site.attribute("lng", "-1");
            siteHash["cat"] = site.attribute("cat", "");

            setData(request, PREFIX_SITE + siteHash["code"].toString(), siteHash);
        }
    }
    setData(request, PREFIX_SITE + "count", siteCount);
}

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(watersites, WaterSitesEngine)
 
// needed since WaterSitesEngine is a QObject // todo: whats going on here...
//#include "watersitesengine.moc"
