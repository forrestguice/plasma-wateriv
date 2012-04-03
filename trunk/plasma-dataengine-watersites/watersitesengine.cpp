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
const QString WaterSitesEngine::DEFAULT_FORMAT = "format=mapper,1.0";

const QString WaterSitesEngine::PREFIX_NET = "net_";
const QString WaterSitesEngine::PREFIX_XML = "xml_";

/**
   DOCS HERE
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
    setData(source, DataEngine::Data());   // bug: without this line plasmoids don't get initial update
    return updateSourceEvent(source);      //      but with it plasmaengineexplorer fails to update
}
 
/**
   This function runs when a source requests an update; it initiates a fetch
   of the requested data. Execution continues afterward in dataFetchComplete.
*/
bool WaterSitesEngine::updateSourceEvent(const QString &source)
{
    QString errorMsg;
    QString requestUrl = SitesRequest::requestForSource(source, errorMsg);
    if (requestUrl == "-1")
    {
        qDebug() << errorMsg;
        return false;
    }

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
        //setData(request, I18N_NOOP(PREFIX_TIMESERIES + "count"), 0);
        reply->deleteLater();
        return;
    }

    setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), true);
    QByteArray bytes = reply->readAll();
    reply->deleteLater();

    extractData(request, bytes);
}

void WaterSitesEngine::extractData( QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        //setData(request, I18N_NOOP(PREFIX_XML + "schema"), document.attribute("xsi:schemaLocation", "-1"));
        //if (document.tagName() == "ns1:timeSeriesResponse")
        //{
            // appears valid waterml - extract data
            setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), true);
        //    extractQueryInfo(request, &document);
        //    extractTimeSeries(request, &document);

        //} else {
        //    // invalid waterml - return error data
        //    setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
        //    setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), "XML Error: not WaterML (parent element is not timeSeriesResponse).");
        //    setData(request, I18N_NOOP(PREFIX_XML + "error_line"), -1);
        //    setData(request, I18N_NOOP(PREFIX_XML + "error_column"), -1);
        //    setData(request, I18N_NOOP(PREFIX_TIMESERIES + "count"), 0);
        //}

    } else {
        // invalid xml - return error data
        setData(request, I18N_NOOP(PREFIX_XML + "isvalid"), false);
        setData(request, I18N_NOOP(PREFIX_XML + "error_msg"), errorMsg);
        setData(request, I18N_NOOP(PREFIX_XML + "error_line"), errorLine);
        setData(request, I18N_NOOP(PREFIX_XML + "error_column"), errorColumn);
        //setData(request, I18N_NOOP(PREFIX_TIMESERIES + "count"), 0);
    }
}

/**void WaterIVEngine::extractQueryInfo( QString &request, QDomElement *document )
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

void WaterIVEngine::extractTimeSeries( QString &request, QDomElement *document )
{
    // gather time series
    QDomNodeList timeSeriesList = document->elementsByTagName("ns1:timeSeries");
    int num_series = timeSeriesList.length();
    setData(request, I18N_NOOP(PREFIX_TIMESERIES + "count"), num_series);

    for (int i=0; i<num_series; i++)
    {
        QDomElement timeSeries = timeSeriesList.at(i).toElement();
        QString timeSeries_name = timeSeries.attribute("name", "-1");

        QString prefix = PREFIX_TIMESERIES + QString::number(i) + "_";
        extractSeriesSourceInfo(request, prefix, &timeSeries);
        extractSeriesVariable(request, prefix, &timeSeries);
        extractSeriesValues(request, prefix, &timeSeries);
    }
}

void WaterIVEngine::extractSeriesSourceInfo( QString &request, QString &prefix, QDomElement *timeSeries )
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
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "sitename"), sourceInfo_siteName.text());
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "sitecode"), sourceInfo_siteCode.text());
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "properties"), sourceInfo_properties);
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "latitude"), sourceInfo_geoLocation_lat.text().toDouble());
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "longitude"), sourceInfo_geoLocation_lat.text().toDouble());
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "timezone_name"), timeZone_default.attribute("zoneAbbreviation", "-1"));
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "timezone_offset"), timeZone_default.attribute("zoneOffset", "00:00"));
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "timezone_daylight"), (sourceInfo_timeZone.attribute("siteUsesDaylightSavingsTime", "false") == "false" ? false : true));
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "timezone_daylight_name"), timeZone_daylight.attribute("zoneAbbreviation", "-1"));
    setData(request, I18N_NOOP(prefix + PREFIX_SOURCEINFO + "timezone_daylight_offset"), timeZone_daylight.attribute("zoneOffset", "00:00"));
}

void WaterIVEngine::extractSeriesVariable( QString &request, QString &prefix, QDomElement *timeSeries )
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

    QMap<QString, QVariant> variable_option;
    QDomNodeList variable_optionList = variable_options.elementsByTagName("ns1:option");
    int num_options = variable_optionList.length();
    for (int j=0; j<num_options; j++)
    {
        QDomElement option = variable_optionList.at(j).toElement();
        variable_option[option.attribute("name", "-1")] = option.attribute("optionCode", "-1");
    }

    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "code"), variable_code.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "code_attributes"), variable_code_attribs);
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "name"), variable_name.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "description"), variable_desc.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "valuetype"), variable_type.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "unitcode"), variable_unit_code.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "options"), variable_option);
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "nodatavalue"), variable_nodata.text().toDouble());
}

void WaterIVEngine::extractSeriesValues( QString &request, QString &prefix, QDomElement *timeSeries )
{
    QDomElement values = timeSeries->elementsByTagName("ns1:values").at(0).toElement();

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

    QMapIterator<QString, QVariant> i(valuesMap);
    while (i.hasNext())
    {
        i.next();
        QVariant date = QDateTime::fromString(i.key(), Qt::ISODate);
        QList<QVariant> recentValue = i.value().toList();

        setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent"), recentValue.at(0).toDouble());
        setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent_date"), date);
        setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent_qualifier"), recentValue.at(1));

        break;   // we only want the first item
    }

    setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "all"), valuesMap);
    setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "qualifiers"), qualifiersMap);
}
*/

//QTemporaryFile temp_file; temp_file.write(reply->readAll());

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(watersites, WaterSitesEngine)
 
// needed since WaterSitesEngine is a QObject // todo: whats going on here...
//#include "watersitesengine.moc"
