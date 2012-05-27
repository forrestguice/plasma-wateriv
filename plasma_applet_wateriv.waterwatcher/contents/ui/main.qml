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
import "plasmapackage:/code/neterrors.js" as NetErrors
import "plasmapackage:/code/display.js" as Display

/**  
    Water Watcher QML Plasmoid (http://code.google.com/p/plasma-wateriv/)  
*/
Item
{
    property string app_name: "Water Watcher";
    property int minEngineVersion: 4;
    property string minEngineName: "wateriv >= 0.3.1";

    id: main; 
  
    width: 200; height: 100;
    property int minimumWidth: 50; 
    property int minimumHeight: 25;

    property variant currentDialog: -1;

    property int displaySeries: 0;
    property string dataRequest: "-1";
    property bool dataRequestIsValid: true;
    property bool dataRequestIsEmpty: (dataRequest == "-1" || dataRequest == "" || dataRequest == " ");

    property int pollingInterval: 30;   // interval in minutes
    onPollingIntervalChanged: { if (pollingInterval < 15) pollingInterval = 15; }

    Component.onCompleted: 
    { 
        plasmoid.busy = true; 
        loadtimer.start();
    }

    PlasmaCore.Theme { id: theme; }
    PlasmaCore.Svg { id: arrowSvg; imagePath: "widgets/arrows"; }
    PlasmaCore.Svg { id: lineSvg; imagePath: "widgets/line"; }
    PlasmaCore.Svg { id: configSvg; imagePath: "widgets/configuration-icons"; }
    PlasmaCore.Svg { id: netSvg; imagePath: "icons/network"; }

    PlasmaCore.DataSource 
    {
        id: dataengine; engine: "wateriv"; 
        interval: pollingInterval * 60000;
        onDataChanged: { refreshDisplay(); }
    }

    Timer
    {
        id: loadtimer; interval: 1500; running: false; repeat: false;
        onTriggered: 
        {
            plasmoid.addEventListener("Activate", main.activate);
            plasmoid.addEventListener("ConfigChanged", main.configChanged);
            configChanged();
        }
    }

    Timer
    {
        id: connecttimer; interval: 1000; running: false; repeat: false;
        onTriggered: { dataengine.connectedSources = [ dataRequest ]; }
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
        dialog.x = popupPosition.x;                                  // x pos
        dialog.y = popupPosition.y;                                  // y pos
        dialog.mainItem.focus = true                                 // focus
        dialog.visible = true;                                       // visible
        if (dialogState != -1) dialog.mainItem.state = dialogState;  // state
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

        main.displaySeries += 1;
        if (main.displaySeries >= numSeries) main.displaySeries = 0;
        main.refreshDisplay();
        plasmoid.writeConfig("tocindex", main.displaySeries);
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

        main.displaySeries -= 1;
        if (main.displaySeries < 0) main.displaySeries = numSeries - 1;
        main.refreshDisplay();
        plasmoid.writeConfig("tocindex", main.displaySeries);
    }

    function errorMessage(errorMsg)
    {
        main_tooltip.mainText = app_name;
        mainWidget.state = "ERROR";
        mainWidget.displayValue = app_name;
        infodialog.panelRecent.title = app_name; 

        main_tooltip.subText = errorMsg;
        mainWidget.displayDate = errorMsg;
        infodialog.panelRecent.content = errorMsg + "<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>&nbsp;";
        infodialog.panelRecent.siteContent = "";
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
            console.log(i18n("Water Watcher: requires " + minEngineVersion + ", found ") + engineVersion);
            errorMessage(i18n("Insufficient data engine:<br/>%0 required").format(minEngineName));
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
            main.displaySeries = 0;
            return false;             // error: no timeseries to display
        }
 
        netErrorIndicator.visible = !netIsValid;
        if (netIsValid == false) 
        {
            var netError = results["net_error"];
            if (typeof netError == "number") netdialog.error = netError;

            if (!netIsValid && numSeries <= 0)
            {
                //errorMessage(i18n("Network request failed: ") + results["net_error"] + "  ");
                errorMessage(i18n("Error: %0").format(NetErrors.nameForErrorCode(results["net_error"])));
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

        if (main.displaySeries >= numSeries || main.displaySeries < 0)
        {
            console.log("warning: current series out of bounds, adjusting");
            main.displaySeries = 0;     // error: series out of bounds
            plasmoid.writeConfig("tocindex", 0);
        }

        return true;   // end reached - no major errors occured
    }

    /**
        refreshDisplay() : function
        Retrieve data from the connected dataengine and update the display.
    */
    function refreshDisplay()
    {
        if ((plasmoid.busy = errorChecking()) == false) return;
        else mainWidget.state = "NORMAL";

        //
        // Gather Data
        //
        var results = dataengine.data[dataRequest];
        var site = new Object();   // obj to hold site data
        var param = new Object();  // obj to hold param data

        var toc_count = results["toc_count"];
        var toc = results["toc_" + main.displaySeries];
        if (typeof toc === "undefined")
        {
            console.log("missing toc! " + main.displaySeries);
            plasmoid.busy = false;
            return;
        }

        var prefix_site = "series_" + toc["site"] + "_";
        var prefix_var = prefix_site + toc["variable"] + "_";
        var prefix_stat = prefix_var + toc["statistic"] + "_";
        var prefix_method = prefix_stat + toc["method"] + "_";

        site.name = results[prefix_site + "name"];
        site.code = results[prefix_site + "code"];
        site.lat = results[prefix_site + "latitude"];
        site.lon = results[prefix_site + "longitude"];
        site.properties = results[prefix_site + "properties"];
        site.hucCd = site.properties["hucCd"];
        site.countyCd = site.properties["countyCd"];
        site.stateCd = site.properties["stateCd"];
        site.typeCd = site.properties["siteTypeCd"];

        param.code = results[prefix_var + "code"];
        param.name = results[prefix_var + "name"];
        param.desc = results[prefix_var + "description"];
        param.units = results[prefix_var + "unitcode"];

        param.statName = results[prefix_stat + "name"];
        param.statCode = results[prefix_stat + "code"];

        param.date = Qt.formatDateTime(results[prefix_method + "recent_date"]);
        param.value = results[prefix_method + "recent_value"];
        param.method_id = results[prefix_method + "id"];
        param.method_desc = results[prefix_method + "description"];
        param.all = results[prefix_method + "all"];
        param.qualifiers = results[prefix_method + "qualifiers"];
        param.qualifier_codes = results[prefix_method + "recent_qualifier"];
        param.qualifier_desc = Display.createQualifierList(param.qualifier_codes, param.qualifiers);

        //
        // Display Data
        //

        // refresh main widget
        mainWidget.displayValue = "" + param.value;
        mainWidget.displayUnits = "" + param.units;
        mainWidget.displayDate = "" + param.date;

        // refresh tooltip
        main_tooltip.mainText = Display.createTooltipTitle(site, param);
        main_tooltip.subText = Display.createTooltipText(site, param);

        // refresh info dialog
        infodialog.navText = Display.createNavigationText(main.displaySeries, toc_count);
        infodialog.panelRecent.title = Display.createRecentPanelTitle(site, param);
        infodialog.panelRecent.content = Display.createRecentPanelText(site, param);
        infodialog.panelRecent.siteContent = Display.createSitePanelText(site);

        plasmoid.busy = false;
    }

    /** 
       configChanged() : function
       The configuration object was changed; update properties and reconnect to
       the dataengine if the request was changed.
    */
    function configChanged()
    {
        //console.log("configChanged");
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
            errorMessage(i18n("Missing data engine:<br/>%0 required").format(minEngineName));
            return false;           // missing engine; abort engine update
        }

        if (!dataRequestIsEmpty) 
        {
            dataengine.connectedSources = [];
        }

        dataRequest = plasmoid.readConfig("datasource");
        pollingInterval = plasmoid.readConfig("datapolling");
        displaySeries = plasmoid.readConfig("tocindex");

        if (dataRequestIsEmpty)
        {
            errorMessage(i18n("Configuration Required"));
            return false;
        }

        connecttimer.start();
        return true;
    }

    /**
        retryConnection() : function
    */
    function retryConnection()
    {
        plasmoid.busy = true;
        dataengine.connectedSources = [];
        connecttimer.start();
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
