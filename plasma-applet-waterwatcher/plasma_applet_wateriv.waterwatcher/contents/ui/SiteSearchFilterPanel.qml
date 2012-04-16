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
    id: filterpanel; anchors.verticalCenter: parent.center; spacing: 2;
    height: anchors.bottomMargin + filterRow1.height + filterRow2.height;

    property string filters: "countyCd=" + countySource.filter + countySource.selectedCode 
                             + "&agencyCd=" + agencySource.selectedCode + "&siteStatus=active"
                             + "&dataType=iv" + minorTypeSource.siteQueryFilter;
    property variant currentDialog: -1;
    property variant currentTarget: -1;

    Row
    {
        id: filterRow1;
        anchors.left: parent.left; spacing: 5;
        Text
        {
            text: i18n("State:"); anchors.verticalCenter: parent.verticalCenter;
        }
        TextButton
        {
            id: stateFilter; buttonText: stateSource.selectedLabel;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { filterpanel.toggleDialog(dialogSelectState, stateFilter); }
        }
        Rectangle { color: "transparent"; width: 25; height: 5; }
        Text
        {
            text: i18n("County:"); anchors.verticalCenter: parent.verticalCenter;
        }
        TextButton
        {
            id: countyFilter; buttonText: countySource.selectedLabel;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { filterpanel.toggleDialog(dialogSelectCounty, countyFilter); }
        }
    }
     
    Row 
    {
        id: filterRow2; anchors.left: parent.left; spacing: 5;
        Text
        {
            text: i18n("Agency:"); anchors.verticalCenter: parent.verticalCenter;
        }
        TextButton
        {
            id: agencyFilter; buttonText: agencySource.selectedLabel;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { filterpanel.toggleDialog(dialogSelectAgency, agencyFilter); }
        }
        Rectangle { color: "transparent"; width: 25; height: 5; }
        Text
        {
            text: i18n("Type:"); anchors.verticalCenter: parent.verticalCenter;
        }
        TextButton
        {
            id: majorTypeFilter; buttonText: majorTypeSource.selectedLabel;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { filterpanel.toggleDialog(dialogSelectMajorType, majorTypeFilter); }
        }
        Rectangle { color: "transparent"; width: 25; height: 5; }
        Text
        {
            text: i18n("Subtype:"); anchors.verticalCenter: parent.verticalCenter;
        }
        TextButton
        {
            id: minorTypeFilter; buttonText: minorTypeSource.selectedLabel;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: { filterpanel.toggleDialog(dialogSelectMinorType, minorTypeFilter); }
        }
    }

    //
    // State Selector Dialog
    //
    PlasmaCore.Dialog
    {
        id: dialogSelectState; mainItem: selectStateDialog;
        visible: false;
    }
    PlasmaCore.DataSource
    {
        id: stateSource; engine: "watersites"; interval: 0;
        connectedSources: ["statecodes?"];
        onDataChanged: { filterpanel.updateStateDisplay(); }
        property string selectedNumeric: "";
        property string selectedAlpha: "";
        property string selectedLabel: "";
    }
    Timer
    {
        id: stateListTimer; interval: 500; running: false; repeat: false;
        onTriggered:
        {
            stateSelector.list.currentIndex = plasmoid.readConfig("stateindex", -1);
            stateSelector.selectedIndex = stateSelector.list.currentIndex;
        }
    }
    Item 
    {
        id: selectStateDialog; width: 300; height: 300
        SiteSearchFilterList 
        {
            id: stateSelector; anchors.fill: parent;
            onSelectedIndexChanged:
            {
                var obj = stateSelector.model.get(stateSelector.selectedIndex);
                if (typeof obj === "object")
                {
                    stateSource.selectedLabel = obj.code;
                    stateSource.selectedAlpha = obj.code;
                    stateSource.selectedNumeric = obj.numeric;
                    agencySource.filter = "us&" + stateSource.selectedAlpha;
                    agencySource.connectedSources = ["agencycodes?" + agencySource.filter];
                    countySource.connectedSources = ["countycodes?" + countySource.filter];
                    plasmoid.writeConfig("stateindex", stateSelector.selectedIndex);
                } else { stateSelector.selectedIndex = 0; }
            }
            onAction: { stateSelector.selectedIndex = stateSelector.list.currentIndex; filterpanel.hideDialog(dialogSelectState, stateFilter); }
        }
        Keys.onDownPressed: { stateSelector.list.currentIndex += (stateSelector.list.currentIndex >= (stateSelector.list.count-1) ? 0 : 1); }
        Keys.onUpPressed: { stateSelector.list.currentIndex -= (stateSelector.list.currentIndex <= 0 ? 0 : 1); }
        Keys.onEscapePressed: { stateSelector.list.currentIndex = stateSelector.selectedIndex; filterpanel.toggleDialog(dialogSelectState, stateFilter); }
        Keys.onReturnPressed: { stateSelector.selectedIndex = stateSelector.list.currentIndex; filterpanel.toggleDialog(dialogSelectState, stateFilter); }
    }
 
    //
    // Agency Selector Dialog
    //
    PlasmaCore.Dialog
    {
        id: dialogSelectAgency; mainItem: selectAgencyDialog; visible: false;
    }
    PlasmaCore.DataSource
    {
        id: agencySource; engine: "watersites"; interval: 0;
        onDataChanged: { filterpanel.updateAgencyDisplay(); }
        property string filter: "";
        property string selectedCode: "";
        property string selectedLabel: "";
    }
    Item
    {
        id: selectAgencyDialog; width: 300; height: 350;

        SiteSearchFilterList
        {
            id: agencySelector; anchors.fill: parent;
            onCurrentIndexChanged:
            {
                var obj = agencySelector.model.get(agencySelector.list.currentIndex);
                if (typeof obj === "object")
                {
                    agencySource.selectedLabel = obj.code;
                    agencySource.selectedCode = obj.code;
                } else { agencySelector.list.currentIndex = 0; }
            }
            onAction: { filterpanel.hideDialog(dialogSelectAgency, agencyFilter); }
        }

        Keys.onDownPressed: { agencySelector.list.currentIndex += (agencySelector.list.currentIndex >= (agencySelector.list.count-1) ? 0 : 1); }
        Keys.onUpPressed: { agencySelector.list.currentIndex -= (agencySelector.list.currentIndex <= 0 ? 0 : 1); }
        Keys.onEscapePressed: { filterpanel.toggleDialog(dialogSelectAgency, agencyFilter); }
        Keys.onReturnPressed: { filterpanel.toggleDialog(dialogSelectAgency, agencyFilter); }
    }

    //
    // County Selector Dialog
    //
    PlasmaCore.Dialog
    {
        id: dialogSelectCounty; mainItem: selectCountyDialog; visible: false;
    }
    PlasmaCore.DataSource
    {
        id: countySource; engine: "watersites"; interval: 0;
        onDataChanged: { filterpanel.updateCountyDisplay(); }
        property string filter: stateSource.selectedNumeric; 
        property string selectedCode: "";
        property string selectedLabel: "";
    }
    Item
    {
        id: selectCountyDialog; width: 300; height: 350;
        SiteSearchFilterList 
        {
            id: countySelector; anchors.fill: parent; singleColumn: true;
            onCurrentIndexChanged:
            {
                var obj = countySelector.model.get(countySelector.list.currentIndex);
                if (typeof obj === "object")
                {
                    countySource.selectedLabel = obj.name;
                    countySource.selectedCode = obj.code;
                } else { countySelector.list.currentIndex = 0; }
            }
            onAction: { filterpanel.hideDialog(dialogSelectCounty, countyFilter); }
        }
        Keys.onDownPressed: { countySelector.list.currentIndex += (countySelector.list.currentIndex >= (countySelector.list.count-1) ? 0 : 1); }
        Keys.onUpPressed: { countySelector.list.currentIndex -= (countySelector.list.currentIndex <= 0 ? 0 : 1); }
        Keys.onEscapePressed: { filterpanel.toggleDialog(dialogSelectCounty, countyFilter); }
        Keys.onReturnPressed: { filterpanel.toggleDialog(dialogSelectCounty, countyFilter); }
    }

    //
    // Major Type Selector Dialog
    //
    PlasmaCore.Dialog
    {
        id: dialogSelectMajorType; mainItem: selectMajorTypeDialog;
        visible: false;
    }
    PlasmaCore.DataSource
    {
        id: majorTypeSource; engine: "watersites"; interval: 0;
        connectedSources: ["sitetypecodes?major"];
        onDataChanged: { filterpanel.updateMajorTypeDisplay(); }
        property string filter: "";
        property string selectedCode: "";
        property string selectedLabel: "";
    }
    Item
    {
        id: selectMajorTypeDialog; width: 300; height: 350;

        SiteSearchFilterList
        {
            id: majorTypeSelector; anchors.fill: parent;
            onSelectedIndexChanged:
            {
                var obj = majorTypeSelector.model.get(majorTypeSelector.list.currentIndex);
                if (typeof obj === "object")
                {
                    majorTypeSource.selectedLabel = obj.code;
                    majorTypeSource.selectedCode = obj.code;
                    minorTypeSource.filter =  (obj.code == "All") ? "none" : obj.code;
                    minorTypeSource.connectedSources = ["sitetypecodes?" + minorTypeSource.filter];
                } else { majorTypeSelector.list.currentIndex = 0; }
            }
            onAction: { majorTypeSelector.selectedIndex = majorTypeSelector.list.currentIndex; filterpanel.hideDialog(dialogSelectMajorType, majorTypeFilter); }
        }

        Keys.onDownPressed: { majorTypeSelector.list.currentIndex += (majorTypeSelector.list.currentIndex >= (majorTypeSelector.list.count-1) ? 0 : 1); }
        Keys.onUpPressed: { majorTypeSelector.list.currentIndex -= (majorTypeSelector.list.currentIndex <= 0 ? 0 : 1); }
        Keys.onEscapePressed: { filterpanel.toggleDialog(dialogSelectMajorType, majorTypeFilter); }
        Keys.onReturnPressed: { majorTypeSelector.selectedIndex = majorTypeSelector.list.currentIndex; filterpanel.toggleDialog(dialogSelectMajorType, majorTypeFilter); }
    }

    //
    // Minor Type Selector Dialog
    //
    PlasmaCore.Dialog
    {
        id: dialogSelectMinorType; mainItem: selectMinorTypeDialog;
        visible: false;
    }
    PlasmaCore.DataSource
    {
        id: minorTypeSource; engine: "watersites"; interval: 0;
        onDataChanged: { filterpanel.updateMinorTypeDisplay(); }
        property string filter: "";
        property string selectedCode: "";
        property string selectedLabel: "";
        property string siteQueryFilter: (majorTypeSource.selectedCode == "All") ? "" :
                                          (minorTypeSource.selectedCode == "All") ? 
                                            "&sitetypes=" + majorTypeSource.selectedCode : 
                                              "&sitetypes=" + minorTypeSource.selectedCode;
    }
    Item
    {
        id: selectMinorTypeDialog; width: 300; height: 350;

        SiteSearchFilterList
        {
            id: minorTypeSelector; anchors.fill: parent;
            onCurrentIndexChanged:
            {
                var obj = minorTypeSelector.model.get(minorTypeSelector.list.currentIndex);
                if (typeof obj === "object")
                {
                    minorTypeSource.selectedLabel = obj.code;
                    minorTypeSource.selectedCode = obj.code;
                } else { minorTypeSelector.list.currentIndex = 0; }
            }
            onAction: { filterpanel.hideDialog(dialogSelectMinorType, minorTypeFilter); }
        }

        Keys.onDownPressed: { minorTypeSelector.list.currentIndex += (minorTypeSelector.list.currentIndex >= (minorTypeSelector.list.count-1) ? 0 : 1); }
        Keys.onUpPressed: { minorTypeSelector.list.currentIndex -= (minorTypeSelector.list.currentIndex <= 0 ? 0 : 1); }
        Keys.onEscapePressed: { filterpanel.toggleDialog(dialogSelectMinorType, minorTypeFilter); }
        Keys.onReturnPressed: { filterpanel.toggleDialog(dialogSelectMinorType, minorTypeFilter); }
    }


    //
    // functions
    //

    function toggleDialog( dialog, target )
    {
        if (dialog.visible) hideDialog(dialog, target);
        else showDialog(dialog, target);
    }

    function showDialog( dialog, target )
    {
        if (filterpanel.currentDialog != -1) hideDialog(currentDialog, currentTarget);

        filterpanel.currentDialog = dialog;
        filterpanel.currentTarget = target;
        var popupPosition = dialog.popupPosition(target);
        dialog.x = popupPosition.x;
        dialog.y = popupPosition.y;

        target.toggled = true;
        dialog.visible = true;
        dialog.mainItem.focus = true;
    }

    function hideDialog( dialog, target )
    {
        if (typeof target === "object") target.toggled = false;
        dialog.mainItem.focus = false;
        dialog.visible = false;
        if (dialog == filterpanel.currentDialog) 
        {
            filterpanel.currentDialog = -1;
            filterpanel.currentTarget = -1;
        }
    }

    function updateStateDisplay()
    {
        stateSelector.model.clear();
        var results = stateSource.data["statecodes?"];
        if (typeof results === "undefined") return;
        for (var key in results)
        {
            if (key != "statecode_count" && key.substring(0, "statecode_".length) == "statecode_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var i = ListSort.findSortedPosition(nameValue, stateSelector.model);
                stateSelector.model.insert(i, {"name":nameValue, "code":result["alpha"], "numeric":result["numeric"]});
            }
        }
        stateListTimer.start();
    }

    function updateAgencyDisplay()
    {
        console.log("agency display called");
        agencySelector.model.clear();
        var results = agencySource.data["agencycodes?" + agencySource.filter];
        if (typeof results === "undefined") return;
        for (var key in results)
        {
            if (key != "agencycode_count" && key.substring(0, "agencycode_".length) == "agencycode_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var codeValue = result["code"];
                var i = ListSort.findSortedPosition(nameValue, agencySelector.model);
                agencySelector.model.insert(i, {"name":nameValue, "code":codeValue});
            }
        }
        agencySelector.model.insert(0, {"name":"United States Geological Survey", "code":"USGS"});
        agencySelector.list.currentIndex = 0;
    }

    function updateCountyDisplay()
    {
        countySelector.model.clear();
        var results = countySource.data["countycodes?" + countySource.filter];
        if (typeof results === "undefined") return;

        for (var key in results)
        {
            if (key != "countycode_count" && key.substring(0, "countycode_".length) == "countycode_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var i = ListSort.findSortedPosition(nameValue, countySelector.model);
                countySelector.model.insert(i, {"name":nameValue, "code":result["countycode"], "statecode":result["statecode"]});
            }
        }
        countySelector.list.currentIndex = 0;
    }

    function updateMajorTypeDisplay()
    {
        majorTypeSelector.model.clear();
        var results = majorTypeSource.data["sitetypecodes?major"];
        if (typeof results === "undefined") return;

        for (var key in results)
        {
            if (key != "sitetypecode_count" && key.substring(0, "sitetypecode_".length) == "sitetypecode_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var i = ListSort.findSortedPosition(nameValue, majorTypeSelector.model);
                majorTypeSelector.model.insert(i, {"name":nameValue, "code":result["code"]});
            }
        }
        majorTypeSelector.model.insert(0, {"name":"All Major Site Types", "code":"All"});
        majorTypeSelector.list.currentIndex = 0;
        majorTypeSelector.selectedIndex = 0;
    }

    function updateMinorTypeDisplay()
    {
        minorTypeSelector.model.clear();
        var results = minorTypeSource.data["sitetypecodes?" + minorTypeSource.filter];
        if (typeof results === "undefined") return;

        for (var key in results)
        {
            if (key != "sitetypecode_count" && key.substring(0, "sitetypecode_".length) == "sitetypecode_")
            {
                var result = results[key];
                var nameValue = result["name"];
                var i = ListSort.findSortedPosition(nameValue, minorTypeSelector.model);
                minorTypeSelector.model.insert(i, {"name":nameValue, "code":result["code"]});
            }
        }
        minorTypeSelector.model.insert(0, {"name":"All Minor Site Types", "code":"All"});
        minorTypeSelector.list.currentIndex = 0;
    }

}
