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

#include "watersitesdata_mapper.h"

const QString WaterSitesDataMapper::FORMAT = "mapper,1.0";

/**
*/
QString WaterSitesDataMapper::getFormat()
{
    return FORMAT;
}

/**
*/
void WaterSitesDataMapper::extractData( WaterSitesEngine *engine, const QString &request, QByteArray &bytes )
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
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), true);
            extractSites(engine, request, &document);

        } else {
            // invalid - return error data
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "isvalid"), false);
            engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_XML + "error_msg"), "XML Error: format is not mapper.");
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

/**
*/
void WaterSitesDataMapper::extractSites( WaterSitesEngine *engine, const QString &request, QDomElement *document )
{
    int siteCount = 0;
    QHash<QString,QVariant> siteHash;

    QDomNodeList sitesList = document->elementsByTagName("sites");
    int num_sitescomponents = sitesList.length();
    for (int i=0; i<num_sitescomponents; i++)
    {
        QDomElement sites = sitesList.at(i).toElement();
        QDomNodeList siteList = sites.elementsByTagName("site");

        int num_sites = siteList.length();
        for (int j=0; j<num_sites; j++)
        {
            QDomElement site = siteList.at(j).toElement();

            siteHash = QHash<QString,QVariant>();
            siteHash["code"] = site.attribute("sno", "-1");
            siteHash["name"] = site.attribute("sna", "-1");
            siteHash["agency"] = site.attribute("agc", "");
            siteHash["latitude"] = site.attribute("lat", "-1");
            siteHash["longitude"] = site.attribute("lng", "-1");
            siteHash["type"] = site.attribute("cat", "");

            engine->setEngineData(request, WaterSitesEngine::PREFIX_SITE + QString::number(siteCount) + "_" + siteHash["code"].toString(), siteHash);
            siteCount += 1;
        }
    }

    QDomNodeList cSiteList = document->elementsByTagName("colocated_sites");
    int num_csitesets = cSiteList.length();
    for (int i=0; i<num_csitesets; i++)
    {
        QDomElement csite = cSiteList.at(i).toElement();
        QDomNodeList siteList = csite.elementsByTagName("site");

        int num_sites = siteList.length();
        for (int j=0; j<num_sites; j++)
        {
            QDomElement site = siteList.at(j).toElement();

            siteHash = QHash<QString,QVariant>();
            siteHash["code"] = site.attribute("sno", "-1");
            siteHash["name"] = site.attribute("sna", "-1");
            siteHash["agency"] = site.attribute("agc", "");
            siteHash["latitude"] = site.attribute("lat", "-1");
            siteHash["longitude"] = site.attribute("lng", "-1");
            siteHash["cat"] = site.attribute("cat", "");

            engine->setEngineData(request, WaterSitesEngine::PREFIX_SITE + QString::number(siteCount) + "_" + siteHash["code"].toString(), siteHash);
            siteCount += 1;
        }
    }

    engine->setEngineData(request, I18N_NOOP(WaterSitesEngine::PREFIX_SITE + "count"), siteCount);
}
 
//#include "watersitesdata_mapper.moc"
