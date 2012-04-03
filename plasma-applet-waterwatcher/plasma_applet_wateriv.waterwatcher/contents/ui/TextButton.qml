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
    color: selected ? backgroundSelectedColor : backgroundNormalColor;
    width: txt.paintedWidth + txt.anchors.leftMargin + txt.anchors.rightMargin; 
    //height: txt.paintedHeight + txt.anchors.topMargin + txt.anchors.bottomMargin;
    height: 25;

    signal action;

    property string buttonText;
    property bool selected: true;
    property color textNormalColor: theme.buttonTextColor;
    property color textSelectedColor: theme.backgroundColor;
    property color backgroundNormalColor: theme.buttonBackgroundColor;
    property color backgroundSelectedColor: theme.viewHoverColor;
    property color backgroundHoverColor: theme.viewHoverColor;

    Text
    {
        id: txt; text: buttonText; 
        color: selected ? textSelectedColor : textNormalColor;
        anchors.fill: parent; anchors.verticalCenter: parent.verticalCenter; 
        anchors.left: parent.left; anchors.topMargin: 4; anchors.bottomMargin: 4;
        anchors.leftMargin: 10; anchors.rightMargin: 10;

        MouseArea
        {
            anchors.fill: parent; hoverEnabled: true;

            onClicked: { btn.action(); }
            onEntered: { btn.color = backgroundHoverColor; }

            onPressed: 
            { 
                btn.color = backgroundSelectedColor; 
                txt.color = textSelectedColor;
            }

            onReleased: 
            { 
                btn.color = selected ? backgroundSelectedColor : backgroundNormalColor; 
                txt.color = selected ? textSelectedColor : textNormalColor;
            }

            onExited: 
            { 
                btn.color = selected ? backgroundSelectedColor : backgroundNormalColor; 
                txt.color = selected ? textSelectedColor : textNormalColor;
            }
        }
    }
}
