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

Column
{
    id: panel; 
    spacing: 5; anchors.bottomMargin: 5;

    property variant searchPanel: siteSearch;  // search panel
    property variant field: inputDataSource;   // input field
    property variant error: errorField;        // error field
    property int recent_sources_top: -1;       // recent sources top
    property int recent_sources_max: 10;       // recent sources max

    signal dataSourceChanged;

    state: "NORMAL";
    states: [
        State
        {
            name: "NORMAL";
            PropertyChanges { target: errorField; visible: false; }
        },
        State
        {
            name: "ERROR";
            PropertyChanges { target: errorField; visible: true; }
            PropertyChanges { target: errorField; state: "READONLY"; }
        }
    ]

    Row
    {
        spacing: 0; anchors.left: parent.left;
        Rectangle { width: 5; height: 5; color: "transparent"; }
        Text
        {
            id: label; text: "Data Source:";
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
        TextField
        {
            id: inputDataSource;
            width: 315; textInitial: dataRequest;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { setDataSource(); }
            Keys.onDownPressed: { nextRecentSource(); }
            Keys.onUpPressed: { prevRecentSource(); }
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    TextField
    {
        id: errorField;
        anchors.leftMargin: label.width + 5;
        anchors.left: parent.left;
        width: 315;
    }

    Row
    {
        Rectangle { width: 5; height: 5; color: "transparent"; }
        SiteSearchMainPanel
        {
            id: siteSearch;
            onSelected: { field.input.text = site; field.input.focus = true; }
            onSelectedAnd: { field.input.text = (field.input.text + "," + site).replace(" ",""); field.input.focus = true; }
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    /**
        Set the data source, add to the recent sources, and close the dialog.
        @param string the data source to change to
    */
    function setDataSource()
    {
        var src = field.input.text.replace(" ","");
        addToRecentSources(src);
        plasmoid.writeConfig("datasource", src); 
        panel.state = "NORMAL";
        panel.dataSourceChanged();
    }

    /**
        hasRecentSource(source) : function
        @returns true if source is already in recent sources, false otherwise
    */
    function hasRecentSource(source)
    {
        var i = 0;
        for (i=0; i<10; i++)
        {
            var value = plasmoid.readConfig("recent_source_" + i, "");
            if (value == source) return true;
        }
        return false;
    }
     
    /**
        Shift top of the recent sources list up by one or more (show next).
        @return int the new index of top
    */
    function nextRecentSource()
    {
        recent_sources_top = plasmoid.readConfig("recent_source_top", 0);
        var c = recent_sources_top;
        do {
            recent_sources_top += 1;
            if (recent_sources_top >= recent_sources_max) recent_sources_top = 0;
            var value = plasmoid.readConfig("recent_source_" + recent_sources_top, "");
            if (value != "")  
            {
                plasmoid.writeConfig("recent_source_top", recent_sources_top);
                break;
            }
        } while (recent_sources_top != c);

        var value = plasmoid.readConfig("recent_source_" + recent_sources_top, "");
        if (value != "") field.input.text = value;
        //console.log("next to: " + recent_sources_top);
    }
   
    /**
        Shift top of the recent sources list down by one or more (show prev).
        @return int the new index of top
    */
    function prevRecentSource()
    {
        recent_sources_top = plasmoid.readConfig("recent_source_top", 0);
        var c = recent_sources_top;
        do {
            recent_sources_top -= 1;
            if (recent_sources_top < 0) recent_sources_top = recent_sources_max - 1;
            var value = plasmoid.readConfig("recent_source_" + recent_sources_top, "");
            if (value != "")  
            {
                plasmoid.writeConfig("recent_source_top", recent_sources_top);
                break;
            }
        } while (recent_sources_top != c);

        var value = plasmoid.readConfig("recent_source_" + recent_sources_top, "");
        if (value != "") field.input.text = value;
        //console.log("previous to: " + recent_sources_top);
    }

    /**
        Add a source to the top of the recent sources list (circular buffer).
        @param source a string to add to the top of the recent sources
    */
    function addToRecentSources(source)
    {
        if (hasRecentSource(source)) return;
        recent_sources_top = plasmoid.readConfig("recent_source_top", -1);
        if (recent_sources_top == -1)
        {   // init first element (first use)
            plasmoid.writeConfig("recent_source_0", "");
            recent_sources_top = 0;
        }
        recent_sources_top++;
        if (recent_sources_top >= recent_sources_max) recent_sources_top = 0;
        plasmoid.writeConfig("recent_source_top", recent_sources_top);
        plasmoid.writeConfig("recent_source_" + recent_sources_top, source);
        //console.log("added recent source: " + recent_sources_top);
    }

}
