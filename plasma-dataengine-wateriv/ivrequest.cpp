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
    IVRequest
 
    This class contains functions for checking the validity of source names in
    an attempt to throw out bad requests before they are passed to the web
    service.
*/

#include "ivrequest.h"
#include "waterivengine.h"

/** 
    Takes a source name and returns a corresponding fully formed request url.
    Returns "-1" if source name cannot be transformed into a request url; the
    error message is placed into errorMsg;
*/

QString IVRequest::requestForSource(const QString &source, QString &errorMsg)
{
    QString request;
    bool hasFormatFilter = false;

    if (source.contains("?"))    // full url:
    {
       if (not hasValidFilters(source, hasFormatFilter, errorMsg))
       {
           //errorMsg.prepend("wateriv (url): " + source);
           return "-1";
       }

       QString urlPart = source.split("?").at(0).toLower();
       if (urlPart == "")
       {
           // no url: default to instantaneous values service
           request.prepend(WaterIVEngine::DEFAULT_SERVER);

       } else if (urlPart == "iv") {
           // pseudo url: instantaneous values
           request = source.mid(2).prepend(WaterIVEngine::DEFAULT_SERVER_IV);

       } else if (urlPart == "dv") {
           // pseudo url: daily values
           request = source.mid(2).prepend(WaterIVEngine::DEFAULT_SERVER_DV);

       } else {
           request = QString(source); 
       }

       // always specify the format
       if (!hasFormatFilter) request.append("&format=" + WaterIVEngine::DEFAULT_FORMAT);

    } else {  // no url supplied: form the request
        request.append(WaterIVEngine::DEFAULT_SERVER); 
        if (source.contains("&"))         // list of args:
        {
            if (not hasValidFilters(source, hasFormatFilter, errorMsg))
            {
                //errorMsg.prepend("wateriv (args): " + source);
                return "-1";
            }
            if (hasFormatFilter) request.append("?");
            else request.append("?format=" + WaterIVEngine::DEFAULT_FORMAT + "&");
            request.append(source);
        
        } else {
            if (source.contains("="))     // single arg: source is major filter
            {
                if (not hasValidFilters(source, hasFormatFilter, errorMsg)) 
                {
                    //errorMsg.prepend("wateriv (arg): " + source);
                    return "-1";
                }
                request.append("?format=" + WaterIVEngine::DEFAULT_FORMAT + "&" + source);
            
            } else {                      // no args: source is list of sites
                if (not isSiteCode(source, errorMsg))
                {
                    //errorMsg.prepend("wateriv (site): " + source);
                    return "-1";
                }
                request.append("?format=" + WaterIVEngine::DEFAULT_FORMAT + "&sites=" + source);
            }
        }
    }
    return request;
}

/**
   Returns true if the supplied request has valid filters (including one of the 
   required major filters). Returns false if the major filter is missing, 
   there is more than one major filter, there are unrecognized filters, or
   the value of one of the supplied major/minor filters has incorrect syntax.
   See http://waterservices.usgs.gov/rest/IV-Service.html for the spec
*/
bool IVRequest::hasValidFilters( const QString &source, bool &hasFormatFilter, QString &errorMsg )
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

        } else if (key == "parametercd" || key == "parametercds" || 
                   key == "variable" || key == "variables" || 
                   key == "var" || key== "vars" || key == "parmcd") {
            // minor filter: parametercd parametercds variable var vars parmcd
            retValue = isParameterCode(value, errorMsg);

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
            if (!retValue) errorMsg.append(": filter error: unrecognized filter: " + key);
        }

        if (retValue == false) break;
    }

    if (c != 1)
    {
        if (c == 0) errorMsg.prepend("missing a major filter");
        else errorMsg.prepend("too many major filters " + QString::number(c));
        return false;
    }

    return retValue;
}

/**
   Return true if the supplied value is in the form of a parameter (variable)
   code or list of codes (max 100).
*/
bool IVRequest::isParameterCode( const QString &value, QString &errorMsg )
{
    QStringList codes = value.split(",");
    int num_codes = codes.length();

    if (num_codes > 100 || num_codes < 1)
    {
        errorMsg.append("too many variable codes (limit 100): " + QString::number(num_codes));
        return false;
    }

    for (int i=0; i<num_codes; i++)
    {
        QString code = codes.at(i);

        bool an_int = false;
        if (!code.toInt(&an_int) || code.length() != 5)
        {
            errorMsg.append("invalid variable code: " + code);
            return false;
        }
    }
    return true;
}

/**
   Return true if the supplied value is a valid format string, false otherwise.
*/
bool IVRequest::isFormatString( const QString &value, QString &errorMsg )
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
   
    bool isSupported = (format == "waterml");
    if (not isSupported) errorMsg.append("unsupported format: " + format + "(supported: " + WaterIVEngine::DEFAULT_FORMAT + ")");
    return isSupported;
}

/**
   Return true if the supplied value is in the format of a state code.
   Only one state code is allowed.
   example: az
*/
bool IVRequest::isStateCode( const QString &request, QString &errorMsg )
{
    bool retValue = (request.length() == 2);
    if (not retValue) errorMsg.append("invalid state code: " + request);
    return retValue;
}

/**
   Return true if the request is in the form of a bBox (bounding box)
*/
bool IVRequest::isBBoxCode( const QString &request, QString &errorMsg )
{
    // input: 4 comma separated decimal values (bBox=-83,36.6,-81,38.5)
    //   * <west lon>, <south lat>, <east lon>, <north lat>
    //   * lon/lat : no minutes or seconds are allowed
    //   * lon/lat : product of lat range and lon range may not exceed 25 degrees.

    QStringList components = request.split(",");
    int num_components = components.length();
    if (num_components != 4)
    {
        errorMsg.append("filter bBox requires 4 arguments but was given "
                        + QString::number(num_components) +  ": " + request);
        return false;
    }

    for (int i=0; i<num_components; i++)
    {
        QString component = components.at(i);

        bool a_double = false;
        if (!component.toDouble(&a_double))
        {
            errorMsg.append("invalid bBox filter: " + component +  ": " + request);
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
bool IVRequest::isHucCode( const QString &request, QString &errorMsg )
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
bool IVRequest::isCountyCode( const QString &request, QString &errorMsg )
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
bool IVRequest::isSiteCode( const QString &request, QString &errorMsg )
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
bool IVRequest::isSiteType( const QString &value, QString &errorMsg )
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
bool IVRequest::isAquiferCode( const QString &value, QString &errorMsg )
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
bool IVRequest::isLocalAquiferCode( const QString &value, QString &errorMsg )
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
bool IVRequest::isSiteStatus( const QString &value, QString &errorMsg )
{
    bool isvalid = (value == "all" || value == "active" || value == "inactive");
    if (!isvalid) errorMsg.append("invalid site status: " + value);
    return isvalid;
}

