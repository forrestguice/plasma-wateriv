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

#include "watersitesdata_sitetypes.h"

const QString WaterSitesDataSiteTypes::FORMAT = "sitetypes,1.0";

/**
*/
QString WaterSitesDataSiteTypes::getFormat()
{
    return FORMAT;
}

/**
*/
void WaterSitesDataSiteTypes::extractData( WaterSitesEngine *engine, const QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        if (document.tagName() == "sitetypes")
        {
            // appears valid - extract data
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), true);
            extractCodes(engine, request, &document);

        } else {
            // invalid - return error data
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), false);
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_msg"), "XML Error: format is not sitetypes.");
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_line"), -1);
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_column"), -1);
        }

    } else {
        // invalid xml - return error data
        engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), false);
        engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_msg"), errorMsg);
        engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_line"), errorLine);
        engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_column"), errorColumn);
    }
}

void WaterSitesDataSiteTypes::extractCodes( WaterSitesEngine *engine, const QString &request, QDomElement *document )
{
    QStringList filter;
    filter << "all";
    if (request.contains("?"))
    {
        QString options = request.split("?").at(1);
        if (options != "")
        {
            filter.clear();
            filter = options.split("&");
        }
    }
    int num_filters = filter.length();

    int codeCount = 0;
    QHash<QString,QVariant> codeHash;

    QDomElement codes = document->toElement();
    QDomNodeList codeList = codes.elementsByTagName("sitetype");

    int num_codes = codeList.length();
    for (int j=0; j<num_codes; j++)
    {
        QDomElement code = codeList.at(j).toElement();
        QString codeString = code.attribute("code", "-1");
        QString majorCode = codeString.mid(0,2).toLower();

        bool matchesFilter = false;
        for (int i=0; i<num_filters; i++)
        {
            QString f = filter.at(i).toLower(); 
            if (f == majorCode || f == "all")
            {
                matchesFilter = true;
                break;
            }
            if (f == "major" && codeString.length() == 2)
            {
                matchesFilter = true;
                break;
            }
        }

        if (matchesFilter)
        {
            codeHash = QHash<QString,QVariant>();
            codeHash["name"] = code.attribute("name", "-1");
            codeHash["code"] = code.attribute("code", "-1");

            engine->setEngineData(request, WaterSitesEngine::PREFIX_SITETYPE + codeHash["code"].toString().toLower(), codeHash);
            codeCount += 1;
        }
    }

    engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_SITETYPE + "count"), codeCount);
}
 
//#include "watersitesdata_agencycodes.moc"
