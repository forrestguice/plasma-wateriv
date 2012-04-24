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

Row
{
    id: searchRow; spacing: 5; anchors.left: parent.left;

    property string query: searchField.input.text;
    signal performSearch();

    state: "NORMAL";
    states: [
        State
        {
            name: "NORMAL";
            PropertyChanges { target: searchField; readOnly: true; }
            PropertyChanges { target: searchField; state: "READONLY"; }
        },
        State
        {
            name: "ADVANCED";
            PropertyChanges { target: searchField; readOnly: false; }
            PropertyChanges { target: searchField; state: "NORMAL"; }
        }
    ]

    TextButton
    {
        buttonText: "Search"; toggled: true;
        anchors.verticalCenter: parent.verticalCenter;
        onAction: { searchRow.performSearch(); }
    }

    TextField
    {
        id: searchField; width: 300; state: "READONLY"; readOnly: true;
        anchors.verticalCenter: parent.verticalCenter;
        onAction: { searchRow.toggleAdvanced(); }
        onCancel: { searchRow.toggleAdvanced(); }
    }

    ImgButton
    {
        id: btnAdvanced; image: configSvg; element: "configure";
        width: 16; height: 16; anchors.verticalCenter: parent.verticalCenter;
        onAction: { searchRow.toggleAdvanced(); }
    }

    //
    // functions
    //

    function toggleAdvanced()
    {
        if (searchRow.state == "ADVANCED") { searchRow.state = "NORMAL"; } 
        else { searchRow.state = "ADVANCED"; }
    }

    function setQuery( q )
    { 
        if (typeof searchField.input != "undefined") searchField.input.text = q;
    }

}
