/*
   FALCON - The Falcon Programming Language.
   FILE: fx-events.h

   Falcon script Xchat plugin
   Definition of xchat event-to-parameter mappings.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-08-04 19:57:00

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Definition of xchat event-to-parameter mappings.
*/

#ifndef FX_EVENTS_H
#define FX_EVENTS_H

#include <map>
#include <list>

#include <falcon/string.h>
#include <falcon/autocstring.h>
#include <stdio.h>

class StringCompareIgnoreCase
{
public:

   bool operator ()( const Falcon::String &first, const Falcon::String &second ) const
   {
   	return first.compareIgnoreCase( second ) < 0;
   }
};

class ParamDesc
{
public:
	const char *m_param;
	const char *m_desc;
	ParamDesc( const char *p, const char *d ):
		m_param( p ),
		m_desc( d )
   {}
};

typedef std::list< ParamDesc > ParamDescList;
typedef std::map< Falcon::String, ParamDescList, StringCompareIgnoreCase > ParamMap;

extern ParamMap s_PMevent;
extern ParamMap s_PMsrvmsg;

void LoadEventParamMap();
void LoadSrvMsgParamMap();

#endif

/* end of fx-events.h */
