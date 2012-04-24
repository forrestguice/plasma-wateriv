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

/**
    SitesRequest
 
    This class contains functions for checking the validity of source names in
    an attempt to throw out bad requests before they are passed to the web
    service.
*/

#include "sitesrequest.h"
#include "watersitesengine.h"
#include "watersitesdata_mapper.h"
#include "watersitesdata_statecodes.h"
#include "watersitesdata_countycodes.h"
#include "watersitesdata_agencycodes.h"
#include "watersitesdata_sitetypes.h"
#include <kstandarddirs.h>

/**
*/
WaterSitesData* SitesRequest::formatForSource( const QString &source )
{
    QString lSource = source.toLower();
    if (source.contains("?"))
    {
        // check host value for special types
        QString host = lSource.split("?").at(0);
        if (host == "statecodes")
        {
            WaterSitesDataStateCodes *statecodesFormat = new WaterSitesDataStateCodes();
            return statecodesFormat;

        } else if (host == "countycodes") {
            WaterSitesDataCountyCodes *countycodesFormat = new WaterSitesDataCountyCodes();
            return countycodesFormat;

        } else if (host == "agencycodes") {
            WaterSitesDataAgencyCodes *agencycodesFormat = new WaterSitesDataAgencyCodes();
            return agencycodesFormat;

        } else if (host == "sitetypecodes") {
            WaterSitesDataSiteTypes *sitetypecodesFormat = new WaterSitesDataSiteTypes();
            return sitetypecodesFormat;
        }
    }

    // assume requests are for mapper data from sites service
    WaterSitesDataMapper *mapperFormat = new WaterSitesDataMapper();
    return mapperFormat;
}

/** 
    Takes a source name and returns a corresponding fully formed request url.
    Returns "-1" if source name cannot be transformed into a request url; the
    error message is placed into errorMsg;
*/

QString SitesRequest::requestForSource(const QString &source, QString &errorMsg, bool &dataIsRemote)
{
    QString request;
    bool hasFormatFilter = false;

    if (source.contains("?"))    // full url:
    {
       QString host = source.split("?").at(0);

       if (host == "statecodes")
       {
           //qDebug() << "special request: statecodes";
           QString path = KStandardDirs::locate("data", "plasma-dataengine-watersites/statecodes.xml");
           if (path == "") 
           {
               errorMsg.prepend("missing data file: statecodes.xml");
               path = "-1";
           }
           dataIsRemote = false;
           return path;

       } else if  (host == "countycodes") {
           //qDebug() << "special request: countycodes";
           QString path = KStandardDirs::locate("data", "plasma-dataengine-watersites/countycodes.xml");
           if (path == "")
           {
               errorMsg.prepend("missing data file: countycodes.xml");
               path = "-1";
           }
           dataIsRemote = false;
           return path;

       } else if  (host == "agencycodes") {
           //qDebug() << "special request: agencycodes";
           QString path = KStandardDirs::locate("data", "plasma-dataengine-watersites/agencycodes.xml");
           if (path == "")
           {
               errorMsg.prepend("missing data file: agencycodes.xml");
               path = "-1";
           }
           dataIsRemote = false;
           return path;

       } else if  (host == "sitetypecodes") {
           //qDebug() << "special request: sitetypecodes";
           QString path = KStandardDirs::locate("data", "plasma-dataengine-watersites/sitetypecodes.xml");
           if (path == "")
           {
               errorMsg.prepend("missing data file: sitetypecodes.xml");
               path = "-1";
           }
           dataIsRemote = false;
           return path;
       }

       if (not hasValidFilters(source, hasFormatFilter, errorMsg))
       {
           //errorMsg.prepend("watersites (url): " + source);
           return "-1";
       }
       request = QString(source); 

       // always specify the format
       if (!hasFormatFilter) request.append("&format=" + WaterSitesEngine::DEFAULT_FORMAT);

       // special case: missing host
       bool missingUrl = (host == "");
       if (missingUrl) request.prepend(WaterSitesEngine::DEFAULT_SERVER);

    } else {  // no url supplied: form the request
        request.append(WaterSitesEngine::DEFAULT_SERVER); 
        if (source.contains("&"))         // list of args:
        {
            if (not hasValidFilters(source, hasFormatFilter, errorMsg))
            {
                //errorMsg.prepend("watersites (args): " + source);
                return "-1";
            }
            if (hasFormatFilter) request.append("?");
            else request.append("?format=" + WaterSitesEngine::DEFAULT_FORMAT + "&");
            request.append(source);
        
        } else {
            if (source.contains("="))     // single arg: source is major filter
            {
                if (not hasValidFilters(source, hasFormatFilter, errorMsg))
                {
                    //errorMsg.prepend("watersites (arg): " + source);
                    return "-1";
                }
                request.append("?format=" + WaterSitesEngine::DEFAULT_FORMAT + "&" + source);
            
            } else {                      // no args: source is list of sites
                if (not isSiteCode(source, errorMsg))
                {
                    //errorMsg.prepend("watersites (site): " + source);
                    return "-1";
                }
                request.append("?format=" + WaterSitesEngine::DEFAULT_FORMAT + "&sites=" + source);
            }
        }
    }
    dataIsRemote = true;
    return request;
}

