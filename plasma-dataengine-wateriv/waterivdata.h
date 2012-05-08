/**
    Copyright 2012 Forrest Guice
    This file is part of Plasma-WaterIV.

    Plasma-WaterIV is free software: you can redistribute it and/or 
    modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Plasma-WaterIV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Plasma-WaterIV.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WATERIVDATA_H
#define WATERIVDATA_H
 
#include "waterivengine.h"

/**
    WaterIVData : abstract class

    Provides an interface for data sources for the wateriv engine.
    Implementations need to override extractData and getFormat().
*/

class WaterIVData
{
    public:
        virtual ~WaterIVData() {}
        virtual QString getFormat() = 0;
        virtual void extractData( WaterIVEngine *engine, const QString &source, QByteArray &bytes ) = 0;
};
 
#endif
