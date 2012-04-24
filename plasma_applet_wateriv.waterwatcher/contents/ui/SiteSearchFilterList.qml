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

Item 
{
    id: filterListContainer;
    property bool singleColumn: false;
    property variant model: filterModel;
    property variant list: filterList;
    property int selectedIndex: -1;

    signal action;
    signal currentIndexChanged(int index);

    ListModel { id: filterModel; }
    ListView 
    {
        id: filterList; model: filterModel; delegate: filterDelegate;
        highlight: Rectangle { radius: 5; color: "lightblue"; }
        anchors.fill: parent;
        onCurrentIndexChanged: 
        { 
            filterListContainer.currentIndexChanged(currentIndex); 
        }
    }
    Component
    {
        id: filterDelegate;
        Rectangle
        {
            width: 400; height: txtName.height + 8; color: "transparent";
            Text
            {
                id: txtName; text: (filterListContainer.singleColumn) ? name : code + "\t" + name; color: theme.buttonTextColor;
                anchors.left: parent.left; anchors.leftMargin: 5; anchors.rightMargin: 5;
                anchors.topMargin: 4; anchors.bottomMargin: 4; anchors.verticalCenter: parent.verticalCenter;
            }
            MouseArea 
            {
                anchors.fill: parent; 
                onClicked: { filterList.currentIndex = index; }
                onDoubleClicked: { filterListContainer.action(); }
            }
        }
    }
}

