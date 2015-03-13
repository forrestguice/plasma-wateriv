| **status** | **Engine Name** | **Version** | **engine\_version** |
|:-----------|:----------------|:------------|:--------------------|
|current|wateriv|0.3.1|4 |

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

Data is returned by the data engine using a key naming scheme. The engine\_version key returns a unique engine version id and can be used by plasmoids to verify compatibility. The _net_ and _xml_ keys contain meta data describing downloading and parsing the data. The _queryinfo_ keys contain information about the request as it was understood by the web service. The _series_ keys contain the actual dataset.

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


| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|toc`_`count|int|table of contents count|
|toc`_`#|QMap<QString, QVariant>|table of contents entry|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|series`_`siteCode`_`name|QString|site name|
|series`_`siteCode`_`code|QString|site code|
|series`_`siteCode`_`properties|QMap<String, QVariant> (by name)|side properties|
|series`_`siteCode`_`latitude|float|site latitude|
|series`_`siteCode`_`longitude|float|site longitude|
|series`_`siteCode`_`timezone`_`name|QString|site timezone name|
|series`_`siteCode`_`timezone`_`offset|...|site timezone offset|
|series`_`siteCode`_`timezone`_`daylight|bool|site uses daylight savings|
|series`_`siteCode`_`timezone`_`daylight\_name|QString|site daylight savings timezone name|
|series`_`siteCode`_`timezone`_`daylight\_offset|...|site dyalight savings timezone offset|

|series`_`siteCode`_`paramCode`_`name|QString|variable name|
|:-----------------------------------|:------|:------------|
|series`_`siteCode`_`paramCode`_`code|QString|variable code|
|series`_`siteCode`_`paramCode`_`code`_`attributes|QMap`<`QString, QVariant`>` (by attrib)|variable properties|
|series`_`siteCode`_`paramCode`_`description|QString|variable description|
|series`_`siteCode`_`paramCode`_`valuetype|QString|variable type (derived)|
|series`_`siteCode`_`paramCode`_`unitcode|QString|variable unit code|
|series`_`siteCode`_`paramCode`_`nodatavalue|double|variable no data value|

|series`_`siteCode`_`paramCode`_`statCode`_`name|QString|statistic name|
|:----------------------------------------------|:------|:-------------|
|series`_`siteCode`_`paramCode`_`statCode`_`code|int|statistic code|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`id|int|method id|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`description|QString|method name/desc|
|series`_`siteCode`_`paramCode\_statCode\_methodId\_all|QMap`<`QString, QList`<`QVariant`>``>`|all values|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`qualifiers|QMap`<`QString, QList`<`QVariant`>``>`|qualifiers for values|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`recent`_`value|double|most recent value|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`recent\_date|QDateTime|date of most recent value|
|series`_`siteCode`_`paramCode`_`statCode`_`methodId`_`recent`_`qualifier|QString|qualifier of most recent value|


Each value in toc`_`# consists of a QMap of index information.
| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|site|QString|site code|
|variable|QString|parameter code|
|statistic|QString|statistic code|
|method|QString|methodid code|

The components of the toc`_`# can assembled into keys using this pattern: series`_`site`_`variable`_`statistic`_`methodId;


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