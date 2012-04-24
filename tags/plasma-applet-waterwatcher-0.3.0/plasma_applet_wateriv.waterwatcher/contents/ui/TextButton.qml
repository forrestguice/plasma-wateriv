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

Rectangle
{
    id: btn; radius: 5; smooth: true;
    width: (txt.desiredWidth > 50) ? txt.desiredWidth : 50; 
    height: (txt.desiredHeight > 25) ? txt.desiredHeight : 25;

    property bool toggled: false;
    property string buttonText;
    property color textNormalColor: theme.buttonTextColor;
    property color textHoverColor: theme.buttonTextColor;
    property color textSelectedColor: theme.backgroundColor;
    property color backgroundNormalColor: theme.buttonBackgroundColor;
    property color backgroundSelectedColor: theme.viewHoverColor;
    property color backgroundHoverColor: theme.viewHoverColor;

    signal action;

    state: toggled ? "TOGGLED" : "NORMAL";
    onToggledChanged: { btn.state = toggled ? "TOGGLED" : "NORMAL"; }

    states: [
        State
        {
            name: "NORMAL";
            PropertyChanges { target: btn; color: backgroundNormalColor; }
            PropertyChanges { target: txt; color: textNormalColor; }
        },
        State
        {
            name: "HOVERED";
            PropertyChanges { target: btn; color: backgroundHoverColor; }
            PropertyChanges { target: txt; color: textHoverColor; }
        },
        State 
        {
            name: "TOGGLED";
            PropertyChanges { target: btn; color: backgroundSelectedColor; }
            PropertyChanges { target: txt; color: textSelectedColor; }
        },
        State
        {
            name: "PRESSED";
            PropertyChanges { target: btn; color: backgroundSelectedColor; }
            PropertyChanges { target: txt; color: textSelectedColor; }
        }
    ]

    MouseArea
    {
        anchors.fill: parent; hoverEnabled: true;
        onClicked: { btn.action(); }
        onEntered: { btn.state = "HOVERED"; }
        onExited: 
        {
            if (toggled) btn.state = "TOGGLED";
            else btn.state = "NORMAL";
        }
        onPressed: { btn.state = "PRESSED"; }
        onReleased: 
        { 
            if (toggled) btn.state = "TOGGLED";
            else btn.state = "NORMAL";
        }
    }

    Text
    {
        id: txt; 
        text: buttonText; 
        anchors.fill: parent; anchors.verticalCenter: parent.verticalCenter; 
        anchors.left: parent.left; anchors.topMargin: 4; anchors.bottomMargin: 4;
        anchors.leftMargin: 10; anchors.rightMargin: 10;
        property int desiredHeight: txt.paintedHeight + txt.anchors.topMargin + txt.anchors.bottomMargin;
        property int desiredWidth: txt.paintedWidth + txt.anchors.leftMargin + txt.anchors.rightMargin; 
    }
}
