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
import org.kde.qtextracomponents 0.1 as QtExtraComponents

Rectangle
{
    id: field; radius: 5; smooth: true; color: theme.backgroundColor;
    width: 100; height: (txt.desiredHeight > 25) ? txt.desiredHeight : 25;

    signal action;
    signal cancel;

    property variant input: txt;
    property string textInitial: "";
    property string textPrevious: "";
    onTextInitialChanged: { txt.text = textInitial; }

    property color textNormalColor: theme.buttonTextColor;
    property color textReadonlyColor: "gray";
    property color textErrorColor: "red";

    property color backgroundNormalColor: theme.backgroundColor;
    property color backgroundModifiedColor: theme.buttonHoverColor;
    property color backgroundErrorColor: theme.buttonFocusColor;
    property color backgroundReadonlyColor: theme.buttonBackgroundColor;

    property bool readOnly: false;

    state: "NORMAL";
    states: [
        State
        {
            name: "NORMAL";
            PropertyChanges { target: txt; readOnly: false; }
            //PropertyChanges { target: field; color: backgroundNormalColor; }
            PropertyChanges { target: txt; color: textNormalColor; }
            PropertyChanges { target: btnAccept; visible: false; }
            PropertyChanges { target: btnCancel; visible: false; }
        },
        State
        {
            name: "MODIFIED";
            when: textInitial != txt.text && !field.readOnly;
            //PropertyChanges { target: field; color: backgroundModifiedColor; }
            PropertyChanges { target: txt; color: textNormalColor; }
            PropertyChanges { target: btnAccept; visible: true; }
            PropertyChanges { target: btnCancel; visible: true; }
        },
        State
        {
            name: "ERROR";
            //PropertyChanges { target: field; color: backgroundErrorColor; }
            PropertyChanges { target: txt; color: textErrorColor; }
            PropertyChanges { target: btnAccept; visible: false; }
            PropertyChanges { target: btnCancel; visible: false; }
        },
        State
        {
            name: "READONLY";
            //PropertyChanges { target: field; color: backgroundReadonlyColor; }
            PropertyChanges { target: txt; readOnly: true; }
            PropertyChanges { target: txt; color: textReadonlyColor; }
            PropertyChanges { target: btnAccept; visible: false; }
            PropertyChanges { target: btnCancel; visible: false; }
        }
    ]

    PlasmaCore.FrameSvgItem
    {
        id: frame; anchors.fill: parent; visible: true;
        imagePath: "widgets/lineedit"; prefix: "base";
    }
    PlasmaCore.FrameSvgItem
    {
        id: focusFrame; anchors.fill: parent; visible: txt.focus;
        imagePath: "widgets/lineedit"; prefix: "focus";
    }

    TextInput
    {
        id: txt; text: textInitial; selectByMouse: true; 
        width: field.width - btnAccept.width - btnCancel.width;
        anchors.verticalCenter: parent.verticalCenter; 
        anchors.left: parent.left; anchors.topMargin: 4; anchors.bottomMargin: 4;
        anchors.leftMargin: 5; anchors.rightMargin: 5;
        property int desiredHeight: txt.paintedHeight + txt.anchors.topMargin + txt.anchors.bottomMargin;
        property int desiredWidth: txt.paintedWidth + txt.anchors.leftMargin + txt.anchors.rightMargin; 

        Keys.onReturnPressed: { if (field.state == "MODIFIED") acceptChanges(); }
    }

    ImgButton
    {
        id: btnAccept; width: 16; height: 16;
        image: configSvg; element: "add";
        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: btnCancel.left; anchors.rightMargin: 4;
        onAction: { acceptChanges(); }
    }

    ImgButton
    {
        id: btnCancel; width: 16; height: 16;
        image: configSvg; element: "delete";
        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right; anchors.rightMargin: 4;
        onAction: { rejectChanges(); }
    }
   
    function acceptChanges()
    {
        textPrevious = textInitial; 
        textInitial = txt.text; 
        field.action();
    }

    function rejectChanges()
    {
        txt.text = textInitial;
        field.cancel();
    }

}
