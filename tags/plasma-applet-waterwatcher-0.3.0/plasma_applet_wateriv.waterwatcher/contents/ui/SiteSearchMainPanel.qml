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
import "plasmapackage:/code/listsort.js" as ListSort

Column
{
    id: searchpanel; spacing: 5;
    anchors.verticalCenter: parent.center;
    height: welcomeLabel.paintedHeight + anchors.bottomMargin + 
            querypanel.paintedHeight + s1.paintedHeight + s2.paintedHeight + 
            resultspanel.paintedHeight + filterpanel.paintedHeight + 32;

    property bool hasSiteEngine: false;
    property bool collapsed: true;
    property variant filterPanel: filterpanel;

    signal selected(string site);
    signal selectedAnd(string site);

    states: [
        State
        {
            name: "AVAILABLE"; when: searchpanel.hasSiteEngine;
            PropertyChanges { target: welcomeLabel; text: i18n("<b>Search for Sites</b>"); }
        },
        State
        {
            name: "UNAVAILABLE"; when: !searchpanel.hasSiteEngine;
            PropertyChanges { target: welcomeLabel; text: i18n("<b>Search for Sites</b> <i>(unavailable)</i><br/>Searching for sites requires <i>plasma-dataengine-watersites</i>."); }
            PropertyChanges { target: searchpanel; collapsed: true; }
        }
    ]

    PlasmaCore.DataSource
    {
        id: siteengine; engine: "watersites";
        onDataChanged: { searchpanel.displayResults(); }
        Component.onCompleted: { searchpanel.hasSiteEngine = siteengine.valid; }
    }
   
    Keys.onDownPressed: { resultspanel.list.currentIndex += (resultspanel.list.currentIndex >= resultspanel.list.count-1) ? 0 : 1; }
    Keys.onUpPressed: { resultspanel.list.currentIndex -= (resultspanel.list.currentIndex <= 0) ? 0 : 1; }
    Keys.onReturnPressed: { searchpanel.selected(resultspanel.model.get(resultspanel.list.currentIndex).code); }

    Rectangle { id: s1; width: parent.width; height: 2; color: "transparent"; }

    Row
    {
        spacing: 5; anchors.left: parent.left;
        ImgButton
        {
            image: configSvg; element: "add"; width: 16; height: 16;
            anchors.verticalCenter: parent.center;
            visible: collapsed;
            onAction: { toggleSearchUI(); }
        }
        ImgButton
        {
            image: configSvg; element: "remove"; width: 16; height: 16;
            anchors.verticalCenter: parent.center;
            visible: !collapsed;
            onAction: { toggleSearchUI(); }
        }
        Text
        {
            id: welcomeLabel; anchors.verticalCenter: parent.center;
            text: i18n("<b>Connecting to plasma-dataengine-watersites . . .</b>");
        }
    }

    SiteSearchFilterPanel
    {
        id: filterpanel;
        onFiltersChanged: { querypanel.setQuery(filterpanel.filters); }
        visible: !collapsed && hasSiteEngine;
    }

    Rectangle 
    { 
        id: s2; width: parent.width; height: 2; color: "transparent"; 
        visible: !collapsed && hasSiteEngine;
    }

    SiteSearchQueryPanel
    {
        id: querypanel;
        onPerformSearch: { searchpanel.performSearch(); }
        visible: !collapsed && hasSiteEngine;
    }

    SiteSearchResultsPanel
    {
        id: resultspanel;
        onDoubleClicked: { searchpanel.selected(code); }
        onPressAndHold: { searchpanel.selectedAnd(code); }
        onClicked: { searchpanel.focus = true; }
        visible: !collapsed && hasSiteEngine;
    }

    //
    // functions
    //

    /**
        performSearch() : function
        Query the watersites data engine with the configured search parameters.
    */
    function performSearch()
    {
        resultspanel.model.clear();
        resultspanel.model.append( {"name":"", "code":i18n("<i>Searching . . .</i>"), "header":false} );
        siteengine.connectedSources = [querypanel.query];
    }

    /**
        displayResults() : function
        Display results returned from the watersites data engine by repopulating
        the list model.
    */
    function displayResults()
    {
        var results = siteengine.data[querypanel.query];
        if (typeof results === "undefined") return;

        var requestIsvalid = results["net_request_isvalid"];
        if (typeof requestIsvalid === "undefined" || requestIsvalid == false)
        {
            resultspanel.model.clear();
            resultspanel.model.append( {"name":results["net_request_error"], "code":i18n("<b>Query Error</b>"), "header":false} );
            return;
        }

        var isvalid = results["net_isvalid"];
        if (typeof isvalid === "undefined" || isvalid == false)
        {
            var netError = results["net_error"];
            if (netError == 203 || netError == 404)
            {
                resultspanel.model.clear();
                resultspanel.model.append( {"name":i18n(""), "code":i18n("<i>No sites found</i>"), "header":false} );
            } else {
                resultspanel.model.clear();
                resultspanel.model.append( {"name":results["net_error"], "code":i18n("<b>Net Error</b>"), "header":false} );
            }
            return;
        }

        var numSites = results["site_count"];
        if (typeof numSites === "undefined" || numSites <= 0) return;
   
        resultspanel.model.clear();
        for (var key in results)
        {
            if (key != "site_count" && key.substring(0, "site_".length) == "site_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var i = ListSort.findSortedPosition(nameValue, resultspanel.model);
                resultspanel.model.insert(i, {"code":result["code"], "name":nameValue, "header":false});
            }
        }
        resultspanel.list.currentIndex = 0;
    }

    function toggleSearchUI()
    {
        searchpanel.collapsed = !searchpanel.collapsed;
        plasmoid.writeConfig("searchpanel_collapsed", searchpanel.collapsed);
    }

}
