A plasmoid and data engine for the KDE4 Desktop that displays recent real-time water data from the USGS (http://waterservices.usgs.gov/). Water data is available from thousands of stream gauges around the United States.

This plasmoid can be used to display river and creek information as a widget on the desktop that is useful for boating and other activities reliant on streamflow conditions.

### Requirements ###

  * To run the QML plasmoid: KDE 4.7+.
  * To run the data engines: KDE 4.6+.

  * To compile the data engines: kdelibs5-dev, build-essential, cmake.

### Features ###

  * A resizable main widget that displays the most recent reading.
  * A popup dialog that displays site data (name, id, lon, lat, etc), and up to the last four data readings.
  * A _search for sites_ dialog that locates sites by state, county, agency, and type.

### Screenshots ###

<p><b>Main Widget</b></p>
<img src='http://plasma-wateriv.googlecode.com/svn/wiki/screenshots/waterwatcher-0.3.1-1.png' alt='Screenshot: Main Widget' />

<p><b>Most Recent Value</b></p>
<img src='http://plasma-wateriv.googlecode.com/svn/wiki/screenshots/waterwatcher-0.3.1-2.png' alt='Screenshot: Most Recent Value' />

<p><b>Search for Sites</b></p>
<img src='http://plasma-wateriv.googlecode.com/svn/wiki/screenshots/waterwatcher-0.3.1-3.png' alt='Screenshot: Search for Sites' />