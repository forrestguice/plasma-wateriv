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
    id: panel; spacing: 0; 

    property int panelWidth: 340;
    
    property string title: i18n(main.app_name);
    property string content: i18n("<br/><br/>");
    property string siteTitle: i18n("<br/><br/>");
    property string siteContent: i18n("<br/><br/>");

    property bool sitePanelCollapsed: false;
    property string emptyContent: "";

    PlasmaCore.DataSource
    {
        id: appsengine; engine: "apps";
        connectedSources: ["kde4-marble.desktop"];
    }
    PlasmaCore.DataSource
    {
        id: monitorengine; engine: "systemmonitor";
        connectedSources: ["ps"]; interval: 1000;
    }

    Item
    {
        width: panelWidth; height: headerRow.height;

        PlasmaCore.FrameSvgItem
        {
            id: headerFrame; anchors.fill: parent;
            imagePath: "widgets/frame"; prefix: "raised";
        }

        Column
        {
            id: headerRow; spacing: 0; anchors.left: parent.left;

            Rectangle { width: 5; height: 5; color: "transparent"; }
            Row
            {
                spacing: 0; anchors.left: parent.left;
       
                Rectangle { width: 10; height: 5; color: "transparent"; }
                TextEdit
                {
                    id: txtTitle; text: title; 
                    font.bold: true; color: theme.viewTextColor;
                    readOnly: true; selectByMouse: true;
                    anchors.rightMargin: 10; anchors.verticalCenter: parent.verticalCenter;
                }
                Rectangle { width: 5; height: 5; color: "transparent"; }
            }
            Rectangle { width: 5; height: 5; color: "transparent"; }
        }

        Row
        {
            anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter;
            ImgButton
            {
                id: btnTitle1; anchors.verticalCenter: parent.verticalCenter;
                image: configSvg; element: "add"; width: 16; height: 16;
                visible: sitePanelCollapsed;
                onAction: { panel.toggleSiteInfo(); } 
            }
            ImgButton
            {
                id: btnTitle2; anchors.verticalCenter: parent.verticalCenter;
                image: configSvg; element: "remove"; width: 16; height: 16;
                visible: !sitePanelCollapsed;
                onAction: { panel.toggleSiteInfo(); } 
            }
            Rectangle { width: 10; height: 5; color: "transparent"; }
        }
    }

    Item
    {
        width: panelWidth; height: siteInfoRow.height;

        opacity: (!sitePanelCollapsed && siteInfo.text != emptyContent) ? 100 : 0;
        Behavior on opacity
        {
            NumberAnimation { properties: "opacity"; duration: 200; } 
        }

        PlasmaCore.FrameSvgItem
        {
            id: frame; anchors.fill: parent;
            imagePath: "widgets/frame"; prefix: "raised";
        }

        Column
        {
            id: siteInfoRow; spacing: 0; 

            Rectangle { width: 5; height: 5; color: "transparent"; }
            Row
            {
                spacing: 0; anchors.left: parent.left;

                Rectangle { width: 10; height: 5; color: "transparent"; }
                TextEdit
                {
                    id: siteInfo; text: panel.siteContent; 
                    font.bold: false; color: theme.textColor;
                    readOnly: true; selectByMouse: true;
                    anchors.verticalCenter: parent.center;
                    anchors.topMargin: 5; anchors.bottomMargin: 5;
                }
                Rectangle { width: 5; height: 5; color: "transparent"; }
            }
            Rectangle { width: 5; height: 5; color: "transparent"; }
        }
    }

    Item
    {
        width: panelWidth; height: contentRow.height;

        PlasmaCore.FrameSvgItem
        {
            id: contentFrame; anchors.fill: parent;
            imagePath: "widgets/frame"; prefix: "raised";
        }
        
        Column
        {
            id: contentRow; spacing: 0; anchors.left: parent.left;

            Rectangle { width: 5; height: 5; color: "transparent"; }
            Row
            {
                spacing: 0; anchors.left: parent.left;

                Rectangle { width: 10; height: 5; color: "transparent"; }
                TextEdit
                {
                    id: txtContent; text: content; 
                    font.bold: false; color: theme.textColor;
                    readOnly: true; selectByMouse: true;
                    anchors.verticalCenter: parent.center;
                }
                Rectangle { width: 5; height: 5; color: "transparent"; }
            }
            Rectangle { width: 5; height: 5; color: "transparent"; }
        }
    }

    //
    // functions
    //

    function toggleSiteInfo()
    {
        panel.sitePanelCollapsed = !panel.sitePanelCollapsed; 
        plasmoid.writeConfig("sitepanel_collapsed", panel.sitePanelCollapsed);
    }

    function launchMarble()
    {
        var pid1 = findMarblePID();
        console.log("marble pid (0): " + pid1);

        var data = appsengine.data["kde4-marble.desktop"];
        if (typeof data === "undefined")
        {
            // error: marble is not installed
            console.log("no marble");
        }

        var service = appsengine.serviceForSource("kde4-marble.desktop");
        var operation = service.operationDescription("launch");
        var job = service.startOperationCall(operation);
        console.log("launched marble");

        var pid2 = findMarblePID();
        console.log("marble pid (1): " + pid2);

    }

    function findMarblePID()
    {
        var pid = -1;

        var data = monitorengine.data["ps"];
        var value = data["value"];

        //for (var key in value)
        //{
        //    console.log("data: " + value[key]);
        //}
        console.log("value: " + data["value"]);

        return pid;
    }

}
