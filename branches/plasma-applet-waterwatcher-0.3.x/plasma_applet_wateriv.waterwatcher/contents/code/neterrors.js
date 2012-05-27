/**
    Copyright 2012 Forrest Guice
    This file is part of Plasma-WaterIV.

    Plasma-WaterIV is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
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
    nameForErrorCode() : function
    @param code the error code
    @return a short string suitable for display explaining the code
*/
function nameForErrorCode( code )
{
    var errorName;
    switch (code)
    {
        case 0:
            errorName = i18n("No Errors");
            break;

        case 3:
            errorName = i18n("Host Not Found"); 
            break;

        case 99:
            errorName = i18n("Unknown Error"); 
            break;

        case 400:
            errorName = i18n("Bad Request (400)"); 
            break;

        case 403:
            errorName = i18n("Access Forbidden (403)"); 
            break;

        case 404:
            errorName = i18n("Data Not Found (404)"); 
            break;

        case 500:
            errorName = i18n("Internal Server Error (500)"); 
            break;

        case 503:
            errorName = i18n("Service Unavailable (503)"); 
            break;

        default:
            errorName = i18n("Network Error (" + code + ")");
            break;
    }
    return errorName;
}

/**
    msgForErrorCode() : function
    @param code the error code
    @return a short string suitable for display explaining the code
*/
function msgForErrorCode( code )
{
    var errorMsg;
    switch (code)
    {
        case 0:     // no errors
            errorMsg = i18n("<br/><br/>");
            break;

        case 3:     // host not found
            errorMsg = i18n("Service host name was not found.&nbsp;&nbsp;<hr/>The network connection is unavailable.&nbsp;&nbsp;"); 
            break;

        case 99:     // unknown error
            errorMsg = i18n("An unknown network error occured.&nbsp;&nbsp;<hr/>The network connection is unavailable.&nbsp;&nbsp;"); 
            break;

        case 400:   // bad request
            errorMsg = i18n("The USGS IV Web Service has rejected the request.&nbsp;&nbsp;<hr/>This message is displayed when request arguments<br/>are used inconsistently. This might happen when<br/>incompatible arguments are mixed (e.g. startDT<br/>with period) or arguments are incorrectly matched<br/>(e.g. startDT with timezone and endDT without)."); 
            break;

        case 403:   // access forbidden
            errorMsg = i18n("Your IP address has been blocked.<hr/>This message is typically due to excessive use that is<br/>seriously affecting the web service. If this message<br/>persists then send your request url and IP address to<br/>gs-w_support_nwisweb@usgs.gov for more information.&nbsp;&nbsp;"); 
            break;

        case 404:   // data not found
            errorMsg = i18n("The requested data is unavailable.<hr/>This may indicate:<ul><li>invalid site codes in the request</li><li>valid codes but that requested parameters are not served by those sites&nbsp;&nbsp;</li><li>values are missing for the requested timespan</li></ul><br/>"); 
            break;

        case 500:   // internval server error
            errorMsg = i18n("The USGS IV Web Service is currently unavailable.  "); 
            break;

        case 503:   // service unavailable
            errorMsg = i18n("The USGS IV Web Service was unable process the request.&nbsp;&nbsp;&nbsp;<hr/>This message is displayed when there are syntax errors in<br/>the request that are not handled by error codes 400 (bad)<br/>and 404 (not found)."); 
            break;

        default:    // other errors
            errorMsg = i18n("<br/><br/>");
            break;
    }
    return errorMsg;
}

