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
import org.kde.qtextracomponents 0.1 as QtExtraComponents

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
    id: main;
  
    width: 175;
    height: 75;
    property int minimumWidth: 175;
    property int minimumHeight: 75;

    property string setting_dataRequest: "-1";
    property int setting_pollingInterval: -1;

    //property Component compactRepresentation: Text { text:"test" }

    Component.onCompleted: 
    {
        //plasmoid.popupIcon = "konqueror";
    }

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
            configChanged();   // hack: must try to connect twice before dataUpdated
            configChanged();   // is triggered for the first time for some reason
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
        main_tooltip :: PlasmaCore.ToopTip 
        The ToolTip UI
    */
    PlasmaCore.ToolTip
    {
        id: main_tooltip
        target: main;
        mainText: "Water Watcher";    // todo: move / localize strings?
        subText: "(not connected)";
        image: "konqueror";          // todo: replace icon
    }

    /**
        layout :: Column
        The Main UI
    */
    Column
    {
        id: layout;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;
        spacing: 8;

        Text 
        {
            id: displayLabel_value
            text: "Water Watcher"
            font.pointSize: 24; font.bold: true;
            anchors.horizontalCenter: parent.horizontalCenter;

            //signal customSignal()
            //onCustomSignal: { console.log(plasmoid.readConfig("datasource")); }
            //MouseArea { id: mouseArea; anchors.fill: parent; onClicked: { displayLabel1.customSignal(); }}
        }

        Text
        {
            id: displayLabel_date
            text: "(not connected)"
            font.pointSize: 12; font.bold: false;
            anchors.horizontalCenter: parent.horizontalCenter
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
                var site = results["timeseries_0_sourceinfo_sitename"];
                var value = results["timeseries_0_values_recent"];
                var units = results["timeseries_0_variable_unitcode"];
                var date = "March 23, 2012 17:45";

                main_tooltip.mainText = "" + site;
                main_tooltip.subText = "" + date + "<br/>" + value + " (" + units + ")";
                displayLabel_value.text = "" + value + " (" + units + ")";
                displayLabel_date.text = "" + date;

            } else {
                main_tooltip.mainText = "Water Watcher";
                main_tooltip.subText = "(received invalid data)";
                displayLabel_value.text = "Water Watcher";
                displayLabel_date.text = "(received invalid data)";
            }

        } else {
            main_tooltip.mainText = "Water Watcher";
            main_tooltip.subText = "(network request failed)";
            displayLabel_value.text = "Water Watcher";
            displayLabel_date.text = "(network request failed)";
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

}
