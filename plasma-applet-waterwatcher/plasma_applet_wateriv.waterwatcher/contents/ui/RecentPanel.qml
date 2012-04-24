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
    id: panel; 
    spacing: 0; anchors.bottomMargin: 5;
    
    property string title: i18n(main.app_name);
    property string content: i18n("<br/><br/>");
    property string siteTitle: i18n("<br/><br/>");
    property string siteContent: i18n("<br/><br/>");
    property bool sitePanelCollapsed: false;

    Row
    {
        id: headerRow; spacing: 0; anchors.left: parent.left;
       
        Rectangle { width: 5; height: 5; color: "transparent"; }
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
        Rectangle { width: 5; height: 5; color: "transparent"; }
        TextEdit
        {
            id: txtTitle; text: title; 
            font.bold: true; color: theme.viewTextColor;
            readOnly: true; selectByMouse: true;
            anchors.rightMargin: 10; anchors.verticalCenter: parent.verticalCenter;
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    Row
    {
        id: siteInfoRow; spacing: 0; anchors.left: parent.left;
        visible: !sitePanelCollapsed;

        Rectangle { width: 26; height: 5; color: "transparent"; }
        TextEdit
        {
            id: siteInfo; text: panel.siteContent; 
            font.bold: false; color: theme.textColor;
            readOnly: true; selectByMouse: true;
            anchors.verticalCenter: parent.center;
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    Rectangle { width: 5; height: 5; color: "transparent"; }

    PlasmaCore.SvgItem
    {
        svg: lineSvg; elementId: "horizontal-line";
        width: parent.width; height: 3;
    }

    Rectangle { width: 5; height: 10; color: "transparent"; }

    Row
    {
        id: contentRow; spacing: 0; anchors.left: parent.left;

        Rectangle { width: 5; height: 5; color: "transparent"; }
        TextEdit
        {
            id: txtContent; text: content; 
            font.bold: false; color: theme.textColor;
            readOnly: true; selectByMouse: true;
            anchors.verticalCenter: parent.center;
        }
        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    //
    // functions
    //

    function toggleSiteInfo()
    {
        panel.sitePanelCollapsed = !panel.sitePanelCollapsed; 
        plasmoid.writeConfig("sitepanel_collapsed", panel.sitePanelCollapsed);
    }

}
