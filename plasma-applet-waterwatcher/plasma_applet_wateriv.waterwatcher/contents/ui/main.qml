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

/**
    WaterWatcher (QML Plasmoid)
    http://code.google.com/p/plasma-wateriv/

    ==========================================================================
    || Description:                                                         ||
    ==========================================================================

       WaterWatcher is a plasmoid that displays the latest real-time water data 
       from the USGS Instantaneous Values Web Service. Data is available from
       thousands of sites around the United States. Readings are made every 15
       minutes and transmitted hourly (http://waterservices.usgs.gov/rest).

    ==========================================================================
    || Requirements:                                                        ||
    ==========================================================================

       KDE 4.7+
       http://www.kde.org

       WaterIV DataEngine v0.1 (plasma-dataengine-wateriv)
       http://code.google.com/p/plasma-wateriv/

    ==========================================================================
    || Installation:                                                        ||
    ==========================================================================

       1) Install the required data engine (plasma-dataengine-wateriv)
          (http://code.google.com/p/plasma-wateriv/downloads/list)

       2) << installation directions go here >>

    ==========================================================================
    || Configuration:                                                       ||
    ==========================================================================

       Provide a valid site code in the settings dialog. Use NSWIS Mapper 
       (http://wdr.water.usgs.gov/nwisgmap/index.html) to locate sites.
*/

Item
{
    id: plasmoid_waterwatcher;

    property string setting_dataRequest: "-1";
    property int setting_pollingInterval: -1;

    //property int minimumWidth: paintedWidth;
    //property int minimumHeight: paintedHeight;

    //Component.onCompleted: {}

    /**
       loadtimer : Timer
    */
    Timer
    {
        id: loadtimer;
        interval: 1000; running: false; repeat: false;

        onTriggered: 
        {
            plasmoid.addEventListener("ConfigChanged", configChanged);
            firstConfig();
        }
    }

    /**
        dataengine : PlasmaCore.DataSource
    */
    PlasmaCore.DataSource 
    {
        id: dataengine
        engine: "wateriv"
        interval: 60 * 60000;  // 1hr

        //onSourceAdded: { console.log("onSourceAdded: "+valid+" " + connectedSources); }
        //onSourceRemoved: { console.log("onSourceRemoved: " + source); }
        onSourceConnected: { console.log("onSourceConnected: " + source); }
        onSourceDisconnected: { console.log("onSourceDisconnected: " + source); }
        //onIntervalChanged: { console.log("onIntervalChanged: " + interval); }
        onNewData: { console.log("onNewData: "); }

        onDataChanged:
        {
            refreshDisplay();   // refresh display when data changes
        }

        Component.onCompleted: 
        {
            loadtimer.start(); // load settings after a short delay
        }
    }

    /** 
       theme :: PlasmaCore.Theme
    */
    PlasmaCore.Theme 
    {
        id: theme
    }
 
    /**
        displayLabel1 : Text
    */
    Text 
    {
        id: displayLabel1

        text: "-1"
        font.pointSize: 16; font.bold: true;
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        signal customSignal()
        onCustomSignal: 
        { 
           console.log("custom signal sent and received: " + plasmoid.readConfig("datasource"));
        }

        MouseArea
        { 
            id: mouseArea 
            anchors.fill: parent 
            onClicked: { displayLabel1.customSignal(); }
        }
    }

    /**
        refreshDisplay() : function
        Retrieve data from the connected dataengine and update the display.
    */
    function refreshDisplay()
    {
        console.log("refreshDisplay");
        var results = dataengine.data[setting_dataRequest];

        var netIsValid = results["net_isvalid"];
        if (netIsValid == true)
        {
            var xmlIsValid = results["xml_isvalid"];
            if (xmlIsValid == true)
            {
                var value = results["timeseries_0_values_recent"];
                var units = results["timeseries_0_variable_unitcode"];
                displayLabel1.text = "" + value + " (" + units + ")";

            } else {
                displayLabel1.text = "!invalid data!";
            }

        } else {
            displayLabel1.text = "!network request failed!";
        }
    }

    /** 
       configChanged() : function
       The configuration object was changed; update properties and reconnect to
       the dataengine if the request was changed.
    */
    function configChanged()
    {
        console.log("configChanged");

        // setting - polling interval (may not be less than 15min)
        setting_pollingInterval = plasmoid.readConfig("datapolling");
        if (setting_pollingInterval < 15) setting_pollingInterval = 15;
        dataengine.interval = setting_pollingInterval * 60 * 1000;

        // setting - requested data (dataengine source)
        if (setting_dataRequest != "-1")
        {
            dataengine.disconnectSource(setting_dataRequest);
        }
        setting_dataRequest = plasmoid.readConfig("datasource");
        dataengine.connectSource(setting_dataRequest);
    }

    function firstConfig()
    {
        console.log("firstConfig");
        configChanged();   // hack: must try to connect twice before dataUpdated
        configChanged();   // is triggered for the first time for some reason
    }

}
