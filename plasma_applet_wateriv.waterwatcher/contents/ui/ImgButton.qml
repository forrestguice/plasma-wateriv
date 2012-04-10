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

Rectangle
{
    id: btn; radius: 5; smooth: true;
    width: 25; height: 25; color: normalColor;
    enabled: visible;

    signal action;

    property color normalColor: "transparent";
    property color hoverColor: theme.viewFocusColor;
    property color pressedColor: theme.viewFocusColor;
    property variant image: arrowSvg;
    property string element;

    states: [
        State 
        {
            name: "NORMAL";
            PropertyChanges { target: btn; color: normalColor; }
        },
        State 
        {
            name: "HOVERED";
            PropertyChanges { target: btn; color: hoverColor; }
        },
        State
        {
            name: "PRESSED";
            PropertyChanges { target: btn; color: pressedColor; }
        }
    ]

    MouseArea
    {
        anchors.fill: parent; hoverEnabled: true;
        onClicked: { btn.action(); }
        onEntered: { btn.state = "HOVERED" }
        onExited: { btn.state = "NORMAL"; }
        onPressed: { btn.state = "PRESSED"; }
        onReleased: { btn.state = "NORMAL"; }
    }

    PlasmaCore.SvgItem
    {
        svg: image; elementId: element; 
        anchors.fill: parent;
    }

}
