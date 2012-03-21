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

#include "waterivengine.h"
#include <QtNetwork>
#include <QDomDocument>
 
//#include <Plasma/DataContainer>
//#include <QDate>
//#include <QTime>
//#include <KSystemTimeZones>
//#include <KDateTime>

const QString WaterIVEngine::DEFAULT_SERVER = "http://waterservices.usgs.gov/nwis/iv";
/**
   This dataengine retrieves timeseries data from the USGS Instantaneous Values
   Web Service using REST. The web service returns the data as an XML 
   document using a set of tags called "WaterML". The dataengine reads these
   tags (DOM) and makes the data available to plasmoids using a simple
   naming scheme for available keys.

   ----------------------
   The source name can be:
   ----------------------
   1) a site code, or comma separated list of site codes (up to 100)
      (see http://wdr.water.usgs.gov/nwisgmap/index.html)

   2) a request string specifying the data to retrieve (the part after 
      the ? in a complete request url)
      (see http://waterservices.usgs.gov/rest/IV-Test-Tool.html)

   3) a complete request url specifying the data to retrieve 
      (see http://waterservices.usgs.gov/rest/IV-Test-Tool.html)

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
const QString WaterIVEngine::PREFIX_TIMESERIES = "timeseries_";
const QString WaterIVEngine::PREFIX_SOURCEINFO = "sourceinfo_";
const QString WaterIVEngine::PREFIX_VARIABLE = "variable_";
const QString WaterIVEngine::PREFIX_VALUES = "values_";
/**
  ---------------------------------------
   Naming scheme for available keys:
  ---------------------------------------

   --> PREFIX_NET + <NET_KEY>
       The NET keys contain info about the network request for data.
       The error key is available when isvalid is false. All other keys are
       available when isvalid is true.
       Examples: net_url net_request net_error

       Available NET_KEYs:
       --> url           : the base url of the web service
       --> request       : the request made to the web service
       --> isvalid       : the result of the network operation (t/f)
       --> error         : failed to retrieve data (network error)

  ---------------------------------------

   --> PREFIX_XML + <XML_KEY>
       The XML keys contain info about the raw xml returned by the request.
       The error_ keys are available when isvalid is false. All other keys are 
       available when isvalid is true.
       Examples: xml_isvalid xml_error_msg

       Available XML_KEYs:
       --> isvalid       : the request returned valid xml (parsed by DOM)
       --> schema        : the xsi:schemaLocation (should be *WaterML-1.1.xsd)
       --> error_msg     : the error msg (when !isvalid)
       --> error_line    : the line the error occured on
       --> error_column  : the column the error occured on

  ---------------------------------------

   --> PREFIX_QUERYINFO + <QUERYINFO_KEY>
       The queryInfo contains info about the request made to the web service.
       Example: queryinfo_url

       Available QUERYINFO_KEYs:
       --> url        : the url of the web service
       --> time       : the time the query was processed
       --> notes      : a QMap<String, QVariant> of query notes (by title)
       --> location   : the requested location (site & other codes)
       --> variable   : the requested variables (param)

  ---------------------------------------

   --> PREFIX_TIMESERIES_COUNT
       The number of available timeseries. The info for each individual time
       series is found under PREFIX_TIMESERIES_<#>
       Example: timeseries_count

  ---------------------------------------

   --> PREFIX_TIMESERIES + <#>_ + PREFIX_SOURCEINFO + <SOURCEINFO_KEY>
       The sourceInfo contains info about the data source of a timeseries.
       Example: timeseries_0_sourceinfo_sitename
       
       Available SOURCEINFO_KEYs:
       --> sitename    : the name of the source (physical site name)
       --> sitecode    : the agency code of the source (e.g. usgs)
       --> properties  : a QMap<String, QVariant> of site properties (by name)
       --> latitude    : the latitude of the source
       --> longitude   : the longitude of the source
       --> timezone_name             : the timezone abreviation of the source
       --> timezone_offset           : the timezone offset
       --> timezone_daylight         : currently using daylight savings (t/f)
       --> timezone_daylight_name    : the daylight savings time name (e.g. EDT)
       --> timezone_daylight_offset  : the daylight savings time offset

  ---------------------------------------

   --> PREFIX_TIMESERIES + <#>_ + PREFIX_VARIABLE + <VARIABLE_KEY>
       The variable contains info about the dependent variable in the timeseries.
       Example: timeseries_0_variable_code

       Available VARIABLE_KEYs:
       --> code            : the variable code (e.g. 00060)
       --> code_attributes : a QMap<QString, QVariant> of code attributes (by name)
       --> name            : the name of the variable
       --> description     : a description of the variable
       --> valuetype       : the type of value the variable describes (e.g. Derived)
       --> unitcode        : the unit code (e.g. ft, cfs, etc)
       --> options
       --> nodatavalue     : the value to assign if a value has no data

  ---------------------------------------

   --> PREFIX_TIMESERIES + <#>_ + PREFIX_VALUES + <VALUES_KEY>
       The values contains info about individual data points in the timeseries.
       Example: timeseries_0_values_all

       Available VALUES_KEYs:
       --> all        : a QMap<QString, QList<QVariant>> containing all values.
                      : the map key is the datetime string of the value.
                      : each QList contains two values:
                      : [0] the value (e.g. 200)
                      : [1] value qualifiers by code (e.g. "P" is provisional)

       --> recent           : the most recent value (e.g. 200)
       --> recent_date      : the datetime of the recent value
       --> recent_qualifier : the qualifier of the most recent value (e.g. "P")

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
    setMinimumPollingInterval(WaterIVEngine::DEFAULT_MIN_POLLING * 60 * 1000);

    replies = new QMap<QNetworkReply*, QString>();

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dataFetchComplete(QNetworkReply*)));
}

/**
   This function runs when a source is requested for the first time.
*/
bool WaterIVEngine::sourceRequestEvent(const QString &sourcename)
{
    setData(sourcename, DataEngine::Data());
    return updateSourceEvent(sourcename);
}
 
