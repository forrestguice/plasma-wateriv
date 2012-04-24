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
    width: (rowNav.width > panelCurrent.width) ? rowNav.width : 
           panelCurrent.width + panelCurrent.anchors.leftMargin + panelCurrent.anchors.rightMargin;
    height: rowNav.height + panelCurrent.height + navSeparator.height + 
            anchors.topMargin + anchors.bottomMargin + (spacing*2);

    signal nextSeries;
    signal prevSeries;
    signal toggleDialog;
    signal configChanged;

    property string navText: "0/0";
    property variant panelConfig: configpanel;
    property variant panelRecent: recentpanel;
    property variant panelCurrent: panelRecent;

    Keys.onEscapePressed: { toggleDialog(); }
    Keys.onPressed:
    {
        if (event.key == Qt.Key_Left) btnPrev.state = "PRESSED";
        else if (event.key == Qt.Key_Right) btnNext.state = "PRESSED";
    }

    Keys.onReleased:
    {
        if (event.key == Qt.Key_Left) 
        {
            btnPrev.state = "NORMAL";
            if (btnPrev.visible) btnPrev.action();

        } else if (event.key == Qt.Key_Right) {
            btnNext.state = "NORMAL";
            if (btnNext.visible) btnNext.action();
        }
    }

    onPanelCurrentChanged:
    {
        panelConfig.visible = false;
        panelRecent.visible = false;
        panelCurrent.visible = true;
        infodialog.focus = true;
    }

    state: "RECENT";
    states: [
        State
        {
            name: "CONFIGURE";
            PropertyChanges { target: infodialog; panelCurrent: panelConfig }
            PropertyChanges { target: tabrecent; toggled: false }
            PropertyChanges { target: tabconfig; toggled: true }
            PropertyChanges { target: btnPrev; visible: false }
            PropertyChanges { target: txtNav; visible: false }
            PropertyChanges { target: btnNext; visible: false }
        },
        State
        {
            name: "RECENT";
            PropertyChanges { target: infodialog; panelCurrent: panelRecent }
            PropertyChanges { target: tabrecent; toggled: true }
            PropertyChanges { target: tabconfig; toggled: false }
            PropertyChanges { target: btnPrev; visible: true }
            PropertyChanges { target: txtNav; visible: true }
            PropertyChanges { target: btnNext; visible: true }
        }
    ]

    Row
    {
        id: rowNav; spacing: 2;

        TextButton
        {
            id: tabconfig; toggled: false;
            buttonText: "Configuration";
            anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 10; anchors.rightMargin: 10;
            onAction: { infodialog.state = "CONFIGURE"; }
        }

        TextButton
        {
            id: tabrecent; toggled: true;
            buttonText: "Most Recent Value";
            anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 10; anchors.rightMargin: 10;
            onAction: { infodialog.state = "RECENT"; }
        }

        ImgButton
        {
            id: btnPrev; 
            image: arrowSvg; element: "left-arrow";
            anchors.leftMargin: 10;
            onAction: { infodialog.prevSeries(); }
        }

        Text
        {
            id: txtNav; text: navText;
            font.bold: true; color: theme.viewTextColor;
            anchors.verticalCenter: parent.verticalCenter;
        }

        ImgButton
        {
            id: btnNext; 
            image: arrowSvg; element: "right-arrow";
            onAction: { infodialog.nextSeries(); }
        }
    }

    PlasmaCore.SvgItem
    {
        id: navSeparator; svg: lineSvg; elementId: "horizontal-line";
        width: infodialog.width; height: 3;
    }

    ConfigPanel
    {
        id: configpanel;
        visible: false;
        onDataSourceChanged: 
        {
            infodialog.toggleDialog();
            infodialog.state = "RECENT";
            infodialog.configChanged();
        }
    }

    RecentPanel
    {
        id: recentpanel;
        visible: false;
    }

}
