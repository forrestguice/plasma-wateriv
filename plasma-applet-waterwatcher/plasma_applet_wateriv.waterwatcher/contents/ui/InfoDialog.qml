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
    id: infodialog; spacing: 5; anchors.bottomMargin: 5;
    width: (rowNav.combinedWidth > txtTitle.paintedWidth) ? rowNav.combinedWidth :
            txtTitle.paintedWidth + txtTitle.anchors.rightMargin + txtTitle.anchors.leftMargin;
    height: rowNav.combinedHeight + txtContent.paintedHeight + 
            txtTitle.paintedHeight + anchors.bottomMargin + (spacing * 3) + navSeparator.height;

    signal nextSeries;
    signal prevSeries;
    signal toggleDialog;

    property string title: main.app_name;
    property string content;
    property string navText: "0/0";

    Keys.onEscapePressed: { toggleDialog(); }

    Keys.onPressed:
    {
        if (event.key == Qt.Key_Left) btnPrev.actionEntered();
        else if (event.key == Qt.Key_Right) btnNext.actionEntered();
    }

    Keys.onReleased:
    {
        if (event.key == Qt.Key_Left) 
        {
            btnPrev.actionExited();
            btnPrev.action();

        } else if (event.key == Qt.Key_Right) {
            btnNext.actionExited();
            btnNext.action();
        }
    }

    Row
    {
        id: rowNav; spacing: 2;
        property int combinedWidth: btnPrev.width + btnNext.width + tabinfo.width + txtNav.paintedWidth + tabinfo.marginleft + tabinfo.marginRight + txtNav.marginLeft + txtNav.marginRight + btnNext.marginLeft + btnNext.marginRight + btnPrev.marginLeft + btnPrev.marginRight + (spacing * 5);
        property int combinedHeight: (txtNav.paintedHeight > btnPrev.height) ? txtNav.paintedHeight : btnPrev.height;

        ArrowButton
        {
            id: btnPrev; arrowElement: "left-arrow";
            onAction: { infodialog.prevSeries(); }
        }

        Text
        {
            id: txtNav; text: navText;
            font.bold: true; color: theme.viewTextColor;
            anchors.verticalCenter: parent.verticalCenter;
        }

        ArrowButton
        {
            id: btnNext; arrowElement: "right-arrow";
            anchors.rightMargin: 10;
            onAction: { infodialog.nextSeries(); }
        }

        TextButton
        {
            id: tabinfo; buttonText: "Most Recent Value";
            anchors.verticalCenter: parent.verticalCenter;
            onAction: {}
        }
    }

    PlasmaCore.SvgItem
    {
        id: navSeparator; svg: lineSvg; elementId: "horizontal-line";
        width: infodialog.width; height: 3;
    }

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
