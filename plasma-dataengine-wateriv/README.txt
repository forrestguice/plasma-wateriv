==============================================================================
||  WaterIV DataEngine v0.1 README                                           ||
==============================================================================

  Table of Contents:

      I) WaterIV Description
     II) Requirements
    III) Compiling & Installation
     IV) Legal Stuff


  ==============================
  I) Description
  ==============================

   The WaterIV DataEngine retrieves timeseries data from the USGS Instantaneous 
   Values Web Service. The web service returns the data as an XML document 
   using a set of tags called "WaterML". The dataengine reads these tags (DOM) 
   and makes the data available to plasmoids using a simple naming scheme.

   (see http://waterservices.usgs.gov/rest)

   -------------------------------------------------------------------

   The data engine source can be:
   1) a site code, or comma separated list of site codes (up to 100)
      (see http://wdr.water.usgs.gov/nwisgmap/index.html)

   2) a request string specifying the data to retrieve (the part after 
      the ? in a complete request url)
      (see http://waterservices.usgs.gov/rest/IV-Test-Tool.html)

   3) a complete request url specifying the data to retrieve 
      (see http://waterservices.usgs.gov/rest/IV-Test-Tool.html)

   Using (1) is a convenient way to get recent data by site code.
   Using (2) allows more control over the data that is requested.
   Using (3) allows data to be requested from an alternate url.

   -------------------------------------------------------------------

   Browse the comment blocks at the beginning of waterivengine.cpp for
   a complete listing of available data keys (key naming scheme).


  ==============================
  II) Requirements
  ==============================

    This DataEngine requires version KDE 4.6+.
    Compiling requires kdelibs-dev (kdelibs5-dev).
    Compiling requries some common build tools (build-essential, cmake).


  =================================
  III) Installation on Ubuntu 11.10
  =================================

   0) install required build tools and libs:

      $ sudo apt-get install build-essential cmake kdelibs5-dev

   1) change directories

      $ cd WaterWatcher-DataEngine/src

   2) compile the source code

      $ cmake -DCMAKE_INSTALL_PREFIX=/usr ./
      $ make

   3) install and register the data engine

      $ sudo make install
      $ kbuildsycoca4

   4) test the installation

      $ plasmaengineexplorer --engine usgswater
      --> supply a sitecode as the request


  ==============================
  IV) Legal Stuff
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