/**
   Returns true if the supplied request has valid filters (including one of the 
   required major filters). Returns false if the major filter is missing, 
   there is more than one major filter, there are unrecognized filters, or
   the value of one of the supplied major/minor filters has incorrect syntax.
   See http://waterservices.usgs.gov/rest/Site-Service.html for the spec
*/
bool SitesRequest::hasValidFilters( const QString &source, bool &hasFormatFilter, QString &errorMsg )
{
    bool retValue = false;
    hasFormatFilter = false;

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
        if (args.at(i) == "") continue;    // special case: started with &
        if (not args.at(i).contains("="))  // invalid case: not a key=value pair
        {
            errorMsg.prepend("syntax error: argument without an equal sign (=) :: " + args.at(i));
            return false;
        }

        QStringList pair = args.at(i).split("=");
        QString key = pair.at(0).toLower();
        QString value = pair.at(1);
 
        if (key == "format")
        {
            // minor filter: format
            hasFormatFilter = true;
            retValue = isFormatString(value, errorMsg);

        } else if (key == "sites" || key == "site" || key == "location") {
            // major filter: sites site location
            retValue = isSiteCode(value, errorMsg);
            c++;

        } else if (key == "statecd" || key == "statecds") {
            // major filter: statecd statecds
            retValue = isStateCode(value, errorMsg);
            c++;

        } else if (key == "huc" || key == "hucs") {
            // major filter: huc hucs
            retValue = isHucCode(value, errorMsg);
            c++;

        } else if (key == "bbox") {
            // major filter: bbox
            retValue = isBBoxCode(value, errorMsg);
            c++;

        } else if (key == "countycd" || key == "countycds") {
            // major filter: countycd countycds
            retValue = isCountyCode(value, errorMsg);
            c++;                     

        } else if (key == "sitetype" || key == "sitetypes" || 
                   key == "sitetypecd" || key == "sitetypecds") { 
            // minor filter: sitetype sitetypes sitetypecd sitetypecds
            retValue = isSiteType(value, errorMsg);

        } else if (key == "sitestatus") {
            // minor filter: sitestatus
            retValue = isSiteStatus(value, errorMsg);

        } else if (key == "localaquifercd") {
            // minor filter: localaquifercd
            retValue = isLocalAquiferCode(value, errorMsg);

        } else if (key == "aquifercd") {
            // minor filter: aquifercd
            retValue = isAquiferCode(value, errorMsg);

        } else if (key == "outputdatatype" || key == "outputdatatypecd" ) {
            // minor filter: outputdatatype
            retValue = isDataType(value, errorMsg);

        } else if (key == "hasdatatype" || key == "datatypecd" || key == "datatype" ) {
            // minor filter: hasdatatype
            retValue = isDataType(value, errorMsg);

        } else {
            // minor filter: <others> (these are currently unchecked)
            retValue = ( key == "period" || key == "startdt" || key == "enddt" ||
                         key == "modifiedsince" || key == "agencycd" || 
                         key == "altmin" || key == "altmax" || key == "altminva" || 
                         key == "altmaxva" || key == "drainareamin" || 
                         key == "drainareamax" || key == "drainareaminva" || 
                         key == "drainareamaxva" || key == "welldepthmin" || 
                         key == "welldepthmax" || key == "welldepthminva" || 
                         key == "welldepthmaxva" || key == "holedepthmin" || 
                         key == "holedepthmax" || key == "holedepthminva" || 
                         key == "holedepthmaxva" );
            if (!retValue) errorMsg.append("unrecognized filter: " + key);
        }

        if (retValue == false) break;
    }

    if (c != 1)
    {
        if (c == 0) errorMsg.prepend("no major filters");
        else errorMsg.prepend("too many major filters: " + QString::number(c));
        return false;
    }

    return retValue;
}

