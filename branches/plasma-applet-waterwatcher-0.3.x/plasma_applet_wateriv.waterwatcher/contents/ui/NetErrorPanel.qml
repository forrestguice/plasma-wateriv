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

import QtQuick 1.0

Item
{
    id: dialog; 
    width: panel.width;
    height: panel.height;

    property int error: 0;

    states: [
        State
        {
            name: "NoError";
            when: (error == 0);
            PropertyChanges { target: panel; title: i18n("No Errors   "); }
            PropertyChanges { target: panel; content: i18n("<br/><br/>"); }
        },
        State
        {
            name: "HostNotFound";
            when: (error == 3);
            PropertyChanges { target: panel; title: i18n("Host Not Found"); }
            PropertyChanges { target: panel; content: i18n("Service host name was not found.&nbsp;&nbsp;<hr/>The network connection is unavailable.&nbsp;&nbsp;"); }
        },
        State
        {
            name: "BadRequest";
            when: (error == 400);
            PropertyChanges { target: panel; title: i18n("Bad Request (400)"); }
            PropertyChanges { target: panel; content: i18n("The USGS IV Web Service has rejected the request.&nbsp;&nbsp;<hr/>This message is displayed when request arguments<br/>are used inconsistently. This might happen when<br/>incompatible arguments are mixed (e.g. startDT<br/>with period) or arguments are incorrectly matched<br/>(e.g. startDT with timezone and endDT without)."); }
        },
        State
        {
            name: "AccessForbidden";
            when: (error == 403);
            PropertyChanges { target: panel; title: i18n("Access Forbidden (403)"); }
            PropertyChanges { target: panel; content: i18n("Your IP address has been blocked.<hr/>This message is typically due to excessive use that is<br/>seriously affecting the web service. If this message<br/>persists then send your request url and IP address to<br/>gs-w_support_nwisweb@usgs.gov for more information.&nbsp;&nbsp;"); }
        },
        State
        {
            name: "NotFound";
            when: (error == 404);
            PropertyChanges { target: panel; title: i18n("Data Not Found (404)"); }
            PropertyChanges { target: panel; content: i18n("The requested data is unavailable.<hr/>This may indicate:<ul><li>invalid site codes in the request</li><li>valid codes but that requested parameters are not served by those sites&nbsp;&nbsp;</li><li>values are missing for the requested timespan</li></ul><br/>"); }
        },
        State
        {
            name: "InternalServerError";
            when: (error == 500);
            PropertyChanges { target: panel; title: i18n("Internal Server Error (500)"); }
            PropertyChanges { target: panel; content: i18n("The USGS IV Web Service is currently unavailable.  "); }
        },
        State
        {
            name: "ServiceUnavailable";
            when: (error == 503);
            PropertyChanges { target: panel; title: i18n("Service Unavailable (503)"); }
            PropertyChanges { target: panel; content: i18n("The USGS IV Web Service was unable process the request.&nbsp;&nbsp;&nbsp;<hr/>This message is displayed when there are syntax errors in<br/>the request that are not handled by error codes 400 (bad)<br/>and 404 (not found)."); }
        },

        State
        {
            name: "Others";
            when: (error != 503 && error != 500 && error != 404 &&
                   error != 403 && error != 400 && error != 3 && 
                   error != 0);
            PropertyChanges { target: panel; title: "Network Error (" + error + ")   "; }
            PropertyChanges { target: panel; content: i18n("<br/><br/>"); }
        }
    ]

    InfoPanel
    {
        id: panel;
        anchors.verticalCenter: dialog.center;
        anchors.horizontalCenter: dialog.center;
    }
}
