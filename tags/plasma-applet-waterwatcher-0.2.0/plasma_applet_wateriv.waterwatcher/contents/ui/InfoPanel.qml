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
    spacing: 5; anchors.bottomMargin: 5;

    property string title: main.app_name;
    property string content: "<br/><br/>";

    TextEdit
    {
        id: txtTitle; text: title; 
        font.bold: true; color: theme.viewTextColor;
        readOnly: true; selectByMouse: true;
        anchors.left: parent.left; anchors.leftMargin: 5; anchors.rightMargin: 10;
    }

    TextEdit
    {
        id: txtContent; text: content; 
        font.bold: false; color: theme.textColor;
        readOnly: true; selectByMouse: true;
        anchors.left: parent.left; anchors.leftMargin: 5; anchors.rightMargin: 10;
    }
}