/**
   This function runs when a source requests an update; it initiates a fetch
   of the requested data. Execution continues in dataFetchComplete when the 
   download is complete. Invalid source names are silently ignored.
*/
bool WaterIVEngine::updateSourceEvent(const QString &source)
{
    QString request;
    if (source.contains("?"))
    {
       // full url supplied: pass it as the request
       if (not hasMajorFilter(source)) return false;
       request = QString(source);

    } else {
        // no url supplied: form the request
        request.append(DEFAULT_SERVER + "?format=waterml,1.1");

        if (source.contains("&"))
        {
            // list of arguments
            if (not hasMajorFilter(source)) return false;
            else request.append("&" + source);
        
        } else {
            if (source.contains("="))
            {
                // single argument: the source is a major filter
                if (not hasMajorFilter(source)) return false;
                else request.append("&" + source);
            
            } else {
                // no arguments: the source is the site code (or list of codes)
                if (not isSiteCode(source)) return false;
                request.append("&sites=" + source);
            }
        }
    }

    // get the requested source (change to data occurs later in dataFetchComplete)
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(request)));
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

    QByteArray bytes = reply->readAll();
    reply->deleteLater();

    setData(request, I18N_NOOP(PREFIX_NET + "isvalid"), true);
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
   Extract the queryInfo (ns1:queryInfo) from the supplied QDomElement 
   (ns1:timeSeriesResponse).
*/
void WaterIVEngine::extractQueryInfo( QString &request, QDomElement *document )
{
    QDomElement queryInfo = document->elementsByTagName("ns1:queryInfo").at(0).toElement();
    QDomElement queryInfo_url = queryInfo.elementsByTagName("ns2:queryURL").at(0).toElement();
    QDomElement queryInfo_time = queryInfo.elementsByTagName("ns2:creationTime").at(0).toElement();
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

    // set the query data
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "url"), QUrl(queryInfo_url.text()));
    setData(request, I18N_NOOP(PREFIX_QUERYINFO + "time"), queryInfo_time.text());  // TODO: convert to QTime
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

/**
   Extract the sourceInfo from the supplied QDomElement (ns1:timeSeries)
*/
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

/**
   Extract the variable info from the supplied QDomElement (ns1:timeSeries)
*/
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

    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "code"), variable_code.text().toInt());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "code_attributes"), variable_code_attribs);
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "name"), variable_name.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "description"), variable_desc.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "valuetype"), variable_type.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "unitcode"), variable_unit_code.text());
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "options"), variable_option);
    setData(request, I18N_NOOP(prefix + PREFIX_VARIABLE + "nodatavalue"), variable_nodata.text().toDouble());
}

/**
   Extract the values from the supplied QDomElement (ns1:timeSeries)
*/
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
            valuesMap[value.attribute("dateTime", "-1")] = list;
        }
    }

    foreach(QVariant vlist, valuesMap)  // iterate over values by order of key
    {
        QList<QVariant> value = vlist.toList();
        setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent"), value.at(0).toDouble());
        // todo: fix loop to include key - pass keys recent_date
        //setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent_date"), value.at(0).toDouble());
        setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "recent_qualifier"), value.at(1));
        break;   // we only want the first item
    }

    setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "all"), valuesMap);
    setData(request, I18N_NOOP(prefix + PREFIX_VALUES + "qualifiers"), qualifiersMap);
}

