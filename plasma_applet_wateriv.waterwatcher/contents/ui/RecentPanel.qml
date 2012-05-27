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

        Rectangle
        {
            width: parent.width - headerRow.width - titleRow.width;
            height: txtTitle.height;
            anchors.left: headerRow.right;
            anchors.right: titleRow.left;
            anchors.verticalCenter: parent.verticalCenter;
            color: "transparent";

            MouseArea
            {
                anchors.fill: parent;
                hoverEnabled: true;
                onPressed:
                {
                    if (sitePanelCollapsed) btnTitle1.state = "PRESSED";
                    else btnTitle2.state = "PRESSED";
                }
                onEntered:
                {
                    if (sitePanelCollapsed) btnTitle1.state = "HOVERED";
                    else btnTitle2.state = "HOVERED";
                }
                onExited:
                {
                    if (sitePanelCollapsed) btnTitle1.state = "NORMAL";
                    else btnTitle2.state = "NORMAL";
                }
                onClicked:
                {
                    if (sitePanelCollapsed) btnTitle1.state = "NORMAL";
                    else btnTitle2.state = "NORMAL";
                    panel.toggleSiteInfo();
                }
            }
        }

        Row
        {
            id: titleRow;
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

}
