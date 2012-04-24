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

#include "watersitesdata_statecodes.h"

const QString WaterSitesDataStateCodes::FORMAT = "statecodes,1.0";

/**
*/
QString WaterSitesDataStateCodes::getFormat()
{
    return FORMAT;
}

/**
*/
void WaterSitesDataStateCodes::extractData( WaterSitesEngine *engine, const QString &request, QByteArray &bytes )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (doc.setContent(QString(bytes), &errorMsg, &errorLine, &errorColumn))
    {
        // valid xml - extract and return data
        QDomElement document = doc.documentElement();
        if (document.tagName() == "statecodes")
        {
            // appears valid - extract data
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), true);
            extractCodes(engine, request, &document);

        } else {
            // invalid - return error data
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), false);
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_msg"), "XML Error: format is not statecodes.");
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

void WaterSitesDataStateCodes::extractCodes( WaterSitesEngine *engine, const QString &request, QDomElement *document )
{
    int codeCount = 0;
    QHash<QString,QVariant> codeHash;

    QDomElement codes = document->toElement();
    QDomNodeList codeList = codes.elementsByTagName("code");

    int num_codes = codeList.length();
    for (int j=0; j<num_codes; j++)
    {
        QDomElement code = codeList.at(j).toElement();

        codeHash = QHash<QString,QVariant>();
        codeHash["name"] = code.attribute("name", "-1");
        codeHash["alpha"] = code.attribute("alpha", "-1");
        codeHash["numeric"] = code.attribute("numeric", "-1");

        engine->setEngineData(request, WaterSitesEngine::PREFIX_STATECODE + codeHash["alpha"].toString().toLower(), codeHash);
        codeCount += 1;
    }

    engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_STATECODE + "count"), codeCount);
}
 
//#include "watersitesdata_mapper.moc"
