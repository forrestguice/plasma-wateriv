| **status** | **Engine Name** | **Version** | **engine\_version** |
|:-----------|:----------------|:------------|:--------------------|
|current|watersites|0.1.0|0 |

# Available Sources #

The WaterSites data engine accesses data using the USGS Sites Web Service http://waterservices.usgs.gov/nwis/site. The web service accepts a request url (almost identical to the requests accepted by the IV/DV web services) and returns a resulting list of sites in the Mapper format (simple XML). The WaterSites data engine parses the resulting list of sites and makes it available to plasmoids through the site`_``<`code`>` keys.

Examples:
|stateCd=AZ|returns info on all sites within AZ|
|:---------|:----------------------------------|
|sites=09506000|returns info on single site 09506000|
|09506000|returns info on single site 09506000|

See http://waterservices.usgs.gov/rest/Site-Test-Tool.html for help forming valid requests.

Special request types include: statecodes`?` agencycodes`?` countycodes`?` and sitetypecodes`?` These requests return data stored in local data files.

Some special requests take simple filters.
|agencycodes`?``<`stateAlphaCode`>`|filter agencies by state|
|:---------------------------------|:-----------------------|
|countycodes`?``<`stateNumericCode`>`|filter counties by state|
|sitetypecodes`?``<`majorTypeCode`>`|filter subtypes by major type|

# Available Data #

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|engine`_`version|int|unique engine id|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|net`_`date|QDateTime|date of net request|
|net`_`url|QString|net host|
|net`_`request|QString|net request|
|net`_`request\_isvalid|bool|request is valid|
|net`_`request\_error|QString|request error|
|net`_`isvalid|bool|net transaction is valid|
|net`_`error|QString|net error|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|xml`_`isvalid|bool|xml is valid|
|xml`_`error`_`msg|QString|xml errors|
|xml`_`error`_`line|int|xml error line|
|xml`_`error`_`column|int|xml error column|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|statecode`_`count|int|# of statecodes|
|statecode`_``<`code`>`|QHash`<`QString, QVariant`>`|statecode entry|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|countycode`_`count|int|# of countycodes|
|countycode`_``<`code`>`|QHash`<`QString, QVariant`>`|countycode entry|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|agencycode`_`count|int|# of agencycodes|
|agencycode`_``<`code`>`|QHash`<`QString, QVariant`>`|agencycode entry|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|sitetypecode`_`count|int|# of sitetype codes|
|sitetypecode`_``<`code`>`|QHash`<`QString, QVariant`>`|sitetype entry|

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|site\_count|int|# of site codes|
|site`_``<`code`>`|QHash`<`QString, QVariant`>`|site entry|

Each statecode entry consists of:
| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|name|QString|state name|
|alpha|QString|state alpha code|
|numeric|QString|state numeric code|

Each countycode entry consists of:

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|name|QString|county name|
|statecode|QString|county state code (numeric)|
|countycode|QString|county code (numeric)|

Each agencycode entry consists of:

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|name|QString|agency name|
|code|QString|agency code|

Each sitetype entry consists of:

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|name|QString|type name|
|code|QString|type code|

Each site entry consists of:

| **Key** | **Type** | **Description** |
|:--------|:---------|:----------------|
|code|QString|site code|
|name|QString|site name|
|agency|QString|site agency|
|latitude|QString|site latitude|
|longitude|QString|site longitude|
|type|QString|site type|