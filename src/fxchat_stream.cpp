/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_stream.cpp

   Falcon script Xchat plugin
   Definition of xchat specific VM streams
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Definition of xchat specific VM streams.
*/

#include "fxchat_stream.h"
#include "fxchat.h"

#include <string.h>

XChatStream::XChatStream():
   m_prefix( "" ),
   Stream( t_stream )
{
}

XChatStream::XChatStream( const Falcon::String &prefix ):
	m_prefix( prefix ),
	Stream( t_stream )
{
}

Falcon::int32 XChatStream::write( const void *buffer, Falcon::int32 size )
{
	const char *buf = (const char *)buffer;
	int len = 0;
	while( len < size )
	{
		len ++;
		m_buffer += *buf;
		if ( *buf == '\n' )
		{
			xchat_print_falcon( m_prefix + m_buffer );
			m_buffer = "";
      }
		++buf;
   }
}



bool XChatStream::writeString( const Falcon::String &source, Falcon::uint32 begin, Falcon::uint32 end )
{
	if ( begin != 0 || end <= source.length() )
	{
		Falcon::String sub( source, begin, end );
		internalWrite( sub );
	}
	else {
		internalWrite( source );
	}

	return true;
}

void XChatStream::internalWrite( const Falcon::String &source )
{
	Falcon::uint32 pos = source.find( "\n" );
	if ( pos != Falcon::csh::npos )
   {
      if ( pos == source.length() - 1 )
      {
         xchat_print_falcon( m_prefix + m_buffer + source );
         m_buffer = "";
      }
      else {
         xchat_print_falcon( m_prefix + m_buffer + source.subString( 0, pos + 1 ) );
         m_buffer = source.subString( pos + 1 );
      }
   }
   else {
      m_buffer += source;
   }
}

Falcon::int64 XChatStream::seek( Falcon::int64 pos, e_whence w )
{
   status( t_unsupported );
   return -1;
}


bool XChatStream::close()
{
	return true;
}

Falcon::int32 XChatStream::read( void *buffer, Falcon::int32 size )
{
   status( t_unsupported );
   return -1;
}

Falcon::int64 XChatStream::tell()
{
	return 0;
}

bool XChatStream::truncate( Falcon::int64 pos )
{
   status( t_unsupported );
   return -1;
}

Falcon::int32 XChatStream::readAvailable( Falcon::int32 msecs_timeout )
{
   status( t_unsupported );
   return -1;
}

Falcon::int32 XChatStream::writeAvailable( Falcon::int32 msecs_timeout )
{
	return 1;
}

Falcon::int64 XChatStream::lastError() const
{
	return 0;
}

bool XChatStream::get( Falcon::uint32 &chr )
{
   status( t_unsupported );
   return false;
}

bool XChatStream::put( Falcon::uint32 chr )
{
	m_buffer += chr;
	if( chr == '\n' )
	{
		xchat_print_falcon( m_prefix + m_buffer );
		m_buffer = "";
	}
	return true;
}



/* end of fxchat_stream.cpp */
