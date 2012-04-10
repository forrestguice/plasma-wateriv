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
    id: mainWidget; spacing: 0;
    anchors.horizontalCenter: parent.horizontalCenter;
    anchors.verticalCenter: parent.verticalCenter;

    property string fontStyle;
    property bool fontBold: false;
    property bool fontItalic: false;
    property color fontColor: "black";
    property color fontShadow: "white";

    property bool showUnits: true;
    property bool showDate: true;
    property bool showShadow: true;
    property bool showCustomColor: false;
    property bool showCustomShadow: false;

    property int displaySeries: 0;
    property int displaySubSeries: 0;
    property string displayDate: "";
    property string displayValue: "";
    property string displayUnits: "";

    onShowUnitsChanged: { var tmp = displayValue; displayValue = ""; displayValue = tmp; }

    onDisplayValueChanged: 
    {
        if (showUnits) 
        {
            if (mainWidget.state == "NORMAL")
            {
                display_value.text = displayValue + " " + displayUnits; 
            } else {
                display_value.text = displayValue; 
            }
        } else {
            display_value.text = displayValue;
        }
    }
    
    onDisplayUnitsChanged:
    {
        if (showUnits) display_value.text = displayValue + " " + displayUnits; 
        else display_value.text = displayValue;
    }

    onDisplayDateChanged: { display_date.text = displayDate; }

    state: "NORMAL";
    states: [
        State
        {
            name: "NORMAL";
        },
        State
        {
            name: "ERROR";
        }
    ]

    Text
    {
        id: display_value; //text: ""; 
        style: Text.Raised; color: fontColor; styleColor: fontShadow;
        font.italic: fontItalic; font.bold: fontBold; font.family: fontStyle;
        font.pixelSize: (current_size * main.width) / 200; 
        anchors.horizontalCenter: parent.horizontalCenter;

        property int default_size: 38;
        property int current_size: 38;

        onTextChanged: 
        {
            if (main.width <= 1 || display_value.paintedWidth <= 1) return;
            //display_value.current_size = display_value.default_size;
 
            while (display_value.paintedWidth < main.width && 
                   display_value.current_size < 64)
            {
                //console.log("scaling up: " + display_value.current_size);
                if (mainWidget.showDate == false)
                {
                    if ((display_value.paintedHeight) >= main.height) 
                        break;
                } else {
                    if ((display_value.paintedHeight + display_date.paintedHeight) >= main.height) 
                        break;
                }
                display_value.current_size++;   // scale font up
            }
            //console.log("done scaling up: " + display_value.current_size);

            while (display_value.paintedWidth > main.width &&
                   display_value.current_size > 8)
            {
                //console.log("scaling down: " + display_value.current_size);
                display_value.current_size--;   // scale font down
            }
            //console.log("done scaling down: " + display_value.current_size);
        }
    }

    Text
    {
        id: display_date;
        visible: showDate;

        text: ""; style: Text.Raised;
        font.bold: false; font.family: fontStyle;
        font.pixelSize: (current_size * main.width) / 200;
        color: fontColor; styleColor: fontShadow;
        anchors.horizontalCenter: parent.horizontalCenter;

        property int default_size: 24;
        property int current_size: 24;

        onTextChanged: 
        {
            if (main.width <= 0 || display_date.paintedWidth <= 0) return;
            // display_date.current_size = display_date.default_size;

            while ( display_date.paintedWidth > main.width && 
                    display_date.current_size > 8)
            {
                display_date.current_size--;
            }
        }
    }

    function triggerFontResize()
    {
        var tmp_value = display_value.text;
        display_value.text = "";
        if (tmp_value == "") display_value.text = main.app_name;
        else display_value.text = tmp_value;

        var tmp_date = display_date.text;
        display_date.text = "";
        display_date.text = tmp_date;
    }
}
