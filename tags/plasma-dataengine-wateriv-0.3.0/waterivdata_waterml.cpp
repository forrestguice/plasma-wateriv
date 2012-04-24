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

#include "waterivdata_waterml.h"

const QString WaterIVDataWaterML::FORMAT = "waterml,1.0";

/**
   @return the format string to be passed to the web service
*/
QString WaterIVDataWaterML::getFormat()
{
    return FORMAT;
}

/**
   Parses the passed QByteArray data as xml using QDomDocument.
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param &bytes QByteArray of raw data
*/
void WaterIVDataWaterML::extractData( WaterIVEngine *engine, const QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "schema"), document.attribute("xsi:schemaLocation", "-1"));
        if (document.tagName() == "ns1:timeSeriesResponse")
        {
            // appears valid waterml - extract data
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "isvalid"), true);
            extractQueryInfo(engine, request, &document);
            extractTimeSeries(engine, request, &document);

        } else {
            // invalid waterml - return error data
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "isvalid"), false);
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_msg"), "XML Error: not WaterML (parent element is not timeSeriesResponse).");
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_line"), -1);
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_column"), -1);
            engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_TOC + "count"), 0);
        }

    } else {
        // invalid xml - return error data
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "isvalid"), false);
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_msg"), errorMsg);
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_line"), errorLine);
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_XML + "error_column"), errorColumn);
        engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_TOC + "count"), 0);
    }
}
/**
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param *document a pointer to a QDomElement containing a waterml timeSeriesResponse
*/
void WaterIVDataWaterML::extractQueryInfo( WaterIVEngine *engine, const QString &request, QDomElement *document )
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
    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_QUERYINFO + "url"), QUrl(queryInfo_url.text()));
    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_QUERYINFO + "time"), queryDateTime);
    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_QUERYINFO + "notes"), queryInfo_notes);
    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_QUERYINFO + "location"), queryInfo_location.text());
    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_QUERYINFO + "variable"), queryInfo_variable.text());
}

/**
   Extract the timeseries (ns1:timeSeries) from the supplied QDomElement.
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param *document a pointer to a QDomElement containing a waterml timeSeriesResponse
*/
void WaterIVDataWaterML::extractTimeSeries( WaterIVEngine *engine, const QString &request, QDomElement *document )
{
    int count = 0;        // gather time series
    QDomNodeList timeSeriesList = document->elementsByTagName("ns1:timeSeries");
    int num_series = timeSeriesList.length();
    for (int i=0; i<num_series; i++)
    {
        QDomElement timeSeries = timeSeriesList.at(i).toElement();
        QString timeSeries_name = timeSeries.attribute("name", "-1");

        QString prefix = WaterIVEngine::PREFIX_SERIES;
        prefix = extractSeriesSourceInfo(engine, request, prefix, &timeSeries);
        prefix = extractSeriesVariable(engine, request, prefix, &timeSeries);
        extractSeriesValues(engine, request, prefix, count, &timeSeries);
    }

    engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_TOC + "count"), count);
}

/**
   Extract the sourceInfo from the supplied QDomElement (ns1:timeSeries)
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param &prefix the key prefix to prepend to any data set
   @param *timeSeries a pointer to a QDomElement containing a waterml timeseries
*/
QString WaterIVDataWaterML::extractSeriesSourceInfo( WaterIVEngine *engine, const QString &request, QString &prefix, QDomElement *timeSeries )
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
    engine->setEngineData(request, I18N_NOOP(p + "name"), sourceInfo_siteName.text());
    engine->setEngineData(request, I18N_NOOP(p + "code"), sourceInfo_siteCode.text());
    engine->setEngineData(request, I18N_NOOP(p + "properties"), sourceInfo_properties);
    engine->setEngineData(request, I18N_NOOP(p + "latitude"), sourceInfo_geoLocation_lat.text());
    engine->setEngineData(request, I18N_NOOP(p + "longitude"), sourceInfo_geoLocation_lon.text());
    engine->setEngineData(request, I18N_NOOP(p + "timezone_name"), timeZone_default.attribute("zoneAbbreviation", "-1"));
    engine->setEngineData(request, I18N_NOOP(p + "timezone_offset"), timeZone_default.attribute("zoneOffset", "00:00"));
    engine->setEngineData(request, I18N_NOOP(p + "timezone_daylight"), (sourceInfo_timeZone.attribute("siteUsesDaylightSavingsTime", "false") == "false" ? false : true));
    engine->setEngineData(request, I18N_NOOP(p + "timezone_daylight_name"), timeZone_daylight.attribute("zoneAbbreviation", "-1"));
    engine->setEngineData(request, I18N_NOOP(p + "timezone_daylight_offset"), timeZone_daylight.attribute("zoneOffset", "00:00"));
    return p;
}

