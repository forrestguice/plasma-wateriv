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
    QString requestUrl = IVRequest::requestForSource(source, errorMsg);
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
    extractData(request, bytes);
}

/**
   Parses the passed QByteArray data as xml using QDomDocument.
*/
void WaterIVEngine::extractData( QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        setData(request, I18N_NOOP(PREFIX_XML + "schema"), document.attribute("xsi:schemaLocation", "-1"));
        if (document.tagName() == "ns1:timeSeriesResponse")
        {
            // appears valid waterml - extract data
            setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), true);
            extractQueryInfo(request, &document);
            extractTimeSeries(request, &document);

        } else {
            // invalid waterml - return error data
            setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
            setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), "XML Error: not WaterML (parent element is not timeSeriesResponse).");
            setData(request, I18N_NOOP(PREFIX_XML + "error_line"), -1);
            setData(request, I18N_NOOP(PREFIX_XML + "error_column"), -1);
            setData(request, I18N_NOOP(PREFIX_TOC + "count"), 0);
        }

    } else {
        // invalid xml - return error data
        setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), errorMsg);
        setData(request, I18N_NOOP(PREFIX_XML + "error_line"), errorLine);
        setData(request, I18N_NOOP(PREFIX_XML + "error_column"), errorColumn);
        setData(request, I18N_NOOP(PREFIX_TOC + "count"), 0);
    }
}

void WaterIVEngine::extractQueryInfo( QString &request, QDomElement *document )
{
    QDomElement queryInfo = document->elementsByTagName("ns1:queryInfo").at(0).toElement();
    QDomElement queryInfo_time = queryInfo.elementsByTagName("ns2:creationTime").at(0).toElement();
    QDomElement queryInfo_url = queryInfo.elementsByTagName("ns2:queryURL").at(0).toElement();
    QDomElement queryInfo_criteria = queryInfo.elementsByTagName("ns2:criteria").at(0).toElement();
    QDomElement queryInfo_location = queryInfo_criteria.elementsByTagName("ns2:locationParam").at(0).toElement();
    QDomElement queryInfo_variable = queryInfo_criteria.elementsByTagName("ns2:variableParam").at(0).toElement();

    // gather query notes
    QDomNodeList queryInfo_notesList = queryInfo.elementsByTagName("ns2:note");
    QMap<QString, QVariant> queryInfo_notes;
    int num_notes = queryInfo_notesList.length();
    for (int i=0; i<num_notes; i++)
    {
        QDomElement note = queryInfo_notesList.at(i).toElement();
        QString title = note.attribute("title", "-1");
        queryInfo_notes[title] = note.text();
    }

    // parse query time into QDateTime (iso 8601)
    QDateTime queryDateTime = QDateTime::fromString(queryInfo_time.text(), Qt::ISODate);

    // set the query data
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "url"), QUrl(queryInfo_url.text()));
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "time"), queryDateTime);
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "notes"), queryInfo_notes);
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "location"), queryInfo_location.text());
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "variable"), queryInfo_variable.text());
}

/**
   Extract the timeseries (ns1:timeSeries) from the supplied QDomElement 
   (ns1:timeSeriesResponse).
*/
void WaterIVEngine::extractTimeSeries( QString &request, QDomElement *document )
{
    int count = 0;        // gather time series
    QDomNodeList timeSeriesList = document->elementsByTagName("ns1:timeSeries");
    int num_series = timeSeriesList.length();
    for (int i=0; i<num_series; i++)
    {
        QDomElement timeSeries = timeSeriesList.at(i).toElement();
        QString timeSeries_name = timeSeries.attribute("name", "-1");

        QString prefix = PREFIX_SERIES;
        prefix = extractSeriesSourceInfo(request, prefix, &timeSeries);
        prefix = extractSeriesVariable(request, prefix, &timeSeries);
        extractSeriesValues(request, prefix, count, &timeSeries);
    }

    setData(request, I18N_NOOP(PREFIX_TOC + "count"), count);
}

