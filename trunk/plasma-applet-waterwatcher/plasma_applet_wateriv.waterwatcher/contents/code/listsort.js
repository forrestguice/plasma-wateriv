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
    findSortedPosition : function
    Find a sorted position for v within ListModel m using binary search
*/
function findSortedPosition( v, m )
{
    if (m.count == 0 ) return 0;
    var left = 0; var right = m.count-1;
    while (right >= left)
    {
        var i = Math.ceil((left + right) / 2);
        var c = m.get(i).name;
        if (v > c) { left = i + 1; }
        else if (v <= c) { right = i - 1; }
    }
    return Math.ceil((left + right) / 2);
}

