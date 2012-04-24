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
    
    property string title: i18n(main.app_name);
    property string content: i18n("<br/><br/>");

    Row
    {
        id: headerRow; spacing: 0; anchors.left: parent.left;

        Rectangle { width: 5; height: 5; color: "transparent"; }

        TextEdit
        {
            id: txtTitle; text: title; 
            font.bold: true; color: theme.viewTextColor;
            readOnly: true; selectByMouse: true;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Rectangle { width: 5; height: 5; color: "transparent"; }
    }

    Row
    {
        id: contentRow; spacing: 0; anchors.left: parent.left;

        Rectangle { width: 5; height: 5; color: "transparent"; }

        TextEdit
        {
            id: txtContent; text: content; 
            font.bold: false; color: theme.textColor;
            readOnly: true; selectByMouse: true;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Rectangle { width: 5; height: 5; color: "transparent"; }

    }
}
