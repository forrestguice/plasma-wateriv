/*
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
*/

Item
{
    id: main;
  
    width: 200; height: 100;
    property int minimumWidth: 50;
    property int minimumHeight: 25;

    property string setting_dataRequest: "-1";  // data request
    property int setting_pollingInterval: -1;   // polling interval
    property int setting_displaySeries: 0;      // series # to display

    property bool setting_showDate: true;
    property bool setting_showUnits: true;
    property bool setting_showShadow: true;
    property bool setting_customShadow: false;
    property bool setting_customColor: false;
     
    //Component.onCompleted: {}

    /**
        loadtimer : this timer runs when the plasmoid is first created.
    */
    Timer
    {
        id: loadtimer;
        interval: 1000; running: false; repeat: false;

        onTriggered: 
        {
            plasmoid.addEventListener("ConfigChanged", configChanged);
            configChanged();
        }
    }

    /**
        dataengine : the data source (starts loadtimer when completed).
    */
    PlasmaCore.DataSource 
    {
        id: dataengine;
        engine: "wateriv";
        interval: 60 * 60000;

        onDataChanged: { refreshDisplay(); }
        Component.onCompleted: { loadtimer.start(); }
    }

    /**
        theme : access to theme information (fonts, colors, etc)
    */
    PlasmaCore.Theme 
    {
        id: theme;
    }

    /**
        main_tooltip : the tooltip user interface (mouse over)
    */
    PlasmaCore.ToolTip
    {
        id: main_tooltip
        target: main;
        mainText: "Water Watcher";    // todo: move / localize strings?
        subText: "(not connected)";
        image: "konqueror";           // todo: replace icon
    }

    /**
        main_mousearea : a mouse area over the entire plasmoid (click action)
    */
    MouseArea
    {
        id: main_mousearea;
        anchors.fill: parent;
        onClicked: { showNextSeries(); }
    }

    /**
        smallWidget : the small/compact user interface
    */
    Column
    {
        id: smallWidget;

        spacing: 0;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        Text
        {
            id: display_value;

            text: "Water Watcher";
            style: Text.Raised; 
            font.bold: true; font.family: "Ubuntu";
            color: theme.textColor; styleColor: theme.backgroundColor;
            anchors.horizontalCenter: parent.horizontalCenter;

            property int default_size: 38;
            property int current_size: 38;
            font.pixelSize: (current_size * main.width) / 200; 

            onTextChanged: 
            {
                if (main.width <= 1 || display_value.paintedWidth <= 1) return;
                //display_value.current_size = display_value.default_size;
 
                while (display_value.paintedWidth < main.width && 
                       display_value.current_size < 64)
                {
                    //console.log("scaling up: " + display_value.current_size);
                    if (setting_showDate == false)
                    {
                        if ((display_value.paintedHeight) >= main.height) 
                            break;
                    } else {
                        if ((display_value.paintedHeight + display_date.paintedHeight) >= main.height) 
                            break;
                    }
                    display_value.current_size++;   // scale font up
                }
                //console.log("done scaling up: " + display_value.current_size);

                while (display_value.paintedWidth > main.width &&
                       display_value.current_size > 8)
                {
                    //console.log("scaling down: " + display_value.current_size);
                    display_value.current_size--;   // scale font down
                }
                //console.log("done scaling down: " + display_value.current_size);
            }
        }

        Text
        {
            id: display_date;

            text: "(not connected)";
            style: Text.Raised; 
            font.bold: false; font.family: "Ubuntu";
            color: theme.textColor; styleColor: theme.backgroundColor;
            anchors.horizontalCenter: parent.horizontalCenter;

            property int default_size: 24;
            property int current_size: 24;
            font.pixelSize: (current_size * main.width) / 200; 

            onTextChanged: 
            {
                if (main.width <= 0 || display_date.paintedWidth <= 0) return;
                // display_date.current_size = display_date.default_size;

                while ( display_date.paintedWidth > main.width && 
                       display_date.current_size > 8)
                {
                    display_date.current_size--;
                }
            }
        }
    }

    /**
        showNextSeries() : function
        Advance the display to the next available timeseries.
    */
    function showNextSeries()
    {
        var results = dataengine.data[setting_dataRequest];
        if (typeof results === "undefined") return;

        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") return;

        setting_displaySeries += 1;
        if (setting_displaySeries >= numSeries) setting_displaySeries = 0;
        refreshDisplay();
    }

    /**
        refreshDisplay() : function
        Retrieve data from the connected dataengine and update the display.
    */
    function refreshDisplay()
    {
        //console.log("refreshDisplay");
        var results = dataengine.data[setting_dataRequest];
        if (typeof results === "undefined") return;

        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") 
        {
            setting_displaySeries = 0;
            return;

        } else {
            if (setting_displaySeries >= numSeries)
            {
                setting_displaySeries = 0;
            }
        }

        var prefix = "timeseries_" + setting_displaySeries + "_";
        var netIsValid = results["net_isvalid"];
        if (typeof netIsValid === "undefined" || netIsValid == false)
        {
            main_tooltip.mainText = "Water Watcher";
            main_tooltip.subText = "(network request failed)";
            display_value.text = "Water Watcher";
            display_date.text = "(network request failed)";
            return;

        } else {
            var xmlIsValid = results["xml_isvalid"];
            if (typeof xmlIsValid === "undefined" || xmlIsValid == false)
            {
                main_tooltip.mainText = "Water Watcher";
                main_tooltip.subText = "(received invalid data)";
                display_value.text = "Water Watcher";
                display_date.text = "(received invalid data)";

            } else {
                var site_name = results[prefix + "sourceinfo_sitename"];
                var site_code = results[prefix + "sourceinfo_sitecode"];

                var var_code = results[prefix + "variable_code"];
                var var_name = results[prefix + "variable_name"];
                var var_desc = results[prefix + "variable_description"];
                var var_value = results[prefix + "values_recent"];
                var var_units = results[prefix + "variable_unitcode"];
                var var_date = Qt.formatDateTime(results[prefix + "values_recent_date"]);

                var qualifiers = results[prefix + "values_qualifiers"];
                var qualifier_code = results[prefix + "values_recent_qualifier"];
                var qualifier_desc = qualifiers[qualifier_code][2];

                main_tooltip.mainText = "" + site_name + " (" + site_code + ")";
                main_tooltip.subText = "" + var_name + " (" + var_code + ")<br/><br/><b>" + var_value + " " 
                                       + var_units + "</b><br/>" + var_date + "<br/><br/>"
                                       + qualifier_desc;

                if (setting_showUnits) display_value.text = "" + var_value + " " + var_units;
                else display_value.text = "" + var_value;
                display_date.text = "" + var_date;
            }
        }
    }

    /** 
       configChanged() : function
       The configuration object was changed; update properties and reconnect to
       the dataengine if the request was changed.
    */
    function configChanged()
    {
        updateFonts();
        updateFields();
        updateEngine();    // bug: source never connects unless we do this twice
        updateEngine();    // ... what is going on here.
    }

    function updateEngine()
    {
        //console.log("updateEngine");
        setting_pollingInterval = plasmoid.readConfig("datapolling");
        if (setting_pollingInterval < 15) setting_pollingInterval = 15;
        dataengine.interval = setting_pollingInterval * 60 * 1000;

        if (setting_dataRequest != "-1")
        {
            dataengine.disconnectSource(setting_dataRequest);
        }
        setting_dataRequest = plasmoid.readConfig("datasource");
        dataengine.connectSource(setting_dataRequest);
    }

    /** 
        updateFields() : function
        Updates visibility of the display fields using the config object.
    */
    function updateFields()
    {
        setting_showUnits = plasmoid.readConfig("infoshowunits");
        setting_showDate = plasmoid.readConfig("infoshowdate");
        display_date.visible = setting_showDate;
    }

    /**
       updateFonts() : function
       Update the display fonts using the config object.
    */
    function updateFonts()
    {
        // Apply font style
        display_value.font.family = plasmoid.readConfig("fontstyle");
        display_date.font.family = plasmoid.readConfig("fontstyle");

        // Set italic
        display_value.font.italic = plasmoid.readConfig("fontitalic");

        // Set bold
        display_value.font.bold = plasmoid.readConfig("fontbold");

        setting_customColor = plasmoid.readConfig("fontusecolor");
        if (setting_customColor)
        {
            // Use custom font color
            display_value.color = plasmoid.readConfig("fontcolor");
            display_date.color = plasmoid.readConfig("fontcolor");

        } else {
            // Use default font color
            display_value.color = theme.textColor;
            display_date.color = theme.textColor;
        }

        setting_showShadow = plasmoid.readConfig("fontshowshadow");
        if (setting_showShadow)
        {
            // Show a shadow behind text
            setting_customShadow = plasmoid.readConfig("fontuseshadow");
            if (setting_customShadow)
            {
                // Use custom shadow color
                display_value.styleColor = plasmoid.readConfig("fontshadow");
                display_date.styleColor = plasmoid.readConfig("fontshadow");

            } else {
                // Use default shadow color
                display_value.styleColor = theme.backgroundColor;
                display_date.styleColor = theme.backgroundColor;
            }

        } else {
            // Use no shadow color (set same as foreground color)
            display_value.styleColor = display_value.color;
            display_date.styleColor = display_value.color;
        }

        var tmp_value = display_value.text; 
        display_value.text = ""; 
        display_value.text = tmp_value; 

        var tmp_date = display_date.text;
        display_date.text = ""; 
        display_date.text = tmp_date;
    }

}
