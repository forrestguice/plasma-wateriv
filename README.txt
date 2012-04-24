==============================================================================
||  WaterSites DataEngine v0.1 README                                       ||
==============================================================================

  ==============================
  0) Table of Contents
  ==============================

      I) Description
     II) Documentation

    III) Requirements
     IV) Compiling
      V)  Installation

     VI) Bugs
    VII) Legal Stuff


  ==============================
  I) Description
  ==============================

   project url: http://code.google.com/p/plasma-wateriv/

   The WaterSites dataengine retrieves information about available sites that
   are monitored by the USGS. It can be used to obtain site coes for use with
   the WaterIV dataengine. See http://waterservices.usgs.gov.

  ==============================
  II) Documentation
  ==============================

   Online documentation is available at 
   http://code.google.com/p/plasma-wateriv/w/list.

   ------
   INPUT: the source name for requests to the data engine can be:

   1) a site code, or comma separated list of site codes (up to 100)
      (see http://wdr.water.usgs.gov/nwisgmap/index.html)

   2) a request string specifying the data to retrieve (the part after 
      the ? in a complete request url)
      (see http://waterservices.usgs.gov/rest/Site-Test-Tool.html)

   3) a complete request url specifying the data to retrieve 
      (see http://waterservices.usgs.gov/rest/Site-Test-Tool.html)

   4) a special request that returns locally stored data: 

        stateCodes?               : returns state alpha and numeric codes
        agencyCodes?[<filter>]    : returns agency codes (filter: state alpha)
        countyCodes?[<filter>]    : returns county codes (filter: state num)
        siteTypeCodes?[<filter>]  : returns sitetype codes (filter: majorType)

        All codes will be returned when the optional filter is not provided.

   ------
   OUTPUT: a set of key/value pairs organized by simple naming scheme

   Documentation is available at: http://code.google.com/p/plasma-wateriv/w/list
   or by browsing the comment blocks at the beginning of watersitesengine.cpp.


  ==============================
  III) Requirements
  ==============================

    * To run this DataEngine requires KDE 4.6+.

    * To compile requires kdelibs-dev (kdelibs5-dev).
    * To compile requries some common build tools (build-essential, cmake).


  =================================
  IV) Compiling (on Ubuntu 11.10)
  =================================

   0) install required build tools and libs:

      $ sudo apt-get install build-essential cmake kdelibs5-dev

   1) change to the source directory

      $ cd plasma-dataengine-watersites-0.1.0

   2) compile the source code

      $ cmake -DCMAKE_INSTALL_PREFIX=/usr ./
      $ make clean
      $ make

   3) (optional) remove the debugging symbols to reduce the binary size

      $ strip ./lib/plasma_engine_watersites.so


  =================================
  V) Installation (on Ubuntu 11.10)
  =================================

   0) succesfully compile the source code (see section IV Compiling)

   1) install directly from the compiled source to /usr 
      (see IV Compiling step 2; cmake)

      $ sudo make install

   2) get KDE to find the newly installed files by rebuilding the config cache

      $ kbuildsycoca4

   3) to uninstall (requires compiled source)

      $ sudo make uninstall


  ==============================
  VI) Bugs
  ==============================
    Users with a Google account can report bugs to the issue tracker at 
    http://code.google.com/p/plasma-wateriv/issues/list

    Other users can send bug reports to ftg2@users.sourceforge.net. Include 
    "plasma-wateriv" somewhere in the subject line.


  ==============================
  VII) Legal Stuff
  ==============================

    Copyright 2012 Forrest Guice
    This file is part of WaterIV DataEngine.

    WaterIV DataEngine is free software: you can redistribute it and/or 
    modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WaterIV DataEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WaterIV DataEngine.  If not, see <http://www.gnu.org/licenses/>.

