/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_errhand.cpp

   Falcon script Xchat plugin
   Definition of xchat specific error handler.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Definition of xchat specific error handler.
*/

#include "fxchat_errhand.h"
#include "fxchat_script.h"
#include "fxchat.h"

void XChatErrHand::handleError( Falcon::Error *error, ScriptData* module )
{
	// signal the module had an error.
	if ( module != 0 )
   {
		module->m_bStatus = false;

      // also, prevent further callbacks to take place in this script.
      module->unhookAll();

      // the user will have to restart the module manually.
   }

	// convert the error in string.
	Falcon::String str;
   error->toString( str );

   // now prefix every row with "plugin name"
   str = (PNAME ": ") + str;

   Falcon::uint32 pos = str.find( "\n" );
   while( pos != Falcon::String::npos )
   {
   	pos++;
   	str.insert( pos, 0, PNAME ": " );
   	pos = str.find( "\n", pos );
   }

   xchat_print_falcon( str );
}

/* end of fxchat_errhand.cpp */
