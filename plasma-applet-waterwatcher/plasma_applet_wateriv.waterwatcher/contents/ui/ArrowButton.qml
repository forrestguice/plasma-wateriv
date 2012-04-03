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
    width: 25; height: 25; color: "transparent";

    signal action;

    property string arrowElement;

    PlasmaCore.SvgItem
    {
        svg: arrowSvg; elementId: arrowElement; anchors.fill: parent;

        MouseArea
        {
            anchors.fill: parent; hoverEnabled: true;
            onEntered: { btn.color = theme.viewHoverColor; }
            onExited: { btn.color = "transparent"; }
            onPressed: { actionEntered(); }
            onReleased: { actionExited(); }
            onClicked: { btn.action(); }
        }
    }

    function actionEntered()
    {
        btn.color = theme.viewFocusColor;
    }

    function actionExited()
    {
        btn.color = "transparent";
    }
}
