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

#ifndef SITESREQUEST_H
#define SITESREQUEST_H

#include "watersitesdata.h"

class QString;
 
class SitesRequest
{
    public:
        SitesRequest();

        static WaterSitesData* formatForSource( const QString &source );
        static QString requestForSource(const QString &source, QString &errorMsg, bool &dataIsRemote );

        static bool hasValidFilters( const QString &request, bool &hasFormatFilter, QString &errorMsg );

        static bool isSiteCode( const QString &value, QString &errorMsg );
        static bool isStateCode( const QString &value, QString &errorMsg );
        static bool isHucCode( const QString &value, QString &errorMsg );
        static bool isBBoxCode( const QString &value, QString &errorMsg );
        static bool isCountyCode( const QString &value, QString &errorMsg );

        static bool isFormatString( const QString &value, QString &errorMsg );

        static bool isDataType( const QString &value, QString &errorMsg );
        static bool isSiteType( const QString &value, QString &errorMsg );
        static bool isSiteStatus( const QString &value, QString &errorMsg );
        static bool isAquiferCode( const QString &value, QString &errorMsg );
        static bool isLocalAquiferCode( const QString &value, QString &errorMsg );
};
 
#endif