/**
   Extract the sourceInfo from the supplied QDomElement (ns1:timeSeries)
*/
QString WaterIVEngine::extractSeriesSourceInfo( QString &request, QString &prefix, QDomElement *timeSeries )
{
    QDomElement sourceInfo = timeSeries->elementsByTagName("ns1:sourceInfo").at(0).toElement();
    QDomElement sourceInfo_siteName = sourceInfo.elementsByTagName("ns1:siteName").at(0).toElement();
    QDomElement sourceInfo_siteCode = sourceInfo.elementsByTagName("ns1:siteCode").at(0).toElement();

    QDomElement sourceInfo_geoLocation = sourceInfo.elementsByTagName("ns1:geoLocation").at(0).toElement();
    QDomElement sourceInfo_geoLocation_lat = sourceInfo_geoLocation.elementsByTagName("ns1:latitude").at(0).toElement();
    QDomElement sourceInfo_geoLocation_lon = sourceInfo_geoLocation.elementsByTagName("ns1:longitude").at(0).toElement();

    QDomElement sourceInfo_timeZone = sourceInfo.elementsByTagName("ns1:timeZoneInfo").at(0).toElement();
    QDomElement timeZone_default = sourceInfo_timeZone.elementsByTagName("ns1:defaultTimeZone").at(0).toElement();
    QDomElement timeZone_daylight = sourceInfo_timeZone.elementsByTagName("ns1:daylightSavingsTimeZone").at(0).toElement();

    // gather site properties
    QDomNodeList sourceInfo_propertiesList = sourceInfo.elementsByTagName("ns1:siteProperty");
    QMap<QString, QVariant> sourceInfo_properties;
    int num_properties = sourceInfo_propertiesList.length();
    for (int j=0; j<num_properties; j++)
    {
        QDomElement property = sourceInfo_propertiesList.at(j).toElement();
        QString name = property.attribute("name", "-1");
        sourceInfo_properties[name] = property.text();
    }

    // set site properties
    QString p = prefix + sourceInfo_siteCode.text() + "_";
    setData(request, I18N_NOOP(p + "name"), sourceInfo_siteName.text());
    setData(request, I18N_NOOP(p + "code"), sourceInfo_siteCode.text());
    setData(request, I18N_NOOP(p + "properties"), sourceInfo_properties);
    setData(request, I18N_NOOP(p + "latitude"), sourceInfo_geoLocation_lat.text().toFloat());
    setData(request, I18N_NOOP(p + "longitude"), sourceInfo_geoLocation_lon.text().toFloat());
    setData(request, I18N_NOOP(p + "timezone_name"), timeZone_default.attribute("zoneAbbreviation", "-1"));
    setData(request, I18N_NOOP(p + "timezone_offset"), timeZone_default.attribute("zoneOffset", "00:00"));
    setData(request, I18N_NOOP(p + "timezone_daylight"), (sourceInfo_timeZone.attribute("siteUsesDaylightSavingsTime", "false") == "false" ? false : true));
    setData(request, I18N_NOOP(p + "timezone_daylight_name"), timeZone_daylight.attribute("zoneAbbreviation", "-1"));
    setData(request, I18N_NOOP(p + "timezone_daylight_offset"), timeZone_daylight.attribute("zoneOffset", "00:00"));
    return p;
}

/**
   Extract the variable info from the supplied QDomElement (ns1:timeSeries)
*/
QString WaterIVEngine::extractSeriesVariable( QString &request, QString &prefix, QDomElement *timeSeries )
{
    QDomElement variable = timeSeries->elementsByTagName("ns1:variable").at(0).toElement();
    QDomElement variable_code = variable.elementsByTagName("ns1:variableCode").at(0).toElement();
    QDomElement variable_name = variable.elementsByTagName("ns1:variableName").at(0).toElement();
    QDomElement variable_desc = variable.elementsByTagName("ns1:variableDescription").at(0).toElement();
    QDomElement variable_type = variable.elementsByTagName("ns1:valueType").at(0).toElement();
    QDomElement variable_nodata = variable.elementsByTagName("ns1:noDataValue").at(0).toElement();
    QDomElement variable_options = variable.elementsByTagName("ns1:options").at(0).toElement();
    QDomElement variable_unit = variable.elementsByTagName("ns1:unit").at(0).toElement();
    QDomElement variable_unit_code = variable_unit.elementsByTagName("ns1:unitCode").at(0).toElement();

    QDomNamedNodeMap variable_code_attributes = variable_code.attributes();
    QMap<QString, QVariant> variable_code_attribs;
    int num_attributes = variable_code_attributes.length();
    for (int j=0; j<num_attributes; j++)
    {
        QDomAttr attribute = variable_code_attributes.item(j).toAttr();
        variable_code_attribs[attribute.name()] = attribute.value();
    }

    QString statName = "";
    QString statCode = "-1";
    QDomNodeList variable_optionList = variable_options.elementsByTagName("ns1:option");
    int num_options = variable_optionList.length();
    for (int j=0; j<num_options; j++)
    {
        QDomElement option = variable_optionList.at(j).toElement();
        if (option.attribute("name", "-1") == "Statistic")
        {
            statCode = option.attribute("optionCode", "-1");
            statName = option.text();
            break;
        }
    }

    QString p = prefix + variable_code.text() + "_";
    setData(request, I18N_NOOP(p + "name"), variable_name.text());
    setData(request, I18N_NOOP(p + "code"), variable_code.text());
    setData(request, I18N_NOOP(p + "code_attributes"), variable_code_attribs);
    setData(request, I18N_NOOP(p + "description"), variable_desc.text());
    setData(request, I18N_NOOP(p + "valuetype"), variable_type.text());
    setData(request, I18N_NOOP(p + "unitcode"), variable_unit_code.text());
    setData(request, I18N_NOOP(p + "nodatavalue"), variable_nodata.text().toDouble());

    p.append(statCode + "_");
    setData(request, I18N_NOOP(p + "name"), statName);
    setData(request, I18N_NOOP(p + "code"), statCode);
    return p;
}

