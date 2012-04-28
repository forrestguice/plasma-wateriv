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
    Water Watcher QML Plasmoid (http://code.google.com/p/plasma-wateriv/)  
*/
Item
{
    id: main; 
    width: 200; height: 100;
    property int minimumWidth: 50; 
    property int minimumHeight: 25;

    property string app_name: "Water Watcher";
    property string minEngineName: "wateriv >= 0.3.0";
    property int minEngineVersion: 3;
    property variant currentDialog: -1;

    property string dataRequest: "-1";
    property bool dataRequestIsValid: true;
    property bool dataRequestIsEmpty: (dataRequest == "-1" || dataRequest == "" || dataRequest == " ");

    property int pollingInterval: 30;   // interval in minutes
    onPollingIntervalChanged: { if (pollingInterval < 15) pollingInterval = 15; }

    Component.onCompleted: { plasmoid.busy = true; }

    PlasmaCore.DataSource 
    {
        id: dataengine; engine: "wateriv"; 
        property bool virgin: true;             // hack: has yet to connect flag
        interval: pollingInterval * 60000;
        onDataChanged: { refreshDisplay(); }
        //onIntervalChanged: { console.log("engine: interval set: " + interval); }
        Component.onCompleted: { loadtimer.start(); }

        function makeConnection()
        {
            if (dataengine.virgin)
            {   // hack: on first time connect then disconnect then connect
                // otherwise the plasmoid never receives the first update
                dataengine.connectSource(dataRequest);
                dataengine.disconnectSource(dataRequest);
            }
            connecttimer.start();
        }
    }

    Timer
    {
        id: loadtimer; interval: 1000; running: false; repeat: false;
        onTriggered: 
        {
            plasmoid.addEventListener("Activate", main.activate);
            plasmoid.addEventListener("ConfigChanged", main.configChanged);
            main.configChanged();
        }
    }

    Timer
    {
        id: connecttimer; interval: 1000; running: false; repeat: false;
        onTriggered: { dataengine.connectSource(dataRequest); }
    }

    MouseArea
    {
        id: mainMouseArea; anchors.fill: parent;
        onPressAndHold: 
        {
            if ((dataRequestIsEmpty || !dataRequestIsValid) && dataengine.valid)
            { main.activate(); }
            else { main.showNextSeries() }; 
        }
        onClicked: { main.activate(); }
    }

    PlasmaCore.Theme { id: theme; }
    PlasmaCore.Svg { id: arrowSvg; imagePath: "widgets/arrows"; }
    PlasmaCore.Svg { id: lineSvg; imagePath: "widgets/line"; }
    PlasmaCore.Svg { id: configSvg; imagePath: "widgets/configuration-icons"; }
    PlasmaCore.Svg { id: netSvg; imagePath: "icons/network"; }

    PlasmaCore.ToolTip
    {
        id: main_tooltip; target: main;
        image: "chronometer"; mainText: main.app_name; subText: i18n("(not connected)");
    }

    PlasmaCore.Dialog
    {
        id: dialog_info; mainItem: infodialog; 
        visible: false;
    }
    InfoDialog
    {
        id: infodialog;
        onNextSeries: { main.showNextSeries(); }
        onPrevSeries: { main.showPrevSeries(); }
        onToggleDialog: { main.toggleDialog(dialog_info, main); }
        onConfigChanged: { main.configChanged(); }
    }

    PlasmaCore.Dialog
    {
        id: dialog_net; mainItem: netdialog; 
        visible: false;
    }
    NetErrorPanel
    {
        id: netdialog;
    }

    PlasmaCore.SvgItem
    {
        id: netErrorIndicator; svg: netSvg; elementId: "network-wired";
        width: (24 * main.width) / 200; height: (24 * main.height) / 100;
        anchors.top: main.top; anchors.right: main.right;
        visible: false;

        MouseArea 
        {
            id: netErrorMouseArea; anchors.fill: parent;
            onClicked: { main.toggleDialog(dialog_net, netErrorIndicator); }
        }
    }

    MainWidget { id: mainWidget; }

    //////////////////////////////////////
    // functions
    //////////////////////////////////////

    /**
        activate() : function
        Called when the plasmoid is clicked or activated with the keyboard shortcut.
    */
    function activate()
    {
        if ((dataRequestIsEmpty || !dataRequestIsValid) && dataengine.valid) 
        { main.toggleDialogState(dialog_info, mainWidget, "CONFIGURE"); }
        else { main.toggleDialog(dialog_info, mainWidget); }
    }

    /**
        toggleDialog( dialog, popupTarget) : function   
        Toggle an existing dialog (PlasmaCore.Dialog id) at popupTarget (id).
    */
    function toggleDialog( dialog, popupTarget )
    {
        if (dialog.visible) hideDialog(dialog, popupTarget);
        else showDialog(dialog, popupTarget, -1);
    }
    function toggleDialogState( dialog, popupTarget, dialogState)
    {
        if (dialog.visible) hideDialog(dialog);
        else showDialog(dialog, popupTarget, dialogState);
    }

    /**
        showDialog( dialog, popupTarget) : function   
        Show an existing dialog (PlasmaCore.Dialog id) at popupTarget (id).
    */
    function showDialog( dialog, popupTarget, dialogState )
    {
        if (main.currentDialog != -1) 
        {   // can not show - a dialog is already shown - hide it
            main.hideDialog(main.currentDialog); 
            return; 
        }
        main.currentDialog = dialog;
        var popupPosition = dialog.popupPosition(popupTarget);
        dialog.x = popupPosition.x - 15;                             // x pos
        dialog.y = popupPosition.y - 15;                             // y pos
        if (dialogState != -1) dialog.mainItem.state = dialogState;  // state
        dialog.mainItem.focus = true                                 // focus
        dialog.visible = true;                                       // visible
    }

    /**
        hideDialog( dialog, popupTarget) : function   
        Hide an existing dialog (PlasmaCore.Dialog id).
    */
    function hideDialog( dialog )
    {
        dialog.mainItem.focus = false
        dialog.visible = false;
        if (dialog == main.currentDialog) main.currentDialog = -1;
    }

    /**
        showNextSeries() : function
        Change the display to the next available timeseries.
    */
    function showNextSeries()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;
        var numSeries = results["toc_count"];
        if (typeof numSeries === "undefined") return;

        mainWidget.displaySeries += 1;
        if (mainWidget.displaySeries >= numSeries) mainWidget.displaySeries = 0;
        main.refreshDisplay();
    }

    /**
        showPrevSeries() : function
        Change the display to the previous available timeseries.
    */
    function showPrevSeries()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;
        var numSeries = results["toc_count"];
        if (typeof numSeries === "undefined") return;

        mainWidget.displaySeries -= 1;
        if (mainWidget.displaySeries < 0) mainWidget.displaySeries = numSeries - 1;
        main.refreshDisplay();
    }

    function determineNavText()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined") return;
        var numSeries = results["toc_count"];
        if (typeof numSeries === "undefined") return;

        var current = 0;
        for (i=0; i<numSeries; i++)
        {
            if (i == mainWidget.displaySeries)
            {
                current = i + 1;
                break;
            }
        }

        return "" + current + " / " + numSeries;
    }

    function errorMessage(errorMsg)
    {
        main_tooltip.mainText = app_name;
        mainWidget.state = "ERROR";
        mainWidget.displayValue = app_name;
        infodialog.panelRecent.title = app_name; 

        main_tooltip.subText = errorMsg;
        mainWidget.displayDate = errorMsg;
        infodialog.panelRecent.content = errorMsg;
        infodialog.navText = "-/-";
    }

    /**
        Checks for common errors; returns true if the calling function should
        return as a result of errors, or false if it should continue (no error).
        @return true no errors, false errors
    */
    function errorChecking()
    {
        var results = dataengine.data[dataRequest];
        if (typeof results === "undefined")
        {
            //console.log("error: results not ready..leaving for now.");
            return false;             // error: no result (probably delayed)
        }

        var engineVersion = results["engine_version"];
        if (typeof engineVersion === "undefined" || engineVersion < minEngineVersion)
        {
            console.log(i18n("Water Watcher: requires version_id 2, found version_id ") + engineVersion);
            errorMessage(i18n("Insufficient data engine:<br/>") + minEngineName + i18n(" required"));
            return false;             // error: insufficient data engine version
        }

        dataRequestIsValid = results["net_request_isvalid"];
        if (!dataRequestIsValid)
        {
            //console.log("error: invalid data source.");
            errorMessage(i18n("Invalid data source."));
            infodialog.panelConfig.field.state = "ERROR";
            infodialog.panelConfig.error.input.text = results["net_request_error"];
            infodialog.panelConfig.state = "ERROR";
            return false;
        }

        var netIsValid = results["net_isvalid"];
        if (typeof netIsValid === "undefined")
        {
            //console.log("error: no net_isvalid key");
            errorMessage("");
            return false;
        }

        var numSeries = results["toc_count"];
        if (typeof numSeries === "undefined") 
        {
            console.log("error: no toc_count key");
            mainWidget.displaySeries = 0;
            return false;             // error: no timeseries to display
        }
 
        netErrorIndicator.visible = !netIsValid;
        if (netIsValid == false) 
        {
            var netError = results["net_error"];
            if (typeof netError == "number") netdialog.error = netError;

            if (!netIsValid && numSeries <= 0)
            {
                errorMessage(i18n("Network request failed: ") + results["net_error"] + "  ");
                return false;                     // error: connection to network failed
            }
        }

        var xmlIsValid = results["xml_isvalid"];
        if (typeof xmlIsValid === "undefined" || xmlIsValid == false)
        {
            //console.log("error: missing xml_isvalid key or invalid xml");
            errorMessage(i18n("Received invalid data."));
            return false;                      // error: xml errors
        }

        if (numSeries <= 0)
        {
            console.log("warning: no series to display");
            errorMessage(i18n("No data for this site."));
            return false;                     // error: no series to display
        }

        if (mainWidget.displaySeries >= numSeries || mainWidget.displaySeries < 0)
        {
            console.log("warning: current series out of bounds, adjusting");
            mainWidget.displaySeries = 0;     // error: series out of bounds
        }

        return true;   // end reached - no major errors occured
    }

    /**
        refreshDisplay() : function
        Retrieve data from the connected dataengine and update the display.
    */
    function refreshDisplay()
    {
        //console.log("refreshDisplay (entered)");
        if ((plasmoid.busy = errorChecking()) == false) return;
        else mainWidget.state = "NORMAL";

        //
        // Gather Data
        //
        var results = dataengine.data[dataRequest];
        var toc = results["toc_" + mainWidget.displaySeries];

        if (typeof toc === "undefined") 
        {
            console.log("missing toc! " + mainWidget.displaySeries);
            plasmoid.busy = false;
            return;
        }

        var prefix_site = "series_" + toc["site"] + "_";
        var site_name = results[prefix_site + "name"];
        var site_code = results[prefix_site + "code"];
        var site_lat = results[prefix_site + "latitude"];
        var site_lon = results[prefix_site + "longitude"];
        var site_properties = results[prefix_site + "properties"];
        var site_hucCd = site_properties["hucCd"];
        var site_countyCd = site_properties["countyCd"];
        var site_stateCd = site_properties["stateCd"];
        var site_typeCd = site_properties["siteTypeCd"];

        var prefix_var = prefix_site + toc["variable"] + "_";
        var var_code = results[prefix_var + "code"];
        var var_name = results[prefix_var + "name"];
        var var_desc = results[prefix_var + "description"];
        var var_units = results[prefix_var + "unitcode"];

        var prefix_stat = prefix_var + toc["statistic"] + "_";
        var var_statName = results[prefix_stat + "name"];
        var var_statCode = results[prefix_stat + "code"];

        var prefix_method = prefix_stat + toc["method"] + "_";
        var var_date = Qt.formatDateTime(results[prefix_method + "recent_date"]);
        var var_value = results[prefix_method + "recent_value"];
        var var_method_id = results[prefix_method + "id"];
        var var_method_desc = results[prefix_method + "description"];
        var var_all = results[prefix_method + "all"];

        var qualifiers = results[prefix_method + "qualifiers"];
        var qualifier_codes = results[prefix_method + "recent_qualifier"];

        var qualifier_desc = "";
        for (qcode in qualifier_codes)
        {
            qualifier_desc += "<br/>" + qualifiers[qualifier_codes[qcode]][2];
        }

        //
        // Display Data
        //

        main_tooltip.mainText = "" + site_name + " (" + site_code + ")";
        main_tooltip.subText = "" + var_name + " (" + var_code + ")<br/><br/><b>" + var_value + " " 
                               + var_units + "</b><br/>" + var_date + "<br/>"
                               + qualifier_desc;      // refresh tooltip content

        // refresh main widget
        mainWidget.displayValue = "" + var_value;
        mainWidget.displayUnits = "" + var_units;
        mainWidget.displayDate = "" + var_date;

        // refresh info dialog
        var value_table = main.createValueTable(var_all, var_units);
        infodialog.panelRecent.title = site_name;
        infodialog.panelRecent.content = var_name + " (" + var_code + ")<br/>"
                                + var_method_desc + " (method " + var_method_id + ")<br/>" 
                                + value_table + "<br/>" + qualifier_desc;
        infodialog.navText = main.determineNavText();
        infodialog.panelRecent.siteContent = "<table>" + 
                                             "<tr><td><b>ID:</b>&nbsp;&nbsp;" + site_code + "</td>" +
                                                 "<td>&nbsp;&nbsp;&nbsp;</td><td><b>Type:</b>&nbsp;&nbsp;" + site_typeCd + "</td></tr>" +
                                             "<tr><td><b>Latitude:</b>&nbsp;&nbsp;" + site_lat + "</td><td>&nbsp;&nbsp;&nbsp;</td>" + 
                                                 "<td><b>Longitude:</b>&nbsp;&nbsp;" + site_lon + "</td></tr>" +
                                             "<tr><td><b>State Code:</b>&nbsp;&nbsp;" + site_stateCd + "</td><td>&nbsp;&nbsp;&nbsp;</td>" + 
                                                 "<td><b>County Code:</b>&nbsp;&nbsp;" + site_countyCd + "</td></tr>" +
                                             "<tr><td><b>HUC Code:</b>&nbsp;&nbsp;" + site_hucCd + "</td><td>&nbsp;&nbsp;&nbsp;</td>" + 
                                                 "<td></td></tr></table>";
        plasmoid.busy = false;
    }

    /**
        createValueTable() : function
        Create a <table> of values.
    */
    function createValueTable( values, var_units )
    {
        var keys = [];
        for (var key in values)
        {
            if (values.hasOwnProperty(key)) keys.push(key);
        }
        keys.sort();

        var c = 0;
        var table = "<table>";
        for (var i = keys.length - 1; i >= 0; i--)
        {
            if (c >= 4) break;
            var k = keys[i];
            var d = new Date(k);
            var v = values[k];
            table += "<tr><td width='50'><b>" + v[0] + " " + var_units + "</b></td><td>" + Qt.formatDateTime(d) + "</td></tr>";
            c++;
        }
        table += "</table>";
        return table;
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
        updateShown();
        plasmoid.busy = updateEngine();
    }

    /**
        updateEngine() : function
        Checks that the engine is available, disconnects currently connected
        source, sets the polling interval from config, sets the source from 
        config, and then attempts to connect.
    */
    function updateEngine()
    {
        if (!dataengine.valid)      // is the engine available?
        {
            errorMessage(i18n("Missing data engine:<br/>") + minEngineName + i18n(" required"));
            return false;           // missing engine; abort engine update
        }

        if (!dataRequestIsEmpty) 
        {
            //console.log("disconnecting source: " + dataRequest);
            dataengine.disconnectSource(dataRequest);
        }

        pollingInterval = plasmoid.readConfig("datapolling");
        dataRequest = plasmoid.readConfig("datasource");

        if (dataRequestIsEmpty)
        {
            errorMessage(i18n("Configuration Required"));
            return false;
        }

        dataengine.makeConnection();
        return true;
    }

    /**
       updateShown() : function
       Update the information shown in the main widget.
    */
    function updateShown()
    {
        mainWidget.showUnits = plasmoid.readConfig("infoshowunits");
        mainWidget.showDate = plasmoid.readConfig("infoshowdate");

        infodialog.panelRecent.sitePanelCollapsed = plasmoid.readConfig("sitepanel_collapsed", true);
        infodialog.panelConfig.searchPanel.collapsed = plasmoid.readConfig("searchpanel_collapsed", true);
    }

    /**
       updateFonts() : function
       Update the display fonts using the config object.
    */
    function updateFonts()
    {
        mainWidget.fontScaleValue = plasmoid.readConfig("fontscalevalue");
        mainWidget.fontScaleDate = plasmoid.readConfig("fontscaledate");

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