/**
   Return true if the supplied value is a valid format string, false otherwise.
*/
bool SitesRequest::isFormatString( const QString &value, QString &errorMsg )
{
    QString format, version;
    if (value.contains(","))
    {
        // a version is specified
        QStringList valueParts = value.split(",");
        format = valueParts.at(0);
        version = valueParts.at(1);

    } else {
        // no version specified
        format = value;
        version = "";
    }
   
    bool isSupported = (format == "mapper");
    if (not isSupported) errorMsg.append("unsupported format: " + format + "(supported: " + WaterSitesEngine::DEFAULT_FORMAT + ")");
    return isSupported;
}

bool SitesRequest::isDataType( const QString &value, QString &errorMsg )
{
    bool retvalue = false;
    int c = 0;
    if (value == "all")
    {
        retvalue = true;

    } else {
        QStringList valueParts = value.split(",");
        int num_types = valueParts.length();
        for (int i=0; i<num_types; i++)
        {
            QString type = valueParts.at(i).toLower();
            if (type == "dv" || type == "pk" || type == "sv" || type == "gw" || 
                type == "qw" || type == "id" || type == "aw" || type == "ad"  )
            {
                retvalue = true;   // valid types;

            } else if (type == "iv" || type == "uv" || type == "rt") {
                c++;   // valid type: one allowed
                retvalue = true;

            } else {
                errorMsg.append("unsupported type: " + type);
                return false; // invalid type
            }
            if (c > 1)
            {
                errorMsg.append("no more than one of type iv, uv, or rt allowed.");
                return false;
            }
        }
    }
    return retvalue;
}

/**
   Return true if the supplied value is in the format of a state code.
   Only one state code is allowed.
   example: az
*/
bool SitesRequest::isStateCode( const QString &request, QString &errorMsg )
{
    bool retValue = (request.length() == 2);
    if (not retValue) errorMsg.append("invalid state code: " + request);
    return retValue;
}

/**
   Return true if the request is in the form of a bBox (bounding box)
*/
bool SitesRequest::isBBoxCode( const QString &request, QString &errorMsg )
{
    // input: 4 comma separated decimal values (bBox=-83,36.6,-81,38.5)
    //   * <west lon>, <south lat>, <east lon>, <north lat>
    //   * lon/lat : no minutes or seconds are allowed
    //   * lon/lat : product of lat range and lon range may not exceed 25 degrees.

    QStringList components = request.split(",");
    int num_components = components.length();
    if (num_components != 4)
    {
        errorMsg.append("bBox requires 4 arguments but was given "
                        + QString::number(num_components) +  ": " + request);
        return false;
    }

    for (int i=0; i<num_components; i++)
    {
        QString component = components.at(i);

        bool a_double = false;
        if (!component.toDouble(&a_double))
        {
            errorMsg.append("invalid bBox component: " + component +  ": " + request);
            return false;
        }
        // todo: check product of lat range and long range against 25 degrees
    }

    return true;
}

/**
   Return true if the supplied request is in the format of a HUC code or a list
   of HUC codes seperated by comma. Major huc codes are 2 digits, minor huc
   codes are 8 digits. The max number of codes is 10; the list may contain only
   one major code.
*/
bool SitesRequest::isHucCode( const QString &request, QString &errorMsg )
{
    int c = 0, major_huc = 0;
    QStringList codes = request.split(",");
    int num_codes = codes.length();
    if (c > 10 || c < 1 || major_huc > 1) 
    {
        if (major_huc > 1) errorMsg.append("too many HUC codes (major) (limit 1): " + QString::number(major_huc));
        else errorMsg.append("too many HUC codes (all) (limit 10): " + QString::number(num_codes));
        return false;
    }

    for (int i=0; i<num_codes; i++)
    {
        QString code = codes.at(i);

        bool an_int = false;
        if (!code.toInt(&an_int))
        {
            errorMsg.append("invalid HUC code: " + code);
            return false;
        }

        if (code.length() == 2)            // a major huc code (one allowed)
        {
            major_huc++;
            c++;

        } else if (code.length() == 8) {   // a minor huc code
            c++;

        } else {                           // invalid length
            errorMsg.append("invalid HUC code: " + code);
            return false;
        }
    }
    return true;
}

