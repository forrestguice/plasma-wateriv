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
import "plasmapackage:/code/neterrors.js" as NetErrors

Item
{
    id: dialog; width: panel.width; height: panel.height;
    property int error: 0;

    states: [
        State
        {
            name: "NoError"; when: (error == 0);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "HostNotFound"; when: (error == 3);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "BadRequest"; when: (error == 400);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "AccessForbidden"; when: (error == 403);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "NotFound"; when: (error == 404);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "InternalServerError"; when: (error == 500);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "ServiceUnavailable"; when: (error == 503);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        },
        State
        {
            name: "Others";
            when: (error != 503 && error != 500 && error != 404 &&
                   error != 403 && error != 400 && error != 3 && error != 0);
            PropertyChanges { target: panel; title: NetErrors.nameForErrorCode(error); }
            PropertyChanges { target: panel; content: NetErrors.msgForErrorCode(error); }
        }
    ]

    InfoPanel
    {
        id: panel;
        anchors.verticalCenter: dialog.center;
        anchors.horizontalCenter: dialog.center;
    }

}
