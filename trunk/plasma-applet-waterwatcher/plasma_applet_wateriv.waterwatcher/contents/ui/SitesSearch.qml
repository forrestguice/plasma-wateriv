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
import org.kde.plasma.core 0.1 as PlasmaCore

Column
{
    id: searchpanel; 

    property bool hasSiteEngine: false;

    spacing: 5; anchors.bottomMargin: 5;
    height: welcomeLabel.paintedHeight + anchors.bottomMargin + searchRow.height + siteList.height;

    states: [
        State
        {
            name: "AVAILABLE";
            when: searchpanel.hasSiteEngine;
            PropertyChanges { target: welcomeLabel; text: i18n("<b>USGS Site Service</b>"); }
            PropertyChanges { target: searchRow; visible: true; }
        },
        State
        {
            name: "UNAVAILABLE";
            when: !searchpanel.hasSiteEngine;
            PropertyChanges { target: welcomeLabel; text: i18n("<b>USGS Site Service</b> <i>(unavailable)</i><br/>Searching for sites requires <i>plasma-dataengine-watersites</i>."); }
            PropertyChanges { target: searchRow; visible: false; }
        }
    ]

    PlasmaCore.DataSource
    {
        id: siteengine; engine: "watersites";
        onDataChanged: 
        {
            var results = siteengine.data[searchField.input.text];
            if (typeof results === "undefined") return;

            siteList.model = siteModel;
            //siteList.model = Qt.createComponent("SitesSearchModel.qml")
            siteList.model.dataSource = siteengine;
        }
        Component.onCompleted: { searchpanel.hasSiteEngine = siteengine.valid; }
    }

    Text
    {
        id: welcomeLabel;
        text: i18n("<b>Connecting to plasma-dataengine-watersites . . .</b>");
        anchors.topMargin: 10; anchors.left: parent.left;
    }

    Row
    {
        id: searchRow;
        spacing: 5; anchors.left: parent.left;

        Text
        {
            text: "Search:";
            anchors.verticalCenter: parent.verticalCenter;
        }
  
        TextField
        {
            id: searchField; width: 250;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { siteList.model = null; siteengine.connectSource(searchField.input.text); }
        }
    }

    ListView 
    {
        id: siteList;
        height: 100; width: 300;

        model: siteModel;

        delegate: TextEdit
        {
            selectByMouse: true;
            text: code + ": " + name;
        }
    }

    SitesSearchModel
    {
        id: siteModel;
    }
}
