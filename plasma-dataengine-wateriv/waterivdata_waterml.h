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

#ifndef WATERIVDATAWATERML_H
#define WATERIVDATAWATERML_H
 
#include "waterivdata.h"
#include <QDomElement>

/**
    An implementation of WaterIVData; the "waterml" format.
*/
class WaterIVDataWaterML  : public WaterIVData
{
    public:
        static const QString FORMAT;

        virtual QString getFormat();
        virtual void extractData( WaterIVEngine *engine, const QString &source, QByteArray &bytes );

    private:
        void extractQueryInfo( WaterIVEngine *engine, const QString &request, QDomElement *element );
        void extractTimeSeries( WaterIVEngine *engine, const QString &request, QDomElement *element );
        QString extractSeriesSourceInfo( WaterIVEngine *engine, const QString &request, QString &prefix, QDomElement *timeSeries );
        QString extractSeriesVariable( WaterIVEngine *engine, const QString &request, QString &prefix, QDomElement *timeSeries );
        void extractSeriesValues( WaterIVEngine *engine, const QString &request, QString &prefix, int &count, QDomElement *timeSeries );
};
 
#endif