/**
   Return true if the supplied request is in the format of a county code
   or a list of county codes separated by a comma. County codes are numeric
   and 5 digits in length. The maximum number of county codes is 20.
*/
bool SitesRequest::isCountyCode( const QString &request, QString &errorMsg )
{
    QStringList codes = request.split(",");
    int num_codes = codes.length();
    if (num_codes > 20 || num_codes < 1) 
    {
        errorMsg.append("too many county codes (limit 20): " + QString::number(num_codes));
        return false;
    }

    for (int i=0; i<num_codes; i++)
    {
       QString code = codes.at(i);

       bool an_int = false;
       if (!code.toInt(&an_int) || code.length() != 5)
       {
           errorMsg.append("invalid county code: " + code);
           return false;
       }
    }
    return true;
}

/**
   Returns true if the supplied request is in the format of a single site code
   or list of sites separated by a comma (with optional agency prefix). Lists 
   may contain up to 100 codes.
   Example: USGS:01646500,06306300
*/
bool SitesRequest::isSiteCode( const QString &request, QString &errorMsg )
{
    QStringList codes = request.split(",");
    int num_codes = codes.length();
    if (num_codes > 100 || num_codes < 1) 
    {
        errorMsg.append("too many site codes (limit 100): " + QString::number(num_codes));
        return false;
    }

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

        bool a_double = false;
        if (!code.toDouble(&a_double))
        {
            errorMsg.append("invalid site code: " + code);
            return false;
        }
    }
    return true;
}

/**
    Returns true if value is in the form of a valid site type.
*/
bool SitesRequest::isSiteType( const QString &value, QString &errorMsg )
{
    QStringList types = value.split(",");
    int num_types = types.length();
    if (num_types < 1)
    {
        errorMsg.append("invalid site type: " + value);
        return false;
    }

    for (int i=0; i<num_types; i++)
    {
        QString type = types.at(i);
        QString major = type.mid(0,2).toUpper();

        if (major != "GL" && major != "WE" && major != "LA" && major != "AT" && 
            major != "ES" && major != "OC" && major != "LK" && major != "ST" &&
            major != "SP" && major != "GW" && major != "SB" && major != "FA" &&
            major != "AG" && major != "AS" && major != "AW")
        {
            errorMsg.append("invalid site type: " + value);
            return false;
        }
    }
    return true;
}

/**
   Returns true if value is in the form of an aquifer code.
*/
bool SitesRequest::isAquiferCode( const QString &value, QString &errorMsg )
{
    QStringList codes = value.split(",");
    int num_codes = codes.length();
    if (num_codes > 1000 || num_codes < 1)
    {
        errorMsg.append("too many aquifer codes (limit 1000): " + QString::number(num_codes));
        return false;
    }

    for (int i=0; i<num_codes; i++)
    {
        QString code = codes.at(i);
        if (code.length() != 10)
        {
            errorMsg.append("invalid aquifer code: " + code);
            return false;
        }
    }
    return true;
}

/**
   Returns true if value is in the form of a local aquifer code.
*/
bool SitesRequest::isLocalAquiferCode( const QString &value, QString &errorMsg )
{
    QStringList codes = value.split(",");
    int num_codes = codes.length();
    if (num_codes > 1000 || num_codes < 1)
    {
        errorMsg.append("too many local aquifer codes (limit 1000): " + QString::number(num_codes));
        return false;
    }

    for (int i=0; i<num_codes; i++)
    {
        QStringList codeParts = codes.at(i).split(":");
        QString state = codeParts.at(0);
        QString code = codeParts.at(1);

        if (state.length() != 2 || code.length() != 7)
        {
            errorMsg.append("invalid local aquifer code: " + code);
            return false;
        }
    }
    return true;
}

/**
   Returns true if value is a valid siteStatus filter.
*/
bool SitesRequest::isSiteStatus( const QString &value, QString &errorMsg )
{
    bool isvalid = (value == "all" || value == "active" || value == "inactive");
    if (!isvalid) errorMsg.append("invalid site status: " + value);
    return isvalid;
}