/**
   Extract the values from the supplied QDomElement (ns1:timeSeries)
*/
void WaterIVEngine::extractSeriesValues( QString &request, QString &prefix, int &count, QDomElement *timeSeries )
{
    QDomNodeList valuesets = timeSeries->elementsByTagName("ns1:values");
    int num_sets = valuesets.length();

    for (int j=0; j<num_sets; j++)
    {
        QDomElement values = valuesets.at(j).toElement();

        QMap<QString, QVariant> qualifiersMap;  // get qualifiers
        QDomNodeList qualifiers = values.elementsByTagName("ns1:qualifier");
        int num_qualifiers = qualifiers.length();
        for (int i=0; i<num_qualifiers; i++)
        {
            QDomElement qualifier = qualifiers.at(i).toElement();
            QDomElement qualifier_code = qualifier.elementsByTagName("ns1:qualifierCode").at(0).toElement();
            QDomElement qualifier_desc = qualifier.elementsByTagName("ns1:qualifierDescription").at(0).toElement();
            QList<QVariant> list;
            list.insert(0, qualifier.attribute("qualifierID", "-1"));
            list.append(qualifier_code.text());
            list.append(qualifier_desc.text());
            list.append(qualifier.attribute("ns1:network", "-1"));
            list.append(qualifier.attribute("ns1:vocabulary", "-1"));

            qualifiersMap[qualifier_code.text()] = list;
        }
    
        // get collection method
        QDomElement method = values.elementsByTagName("ns1:method").at(0).toElement();
        QDomElement method_desc = method.elementsByTagName("ns1:methodDescription").at(0).toElement();

        QMap<QString, QVariant> valuesMap;    // get list of values
        QDomNodeList valueList = values.elementsByTagName("ns1:value");
        int num_values = valueList.length();
        for (int i=0; i<num_values; i++)
        {
            QDomElement value = valueList.at(i).toElement();
            QList<QVariant> list;
            list.insert(0, value.text());
            list.append(value.attribute("qualifiers", ""));
            QString dateTime = value.attribute("dateTime", "-1");
            if (dateTime != "-1")
            {
                valuesMap[dateTime] = list;
            }
        }
       
        QString p = prefix + method.attribute("methodID", "-1") + "_";
        setData(request, I18N_NOOP(p + "id"), method.attribute("methodID", "-1").toInt());
        setData(request, I18N_NOOP(p + "description"), method_desc.text());
        setData(request, I18N_NOOP(p + "all"), valuesMap);
        setData(request, I18N_NOOP(p + "qualifiers"), qualifiersMap);

        QMapIterator<QString, QVariant> i(valuesMap);
        while (i.hasNext())   // get the most recent value
        {
            i.next();
            QVariant date = QDateTime::fromString(i.key(), Qt::ISODate);
            QList<QVariant> recentValue = i.value().toList();

            setData(request, I18N_NOOP(p + "recent_value"), recentValue.at(0).toDouble());
            setData(request, I18N_NOOP(p + "recent_date"), date);
            setData(request, I18N_NOOP(p + "recent_qualifier"), recentValue.at(1));
            break;   // we only want the first item
        }

        QStringList components = prefix.split("_");  // table of contents
        QMap<QString, QVariant> tocEntry = QMap<QString, QVariant>();
        tocEntry.insert("site", components.at(1));
        tocEntry.insert("variable", components.at(2));
        tocEntry.insert("statistic", components.at(3));
        tocEntry.insert("method", method.attribute("methodID", "-1"));
	setData(request, I18N_NOOP(PREFIX_TOC + QString::number(count)), tocEntry);
        count++;
    }
}

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(wateriv, WaterIVEngine)
 
#include "waterivengine.moc"
