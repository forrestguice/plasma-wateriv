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
*/

Item
{
    id: main;
  
    width: 200; height: 100;
    property int minimumWidth: 50; property int minimumHeight: 25;

    property string app_name: "Water Watcher";
    property int minEngineVersion: 1;
    property int pollingInterval: 60*60000;
    property string dataRequest: "-1";
    property bool dataRequestIsEmpty: (dataRequest == "-1" || dataRequest == "" || dataRequest == " ");
    property bool dataRequestIsValid: true;

    PlasmaCore.Theme { id: theme; }

    PlasmaCore.Svg { id: arrowSvg; imagePath: "widgets/arrows"; }
    PlasmaCore.Svg { id: lineSvg; imagePath: "widgets/line"; }
    PlasmaCore.Svg { id: configSvg; imagePath: "widgets/configuration-icons"; }

    PlasmaCore.DataSource 
    {
        id: dataengine; engine: "wateriv"; interval: pollingInterval;
        onDataChanged: { refreshDisplay(); }
        Component.onCompleted: 
        { 
            plasmoid.busy = true; loadtimer.start(); 
        }
    }

    onPollingIntervalChanged: { if (pollingInterval < 15) pollingInterval = 15; }

    Timer
    {
        id: loadtimer; interval: 1000; running: false; repeat: false;
        onTriggered: 
        {
            plasmoid.addEventListener("ConfigChanged", configChanged);
            configChanged();
        }
    }

    MainWidget { id: mainWidget; }

    MouseArea
    {
        id: mainMouseArea; anchors.fill: parent;
        onPressAndHold: 
        {
            if (dataRequestIsEmpty) { infodialog.state = "CONFIGURE"; dialog_info.toggleDialog(); }
            else { showNextSeries() }; 
        }
        onClicked: 
        {
            if ((dataRequestIsEmpty || !dataRequestIsValid) && dataengine.valid) infodialog.state = "CONFIGURE";
            dialog_info.toggleDialog()
        }
    }

    PlasmaCore.ToolTip
    {
        id: main_tooltip; target: main;
        image: "timer"; mainText: "Water Watcher"; subText: "(not connected)";
    }

    PlasmaCore.Dialog
    {
        id: dialog_info;
        mainItem: infodialog; visible: false;

        function toggleDialog()
        {
            if (dialog_info.visible == false)
            {
                var popupPosition = dialog_info.popupPosition(main);
                dialog_info.x = popupPosition.x - 15;
                dialog_info.y = popupPosition.y - 15;
            }

            mainItem.focus = !dialog_info.visible;
            dialog_info.visible = !dialog_info.visible;
        }
    }

    InfoDialog
    {
        id: infodialog;
        onNextSeries: { showNextSeries(); }
        onPrevSeries: { showPrevSeries(); }
        onToggleDialog: { dialog_info.toggleDialog(); }
    }

    //////////////////////////////////////
    // functions
    //////////////////////////////////////

    /**
        showNextSeries() : function
        Change the display to the next available timeseries.
    */
    function showNextSeries()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;

        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") return;

        var numSubSeries = results["timeseries_" + mainWidget.displaySeries + "_values_count"];
        if (typeof numSubSeries === "undefined") return;

        mainWidget.displaySubSeries += 1;
        if (mainWidget.displaySubSeries >= numSubSeries) 
        {
            mainWidget.displaySubSeries = 0;
            mainWidget.displaySeries += 1;
            if (mainWidget.displaySeries >= numSeries) mainWidget.displaySeries = 0;
        }

        refreshDisplay();
    }

    /**
        showPrevSeries() : function
        Change the display to the previous available timeseries.
    */
    function showPrevSeries()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;
        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") return;

        var numSubSeries = results["timeseries_" + mainWidget.displaySeries + "_values_count"];
        if (typeof numSubSeries === "undefined") return;

        mainWidget.displaySubSeries -= 1;
        if (mainWidget.displaySubSeries < 0) 
        {
            mainWidget.displaySeries -= 1;
            if (mainWidget.displaySeries < 0) mainWidget.displaySeries = numSeries - 1;

            numSubSeries = results["timeseries_" + mainWidget.displaySeries + "_values_count"];
            mainWidget.displaySubSeries = numSubSeries-1;
        }

        refreshDisplay();
    }

    function determineNavText()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;
        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") return;

        var current = 0; var total = 0;
        for (i=0; i<numSeries; i++)
        {
            var numSubSeries = results["timeseries_" + i + "_values_count"];
            if (typeof numSubSeries === "undefined") return;

            if (i == mainWidget.displaySeries)
            {
                current = total + mainWidget.displaySubSeries + 1;
            }
            total += numSubSeries;
        }

        return "" + current + " / " + total;
    }

    function errorMessage(errorMsg)
    {
        main_tooltip.mainText = app_name;
        mainWidget.displayValue = app_name;
        infodialog.panelRecent.title = app_name; 

        main_tooltip.subText = errorMsg;
        mainWidget.displayDate = errorMsg;
        infodialog.panelRecent.content = errorMsg;
        infodialog.navText = "0/0";
    }

    /**
        Checks for common errors; returns true if the calling function should
        return as a result of errors, or false if it should continue (no error).
    */
    function errorChecking()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined")
        {
            plasmoid.busy = false;   // error: no result from data engine
            return true;             // (probably delayed)
        }

        var engineVersion = results["engine_version"];
        if (typeof engineVersion === "undefined" || engineVersion < minEngineVersion)
        {
            errorMessage("Insufficient data engine:<br/>wateriv >= 0.2.0 required");
            console.log("Water Watcher: requires version_id 1, found version_id " + engineVersion);
            plasmoid.busy = false;
            return true;             // error: insufficient data engine version
        }

        dataRequestIsValid = results["net_request_isvalid"];
        if (!dataRequestIsValid)
        {
            errorMessage("Invalid data source.");
            infodialog.panelConfig.field.state = "ERROR";
            infodialog.panelConfig.error.input.text = results["net_request_error"];
            infodialog.panelConfig.state = "ERROR";
            plasmoid.busy = false;
            return true;
        }
 
        var numSeries = results["timeseries_count"];
        if (typeof numSeries === "undefined") 
        {
            mainWidget.displaySeries = 0;
            mainWidget.displaySubSeries = 0;
            plasmoid.busy = false;
            return true;             // error: no timeseries to display
        }

        var netIsValid = results["net_isvalid"];
        if (typeof netIsValid === "undefined")
        {
            errorMessage("Connection to engine failed.");
            plasmoid.busy = false;
            return true;                      // error: connection to engine failed
        }

        if (netIsValid == false) 
        {
            errorMessage("Network request failed: " + results["net_error"] + ".");
            plasmoid.busy = false;
            return true;                      // error: connection to network failed
        }

        var xmlIsValid = results["xml_isvalid"];
        if (typeof xmlIsValid === "undefined" || xmlIsValid == false)
        {
            errorMessage("Received invalid data.");
            plasmoid.busy = false;
            return true;                      // error: xml errors
        }

        if (mainWidget.displaySeries >= numSeries )
        {
            mainWidget.displaySeries = 0;     // error: series out of bounds
            mainWidget.displaySubSeries = 0;  // corrent and ignore error
        }

        return false;   // end reached - no major errors occured
    }

    /**
        refreshDisplay() : function
        Retrieve data from the connected dataengine and update the display.
    */
    function refreshDisplay()
    {
        if (errorChecking()) return;

        var results = dataengine.data[dataRequest];
        var prefix = "timeseries_" + mainWidget.displaySeries + "_";

        var numSubSeries = results[prefix+"values_count"];
        if (mainWidget.displaySubSeries >= numSubSeries)
        {
            mainWidget.displaySubSeries = 0;
        }
    
        var site_name = results[prefix + "sourceinfo_sitename"];
        var site_code = results[prefix + "sourceinfo_sitecode"];

        var var_code = results[prefix + "variable_code"];
        var var_name = results[prefix + "variable_name"];
        var var_desc = results[prefix + "variable_description"];
        var var_units = results[prefix + "variable_unitcode"];
        var var_date = Qt.formatDateTime(results[prefix + "values_"+mainWidget.displaySubSeries+"_recent_date"]);
        var var_value = results[prefix + "values_"+mainWidget.displaySubSeries+"_recent"];
        var var_method_id = results[prefix + "values_"+mainWidget.displaySubSeries+"_method_id"];
        var var_method_desc = results[prefix + "values_"+mainWidget.displaySubSeries+"_method_description"];

        var qualifiers = results[prefix + "values_"+mainWidget.displaySubSeries+"_qualifiers"];
        var qualifier_code = results[prefix + "values_"+mainWidget.displaySubSeries+"_recent_qualifier"];
        var qualifier_desc = qualifiers[qualifier_code][2];

        // refresh tooltip content
        main_tooltip.mainText = "" + site_name + " (" + site_code + ")";
        main_tooltip.subText = "" + var_name + " (" + var_code + ")<br/><br/><b>" + var_value + " " 
                               + var_units + "</b><br/>" + var_date + "<br/><br/>"
                               + qualifier_desc;

        // refresh main widget
        mainWidget.displayValue = "" + var_value;
        mainWidget.displayUnits = "" + var_units;
        mainWidget.displayDate = "" + var_date;

        // refresh info dialog
        infodialog.panelRecent.title = site_name + " (" + site_code + ")";
        infodialog.panelRecent.content = "" + var_name + " (" + var_code + ")<br/>"
                                + var_method_desc + " (method " + var_method_id + ")<br/><br/>" 
                                + "<b>" + var_value + " " + var_units + "</b><br/>"
                                + var_date + "<br/><br/>"
                                +  qualifier_desc;
        infodialog.navText = determineNavText();

        plasmoid.busy = false;
    }

    /** 
       configChanged() : function
       The configuration object was changed; update properties and reconnect to
       the dataengine if the request was changed.
    */
    function configChanged()
    {
        plasmoid.busy = true;
        updateFonts();
        mainWidget.showUnits = plasmoid.readConfig("infoshowunits");
        mainWidget.showDate = plasmoid.readConfig("infoshowdate");
        updateEngine();    // bug: source never connects unless we do this twice
        updateEngine();    // ... what is going on here.
    }

    function updateEngine()
    {
        if (!dataengine.valid)
        {
            errorMessage("Missing data engine:<br/>wateriv >= 0.2.0 required");
            plasmoid.busy = false;
            return;
        }

        pollingInterval = plasmoid.readConfig("datapolling");
        if (!dataRequestIsEmpty) dataengine.disconnectSource(dataRequest);

        dataRequest = plasmoid.readConfig("datasource");
        if (dataRequestIsEmpty)
        {
            errorMessage("Configuration Required");
            plasmoid.busy = false;
            return;
        }

        dataengine.connectSource(dataRequest);
    }

    /**
       updateFonts() : function
       Update the display fonts using the config object.
    */
    function updateFonts()
    {
        mainWidget.fontStyle = plasmoid.readConfig("fontstyle");    // style
        mainWidget.fontItalic = plasmoid.readConfig("fontitalic");  // italic
        mainWidget.fontBold = plasmoid.readConfig("fontbold");      // bold

        mainWidget.showCustomColor = plasmoid.readConfig("fontusecolor");
        if (!mainWidget.showCustomColor) mainWidget.fontColor = theme.textColor;
        else mainWidget.fontColor = plasmoid.readConfig("fontcolor");

        mainWidget.showShadow = plasmoid.readConfig("fontshowshadow");
        if (mainWidget.showShadow)
        {
            mainWidget.showCustomShadow = plasmoid.readConfig("fontuseshadow");
            if (mainWidget.showCustomShadow)                   // custom shadow
            {
                mainWidget.fontShadow = plasmoid.readConfig("fontshadow");
            } else {
                mainWidget.fontShadow = theme.backgroundColor; // default shadow
            }
        } else {
            mainWidget.fontShadow = mainWidget.fontColor;       // no shadow
        }

        mainWidget.triggerFontResize();
    }

}
