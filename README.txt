
    WaterWatcher (QML Plasmoid) 0.2.1 README
    http://code.google.com/p/plasma-wateriv/

    ==========================================================================
    || Description:                                                         ||
    ==========================================================================

       WaterWatcher is a plasmoid that displays the latest real-time water data 
       from the USGS Instantaneous Values Web Service. Data is available from
       thousands of sites around the United States. Readings are made every 15
       minutes and transmitted hourly (http://waterservices.usgs.gov/rest).

    ==========================================================================
    || Requirements:                                                        ||
    ==========================================================================

       KDE 4.7+
       http://www.kde.org

       WaterIV DataEngine v0.2.1 (plasma-dataengine-wateriv)
       http://code.google.com/p/plasma-wateriv/

    ==========================================================================
    || Installation:                                                        ||
    ==========================================================================

       1) Install the required data engine (plasma-dataengine-wateriv)
          (http://code.google.com/p/plasma-wateriv/downloads/list)

          If upgrading from a previous version, logging-out and logging-in may
          be required for KDE to recognize the new engine version.

       2) Install or upgrade the plasmoid
          
          Using plasmapkg:
             installing:
             $ plasmapkg -r plasma_applet_wateriv.waterwatcher
             $ plasmapkg -i plasma_applet_wateriv.waterwatcher
         
             upgrading:
             $ plasmapkg -u plasma_applet_wateriv.waterwatcher

          Using cmake:
             $ cmake -DCMAKE_INSTALL_PREFIX=/usr ./
             $ sudo make install
             $ sudo make uninstall

    ==========================================================================
    || Configuration:                                                       ||
    ==========================================================================

       Provide a valid site code in the settings dialog. Use NSWIS Mapper 
       (http://wdr.water.usgs.gov/nwisgmap/index.html) to locate sites.

    ==========================================================================
    || Bugs:                                                                ||
    ==========================================================================

       Users with a Google account can report bugs to the issue tracker at
       http://code.google.com/p/plasma-wateriv/issues/list

       Other users can send bug reports to ftg2@users.sourceforge.net. Include
       "plasma-wateriv" somewhere in the subject line.

    ==========================================================================
    || Legal Stuff:                                                         ||
    ==========================================================================

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


