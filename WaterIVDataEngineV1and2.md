| **status** | **Engine Name** | **Version** | **engine\_version** |
|:-----------|:----------------|:------------|:--------------------|
|deprecated|wateriv|0.2.0, 0.2.1|1, 2|

# Introduction #

This page describes the data made available by the WaterIV data engine. Plasmoids are able to access this data by connecting to the data engine (_wateriv_), requesting a source, and then using the key naming scheme described in "Available Data".

# Available Sources #

The most simple way to access the data engine is to request data from a single site using its site code. This will return a timeseries consisting of the most recent value. More extensive sets of data can be obtained by providing a request url (or partial url) specifying the arguments to pass to the web service. Valid source names include:

  1. A site code or comma separated list of site codes (up to 100).
> > See http://wdr.water.usgs.gov/nwisgmap/index.html.
  1. A request string specifying the data to retrieve (the part after the ? in a complete request url).
> > See http://waterservices.usgs.gov/rest/IV-Test-Tool.html.
  1. A complete request url specifying the data to retrieve.
> > See http://waterservices.usgs.gov/rest/IV-Test-Tool.html.
  1. A psuedo url specifying IV (Instant Values) or DV (Daily Values).
> > Example: IV?sites=10133650 DV?sites=10133650

Using (1) is a convenient way to get recent data by site code.
Using (2) allows more control over the data that is requested.
Using (3) allows data to be requested from an alternate url.
Using (4) is a convenient way to get historic daily values instead of instantaneous values.


# Available Data #

Data is returned by the data engine using a key naming scheme. The engine\_version key returns a unique engine version id and can be used by plasmoids to verify compatibility. The _net_ and _xml_ keys contain meta data describing downloading and parsing the data. The _queryinfo_ keys contain information about the request as it was understood by the web service. The _timeseries_ keys contain the actual dataset.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|engine`_`version|int|A unique version id.|


> 
---


Keys starting with _net`_`_ contain info about the network request for data. The _net`_`request`_`isvalid_ key is false when the data engine rejects a request. The _net`_`request`_`error_ key contains the error. The _net`_`error key is available when_net`_`isvalid_is false. Other keys will not be available unless_net`_`isvalid_and_net`_`request`_`isvalid_are true._

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|net`_`isvalid|bool|The result of the network operation.|
|net`_`url|QString|The base url of the web service.|
|net`_`request|QString|The request made to the web service.|
|net`_`request\_isvalid|bool|The request was accepted by the data engine.|
|net`_`request\_error|QString|Any error messages generated when parsing the request (when request\_isvalid is false)|
|net`_`error|int|Any net error codes that occurred|


Keys starting with _xml`_`_ contain info about the raw xml returned by the request. The _xml`_`error`_`_ keys are available when _xml`_`isvalid_ is false. Other keys will not be available unless _xml`_`isvalid_ is true.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|xml\_isvalid|bool|The request returned valid xml|
|xml\_schema|QString|The xsi:schemaLocation (should be WaterML-1.1.xsd)|
|xml\_error\_msg|QString|Any error messages that occurred.|
|xml\_error\_line|int|The line the error occured on.|
|xml\_error\_column|int|The column the error occured on.|

> 
---


Keys starting with _queryinfo`_`_ contain info about the data request as it was understood by the web service. These keys will not be available when _net`_`isvalid_ or _xml`_`isvalid_ is false.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|queryinfo\_url|QString|The url of the web service.|
|queryinfo\_time|QDateTime|The time the query was processed.|
|queryinfo\_notes|QMap<String,QVariant>|A QMap of query notes by title.|
|queryinfo\_location|QString|The requested location (site/other codes).|
|queryinfo\_variable|QString|The requested variables (parameter codes).|

> 
---


The _timeseries`_`count_ key returns the number of available data sets. The data for each individual time series can be found under _timeseries`_`i_ (where 0 <= i < _timeseries`_`count_). This key will return 0 when _net`_`isvalid_ or _xml`_`isvalid_ is false.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|timeseries\_count|int|The number of available timeseries.|

