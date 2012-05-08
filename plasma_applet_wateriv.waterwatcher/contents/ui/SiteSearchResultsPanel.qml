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
    id: resultspanel; spacing: 4;

    property variant model: siteModel;
    property variant list: siteList;

    signal clicked();
    signal doubleClicked(string code);
    signal pressAndHold(string code);

    PlasmaCore.SvgItem
    {
        id: navSeparator; svg: lineSvg; elementId: "horizontal-line";
        width: parent.width; height: 3;
    }

    Rectangle
    {
        id: siteListHeader; radius: 5; smooth: true; 
        color: theme.buttonBackgroundColor;
        width: parent.width; height: headerCode.height + 10;

        PlasmaCore.FrameSvgItem
        {
            id: headerFrame; anchors.fill: parent;
            imagePath: "widgets/frame"; prefix: "raised";
        }

        Text
        {
            id: headerCode; text: i18n("<b>Code</b>");
            width: 120; color: theme.buttonTextColor;
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 10;
        }
    
        Text
        {
            id: headerName; text: i18n("<b>Name</b>");
            anchors.left: headerCode.right; 
            width: 275; color: theme.buttonTextColor;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Text
        {
            id: headerCount; 
            text: (siteList.count == 0) ? "" : (siteList.count == 1 ? "1 result" : siteList.count + " results");
            anchors.right: parent.right; anchors.rightMargin: 10; color: theme.buttonTextColor;
            anchors.verticalCenter: parent.verticalCenter;
        }
    }

    Item
    {
        width: 398; height: 150; 
        anchors.leftMargin: 2; anchors.left: parent.left;

        PlasmaCore.FrameSvgItem
        {
            id: frame; anchors.fill: parent;
            imagePath: "widgets/frame"; prefix: "sunken";
        }

        ListModel { id: siteModel; }
        ListView 
        {
            id: siteList; model: siteModel;
            width: 390; height: 135; 
            anchors.verticalCenter: parent.verticalCenter; 
            anchors.horizontalCenter: parent.horizontalCenter;
            delegate: siteDelegate;
            highlight: siteListHighlight;
        }
        Component
        {
            id: siteListHighlight;
            Rectangle { width: 390; radius: 5; smooth: true; color: theme.highlightColor; }
        }
        Component 
        {
            id: siteDelegate;
            Rectangle
            {
                width: 390; height: txtCode.height + 8; color: "transparent";
                Text
                {
                    id: txtCode; text: code;
                    width: 120; anchors.leftMargin: 5; anchors.rightMargin: 5;
                    anchors.topMargin: 4; anchors.bottomMargin: 4;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left; color: theme.textColor;
                }
                Text
                {
                    id: txtName; text: name; elide: Text.ElideRight;
                    width: 265; anchors.rightMargin: 5;
                    anchors.topMargin: 4; anchors.bottomMargin: 4;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: txtCode.right; color: theme.textColor;
                }
                MouseArea 
                { 
                    anchors.fill: parent; 
                    onClicked: { siteList.currentIndex = index; resultspanel.clicked(); }
                    onDoubleClicked: { siteList.currentIndex = index; resultspanel.doubleClicked(txtCode.text); } 
                    onPressAndHold: { siteList.currentIndex = index; resultspanel.pressAndHold(txtCode.text); }
                }
            }
        }
    }

}