/**
   Returns true if the supplied request has one of the required major filters.
   Returns false if the major filter is missing or if there is more than one 
   major filter.
*/
bool WaterIVEngine::hasMajorFilter( const QString &source )
{
    bool retValue = false;

    QString request = source;
    if (source.contains("?"))
    {
        request = source.split("?").at(1);
    }

    int c = 0;
    QStringList args = request.split("&");
    int num_args = args.length();
    for (int i=0; i<num_args; i++)
    {
        QStringList pair = args.at(i).split("=");
        QString key = pair.at(0).toLower();
        QString value = pair.at(1);
 
        if (key == "sites")
        {
            retValue = WaterIVEngine::isSiteCode(value);
            c++;

        } else if (key == "statecd") {
            retValue = WaterIVEngine::isStateCode(value);
            c++;

        } else if (key == "huc") {
            retValue = WaterIVEngine::isHucCode(value);
            c++;

        } else if (key == "bbox") {
            retValue = WaterIVEngine::isBBoxCode(value);
            c++;

        } else if (key == "countycd") {
            retValue = WaterIVEngine::isCountyCode(value);
            c++;
        }
        if (retValue == false) break;
    }

    if (c != 1)
    {
        qDebug() << "hasMajorFilter: invalid filters: too many major filters";
        return false;
    }

    return retValue;
}

/**
   Return true if the supplied request is in the format of a state code.
   Only one state code is allowed.
   example: az
*/
bool WaterIVEngine::isStateCode( const QString &request )
{
    return (request.length() == 2);
}

/**
   Return true if the request is in the form of a bBox (bounding box)
*/
bool WaterIVEngine::isBBoxCode( const QString &request )
{
    qDebug() << "Unsupported major filter: bBox: " + request;
    return false;  // todo: support bbox as major filter
}

/**
   Return true if the supplied request is in the format of a HUC code or a list
   of HUC codes seperated by comma. Major huc codes are 2 digits, minor huc
   codes are 8 digits. The max number of codes is 10; the list may contain only
   one major code.
*/
bool WaterIVEngine::isHucCode( const QString &request )
{
    int c = 0, major_huc = 0;

    QStringList codes = request.split(",");
    int num_codes = codes.length();
    for (int i=0; i<num_codes; i++)
    {
        QString code = codes.at(i);

        bool is_int = true;
        code.toInt(&is_int);
        if (not is_int) return false;

        if (code.length() == 2)            // a major huc code (one allowed)
        {
            major_huc++;
            c++;

        } else if (code.length() == 8) {   // a minor huc code
            c++;

        } else {                           // invalid length
            return false;
        }
    }

    if (c > 10 || c < 1 || major_huc > 1) return false;
    else return true;
}

/**
   Return true if the supplied request is in the format of a county code
   or a list of county codes separated by a comma. County codes are numeric
   and 5 digits in length. The maximum number of county codes is 20.
*/
bool WaterIVEngine::isCountyCode( const QString &request )
{
    QStringList codes = request.split(",");
    int num_codes = codes.length();
    for (int i=0; i<num_codes; i++)
    {
       QString code = codes.at(i);

       bool is_int = true;
       code.toInt(&is_int);
       if (not is_int) return false;
    
       if (code.length() != 5) return false;
    }

    if (num_codes > 20 || num_codes < 1) return false;
    else return true;
}

/**
   Returns true if the supplied request is in the format of a single site code
   or list of sites separated by a comma (with optional agency prefix). Lists 
   may contain up to 100 codes.
   Example: USGS:01646500,06306300
*/
bool WaterIVEngine::isSiteCode( const QString &request )
{
    QStringList codes = request.split(",");
    int num_codes = codes.length();
    for (int i=0; i<num_codes; i++)
    {
        QString site = codes.at(i);
        QString agency, code;
        if (site.contains(":"))   // has optional agency prefix
        {
            QStringList pair = request.split(":");
            agency = pair.at(0);
            code = pair.at(1);

        } else {
            agency = "";
            code = site;
        }

        bool is_int = true;
        code.toInt(&is_int);
        if (not is_int) return false;
    }

    if (num_codes > 100 || num_codes < 1) return false;
    else return true;
}
 
//QTemporaryFile temp_file;
//temp_file.write(reply->readAll());
//if (request == I18N_NOOP("Local"))

// the first argument must match X-Plasma-EngineName in the .desktop file
// the second argument is the class that derives from Plasma::DataEngine
K_EXPORT_PLASMA_DATAENGINE(wateriv, WaterIVEngine)
 
// needed since WaterIVEngine is a QObject
//#include "waterivengine.moc"