> 
---


Keys starting with _timeseries`_`i`_`_ contain info about an individual timeseries. Each timeseries is composed of three segments; sourceinfo, variable, and values.

The _sourceinfo_ keys contain info about the source of a timeseries.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|timeseries\_i\_sourceinfo\_sitename|QString|The name of the source (physical site name).|
|timeseries\_i\_sourceinfo\_sitecode|QString|The agency code of the source (e.g. usgs)|
|timeseries\_i\_sourceinfo\_properties|QMap<String,QVariant>|A QMap of site properties by name.|
|timeseries\_i\_sourceinfo\_latitude|QString|The latitude of the source.|
|timeseries\_i\_sourceinfo\_longitude|QString|The longitude of the source.|
|timeseries\_i\_sourceinfo\_timezone\_name|QString|The timezone abreviation of the source.|
|timeseries\_i\_sourceinfo\_timezone\_offset|QString|The timezone offset.|
|timeseries\_i\_sourceinfo\_timezone\_daylight|bool|Site currently using daylight savings.|
|timeseries\_i\_sourceinfo\_timezone\_daylight\_name|QString|The daylight savings time name (e.g. EDT).|
|timeseries\_i\_sourceinfo\_timezone\_daylight\_offset|QString|The daylight savings time offset.|


The _variable_ keys contain info about the dependent variable in the timeseries.

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|timeseries\_i\_variable\_code|QString|The variable code (e.g. 00060).|
|timeseries\_i\_variable\_code\_attributes|QMap<QString,QVariant>|A QMap of code attributes by name.|
|timeseries\_i\_variable\_name|QString|The name of the variable.|
|timeseries\_i\_variable\_description|QString|A description of the variable.|
|timeseries\_i\_variable\_valuetype|QString|The type of value the variable describes (e.g. Derived).|
|timeseries\_i\_variable\_unitcode|QString|The unit code (e.g. ft, cfs, etc).|
|timeseries\_i\_variable\_options|QMap<QString,QVariant>|A QMap of options by name.|
|timeseries\_i\_variable\_nodatavalue|double|The value to assign if a value has no data.|


The _values_ keys contain info about individual data points in the timeseries. The _timeseries`_`i`_`values`_`count_ key returns the number of value sets in the time series. The data for each set can be found under _timeseries`_`i`_`values`_`j_ (where 0 <= j < _timeseries`_`i`_`values`_`count_).

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|timeseries\_i\_values\_count|int|The number of available value sets.|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|timeseries\_i\_values\_j\_recent|double|The most recent value (e.g. 200).|
|timeseries\_i\_values\_j\_recent\_date|QDateTime|The datetime of the recent value.|
|timeseries\_i\_values\_j\_recent\_qualifier|QString|The qualifier of the most recent value (e.g. "P").|
|timeseries\_i\_values\_j\_all|QMap`<`QString,QList`<`QVariant`>``>`|A QMap of all values by QDateTime.|
|timeseries\_i\_values\_j\_qualifiers|QMap`<`QString,QList`<`QVariant`>``>`|A QMap of qualifiers by code.|


Each value in _all_ consists of a tuple.
| **Index** | **Type** | **Description** |
|:----------|:---------|:----------------|
|0 |double|The value (e.g. 200).|
|1 |QString|The qualifier codes assigned to the value (e.g. P is provisional data).|


Each qualifier consists of a QList of five values.
| **Index** | **Type** | **Description** |
|:----------|:---------|:----------------|
|0 |int|qualifier id (e.g. 0 : an integer id)|
|1 |QString|qualifier code (e.g. "P" same as map key)|
|2 |QString|qualifier description (e.g. "Provisional)|
|3 |QString|qualifier network (?)|
|4 |QString|qualifier vocabulary (?)|