/**
   Extract the variable info from the supplied QDomElement (ns1:timeSeries)
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param &prefix the key prefix to prepend to any data set
   @param *timeSeries a pointer to a QDomElement containing a waterml timeseries
*/
QString WaterIVDataWaterML::extractSeriesVariable( WaterIVEngine *engine, const QString &request, QString &prefix, QDomElement *timeSeries )
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
    engine->setEngineData(request, I18N_NOOP(p + "name"), variable_name.text());
    engine->setEngineData(request, I18N_NOOP(p + "code"), variable_code.text());
    engine->setEngineData(request, I18N_NOOP(p + "code_attributes"), variable_code_attribs);
    engine->setEngineData(request, I18N_NOOP(p + "description"), variable_desc.text());
    engine->setEngineData(request, I18N_NOOP(p + "valuetype"), variable_type.text());
    engine->setEngineData(request, I18N_NOOP(p + "unitcode"), variable_unit_code.text());
    engine->setEngineData(request, I18N_NOOP(p + "nodatavalue"), variable_nodata.text().toDouble());

    p.append(statCode + "_");
    engine->setEngineData(request, I18N_NOOP(p + "name"), statName);
    engine->setEngineData(request, I18N_NOOP(p + "code"), statCode);
    return p;
}

/**
   Extract the values from the supplied QDomElement (ns1:timeSeries)
   @param *engine pointer to the data engine
   @param &request name of the source (to set data on the engine)
   @param &prefix the key prefix to prepend to any data set
   @param &count a counter variable incremented each call (creates toc)
   @param *timeSeries a pointer to a QDomElement containing a waterml timeseries
*/
void WaterIVDataWaterML::extractSeriesValues( WaterIVEngine *engine, const QString &request, QString &prefix, int &count, QDomElement *timeSeries )
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
        engine->setEngineData(request, I18N_NOOP(p + "id"), method.attribute("methodID", "-1").toInt());
        engine->setEngineData(request, I18N_NOOP(p + "description"), method_desc.text());
        engine->setEngineData(request, I18N_NOOP(p + "all"), valuesMap);
        engine->setEngineData(request, I18N_NOOP(p + "qualifiers"), qualifiersMap);

        int c = 0;                  // gather recent values and dates
        double recentValues[2];
        QMapIterator<QString, QVariant> i(valuesMap);
        while (i.hasNext() && c < 2)
        {
            i.next();

            QVariant date = QDateTime::fromString(i.key(), Qt::ISODate);
            QList<QVariant> recentValue = i.value().toList();
            recentValues[c] = recentValue.at(0).toDouble();

            if (c == 0)             // set most recent value
            {
                engine->setEngineData(request, I18N_NOOP(p + "recent_value"), recentValues[0]);
                engine->setEngineData(request, I18N_NOOP(p + "recent_date"), date);

                QList<QVariant> recentQualifiers;
                QStringList rQualifiers = recentValue.at(1).toString().split(" ");
                foreach( QString q, rQualifiers )
                {
                    recentQualifiers << q;
                }
                engine->setEngineData(request, I18N_NOOP(p + "recent_qualifier"), recentQualifiers);
            }
            c++;
        }

        double vi = (c > 0) ? recentValues[c-1] : 1;
        double vf = (c > 0) ? recentValues[0] : 1;
        double recentChange = 100 * (vf - vi) / vi;
        engine->setEngineData(request, I18N_NOOP(p + "recent_change"), recentChange);

        // write table of contents entry
        QStringList components = prefix.split("_");
        QMap<QString, QVariant> tocEntry = QMap<QString, QVariant>();
        tocEntry.insert("site", components.at(1));
        tocEntry.insert("variable", components.at(2));
        tocEntry.insert("statistic", components.at(3));
        tocEntry.insert("method", method.attribute("methodID", "-1"));
	engine->setEngineData(request, I18N_NOOP(WaterIVEngine::PREFIX_TOC + QString::number(count)), tocEntry);
        count++;
    }
}

