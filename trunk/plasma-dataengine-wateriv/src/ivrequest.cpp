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
    Takes a source name and returns a corresponding fully formed request url.
    Returns "-1" if source name cannot be transformed into a request url; the
    error message is placed into errorMsg;
*/

#include "ivrequest.h"
#include "waterivengine.h"
//#include <QStringList>

QString IVRequest::requestForSource(const QString &source, QString &errorMsg)
{
    QString request;
    if (source.contains("?"))
    {
       if (not hasMajorFilter(source, errorMsg)) 
       {
           errorMsg.prepend("wateriv (url): " + source);
           return "-1";
       }

       if (source.split("?").at(0) == "")  // partial ur: add base url
       {
           request.append(WaterIVEngine::DEFAULT_SERVER + source);
       
       } else {                           // full url: pass as request
           request = QString(source);
       }

    } else {  // no url supplied: form the request
        request.append(WaterIVEngine::DEFAULT_SERVER + "?format=waterml,1.1");

        if (source.contains("&"))         // list of args:
        {
            if (not hasMajorFilter(source, errorMsg))
            {
                errorMsg.prepend("wateriv (args): " + source);
                return "-1";
            }
            request.append("&" + source);
        
        } else {
            if (source.contains("="))     // single arg: source is major filter
            {
                if (not hasMajorFilter(source, errorMsg)) 
                {
                    errorMsg.prepend("wateriv (arg): " + source);
                    return "-1";
                }
                request.append("&" + source);
            
            } else {                      // no args: source is list of sites
                if (not isSiteCode(source, errorMsg)) 
                {
                    errorMsg.prepend("wateriv (site): " + source);
                    return "-1";
                }
                request.append("&sites=" + source);
            }
        }
    }
    return request;
}

/**
   Returns true if the supplied request has one of the required major filters.
   Returns false if the major filter is missing or if there is more than one 
   major filter.
*/
bool IVRequest::hasMajorFilter( const QString &source, QString &errorMsg )
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
        if (args.at(i) == "") continue;    // special case: started with &
        if (not args.at(i).contains("="))  // invalid case: not a key=value pair
        {
            errorMsg.prepend(": syntax error: argument without an equal sign (=) :: " + args.at(i));
            return false;
        }

        QStringList pair = args.at(i).split("=");
        QString key = pair.at(0).toLower();
        QString value = pair.at(1);
 
        if (key == "sites")
        {
            retValue = IVRequest::isSiteCode(value, errorMsg);
            c++;

        } else if (key == "statecd") {
            retValue = IVRequest::isStateCode(value, errorMsg);
            c++;

        } else if (key == "huc") {
            retValue = IVRequest::isHucCode(value, errorMsg);
            c++;

        } else if (key == "bbox") {
            retValue = IVRequest::isBBoxCode(value, errorMsg);
            c++;

        } else if (key == "countycd") {
            retValue = IVRequest::isCountyCode(value, errorMsg);
            c++;
        }
        if (retValue == false) break;
    }

    if (c != 1)
    {
        if (c == 0) errorMsg.prepend(": filter error: no major filters");
        else errorMsg.prepend(": filter error: too many major filters: " + QString::number(c));
        return false;
    }

    return retValue;
}

/**
   Return true if the supplied request is in the format of a state code.
   Only one state code is allowed.
   example: az
*/
bool IVRequest::isStateCode( const QString &request, QString &errorMsg )
{
    bool retValue = (request.length() == 2);
    if (not retValue) errorMsg.append(": filter error: invalid state code: " + request);
    return retValue;
}

/**
   Return true if the request is in the form of a bBox (bounding box)
*/
bool IVRequest::isBBoxCode( const QString &request, QString &errorMsg )
{
    errorMsg.append(": filter error: unsupported major filter (bBox): " + request);
    return false;  // todo: support bbox as major filter
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
    for (int i=0; i<num_codes; i++)
    {
        QString code = codes.at(i);

        bool is_int = true;
        code.toInt(&is_int);
        if (not is_int) 
        {
            errorMsg.append(": filter error: invalid HUC code: " + code);
            return false;
        }

        if (code.length() == 2)            // a major huc code (one allowed)
        {
            major_huc++;
            c++;

        } else if (code.length() == 8) {   // a minor huc code
            c++;

        } else {                           // invalid length
            errorMsg.append(": filter error: invalid HUC code: " + code);
            return false;
        }
    }

    if (c > 10 || c < 1 || major_huc > 1) 
    {
        if (major_huc > 1) errorMsg.append(": filter error: too many HUC codes (major) (limit 1): " + QString::number(major_huc));
        else errorMsg.append(": filter error: too many HUC codes (all) (limit 10): " + QString::number(num_codes));
        return false;
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
    for (int i=0; i<num_codes; i++)
    {
       QString code = codes.at(i);

       bool is_int = true;
       code.toInt(&is_int);
       if (not is_int || code.length() != 5)
       {
           errorMsg.append(": filter error: invalid county code: " + code);
           return false;
       }
    }

    if (num_codes > 20 || num_codes < 1) 
    {
        errorMsg.append(": filter error: too many county codes (limit 20): " + QString::number(num_codes));
        return false;
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

        bool an_int = true;
        code.toInt(&an_int);
        if (not an_int) 
        {
            errorMsg.append(": filter error: invalid site code: " + code);
            return false;
        }
    }

    if (num_codes > 100 || num_codes < 1) 
    {
        errorMsg.append(": filter error: too many site codes (limit 100): " + QString::number(num_codes));
        return false;
    }
    return true;
}
 
