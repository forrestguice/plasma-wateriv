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

Column
{
    id: panel; 
    spacing: 5; 
    anchors.bottomMargin: 5;
    property variant field: inputDataSource;
    property variant error: errorField;

    state: "NORMAL";
    states: [
        State
        {
            name: "NORMAL";
            PropertyChanges { target: errorField; visible: false; }
        },
        State
        {
            name: "ERROR";
            PropertyChanges { target: errorField; visible: true; }
            PropertyChanges { target: errorField; state: "READONLY"; }
        }
    ]

    Row
    {
        spacing: 5;
        anchors.left: parent.left;
        Text
        {
            id: label;
            text: "Data Source:";
            anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 5;
        }

        TextField
        {
            id: inputDataSource;
            width: 250; textInitial: dataRequest;
            anchors.verticalCenter: parent.verticalCenter;
            onAction: 
            {
                plasmoid.writeConfig("datasource", input.text); 
                dialog_info.toggleDialog();
                infodialog.state = "RECENT";
                panel.state = "NORMAL";
                main.configChanged();
            }
        }
    }

    TextField
    {
        id: errorField;
        anchors.leftMargin: label.width + 5;
        anchors.left: parent.left;
        width: 250;
    }
}
