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

/**
    printf; replaces %# with supplied arguments 
*/
String.prototype.format = function() 
{
    var args = arguments;
    return this.replace(/%(\d+)/g, function (m, i) {
        return (typeof args[i] === 'undefined') ? m : args[i];
    });
};

/**
    createTooltipTitle : function
    @param site an obj containing site data
    @param param an obj containing param data
*/
function createTooltipTitle(site)
{
    return "%0 %1".format(site.name, site.code);
}

/**
    createTooltipText : function
    @param site an obj containing site data
    @param param an obj containing param data
*/
function createTooltipText(site, param)
{
    return "%0 (%1)<br/><br/><b>%2 %3</b><br/>%4<br/>%5".format(param.name, param.code, param.value, param.units, param.date, param.qualifier_desc);
}

/**
    createSitePanel : function
    @param site an obj containing site data
    @param param an obj containing param data
*/
function createSitePanelText(site, param)
{
    return "<table><tr><td><b>ID:</b>&nbsp;&nbsp;%0</td><td>&nbsp;&nbsp;&nbsp;</td><td><b>Type:</b>&nbsp;&nbsp;%3</td></tr><tr><td><b>Latitude:</b>&nbsp;&nbsp;%1</td><td>&nbsp;&nbsp;&nbsp;</td><td><b>Longitude:</b>&nbsp;&nbsp;%2</td></tr><tr><td><b>State Code:</b>&nbsp;&nbsp;%4</td><td>&nbsp;&nbsp;&nbsp;</td><td><b>County Code:</b>&nbsp;&nbsp;%5</td></tr><tr><td><b>HUC Code:</b>&nbsp;&nbsp;%6</td><td>&nbsp;&nbsp;&nbsp;</td><td></td></tr></table>".format(site.code, site.lat, site.lon, site.typeCd, site.stateCd, site.countyCd, site.hucCd);
}

/**
    createRecentPanelTitle : function
    @param site an obj containing site data
    @param param an obj containing param data
*/
function createRecentPanelTitle(site, param)
{
    return "%0".format(site.name);
}

/**
    createRecentPanelText : function
    @param site an obj containing site data
    @param param an obj containing param data
*/
function createRecentPanelText(site, param)
{
    var value_table = createValueTable(param.all, param.units);
    return "%0 (%1)<br/>%2 (method %3)<br/>%4<br/>%5".format(param.name, param.code, param.method_desc, param.method_id, value_table, param.qualifier_desc);
}

/**
   createNavigationText(current, total) : function
   @param current the currently displayed series
   @param total the total number of series
*/
function createNavigationText(current, total)
{
    return "%0 / %1".format((current + 1), total);
}

/**
   createQualifierList() : function
   @param codes a list of qualifier codes to display
   @param qualifiers a hash of all available qualifiers by code
   @return a list of qualifiers suitable for display with html.
*/
function createQualifierList(codes, qualifiers)
{
    var display = "";
    for (code in codes)
    {
        display += "<br/>" + qualifiers[codes[code]][2];
    }
    return display;
}

/**
   createValueTable() : function
   @param values a hash<datestring, [value, qualifier]> of values
   @param units the units string to display
   @return a "<table>" of values suitable for display with html.
*/
function createValueTable( values, units, n )
{
    if (typeof n === "undefined") n = 4;
    if (typeof units === "undefined") units = "";

    var keys = [];
    for (var key in values)
    {
        if (values.hasOwnProperty(key)) keys.push(key);
    }
    keys.sort();

    var c = 0;
    var table = "<table>";
    for (var i = keys.length - 1; i >= 0; i--)
    {
        if (c >= n) break;
        var k = keys[i];
        var d = Qt.formatDateTime(new Date(k));
        var v = values[k];
        table += "<tr><td width='50'><b>" + v[0] + " " + units + 
                 "</b></td><td>" + d + "</td></tr>";
        c++;
    }

    if (c < n)
    {
        for (var j=0; j < (n-c); j++)
        {
            table += "<tr><td width='50'>&nbsp;</td><td>&nbsp;</td></tr>";
        }
    }

    table += "</table>";
    return table;
}

