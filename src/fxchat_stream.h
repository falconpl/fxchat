/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_stream.h

   Falcon script Xchat plugin
   Falcon stream redirected to xchat
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
	Falcon script Xchat plugin.
	Redefinition of the streams.
*/

#ifndef fxchat_stream_H
#define fxchat_stream_H

#include <falcon/engine.h>
#include <falcon/stream.h>

class XChatStream: public Falcon::Stream
{
	Falcon::String m_prefix;
	Falcon::String m_buffer;

	void internalWrite( const Falcon::String &source );

protected:
	virtual Falcon::int64 seek( Falcon::int64 pos, e_whence w );

public:
   bool writeString( const Falcon::String &source, Falcon::uint32 begin=0, Falcon::uint32 end = Falcon::csh::npos );

   XChatStream();
	XChatStream( const Falcon::String &prefix );

   virtual bool close();
   virtual Falcon::int32 read( void *buffer, Falcon::int32 size );
   virtual Falcon::int32 write( const void *buffer, Falcon::int32 size );
   virtual Falcon::int64 tell();
   virtual bool truncate( Falcon::int64 pos=-1 );
   virtual Falcon::int32 readAvailable( Falcon::int32 msecs_timeout );
   virtual Falcon::int32 writeAvailable( Falcon::int32 msecs_timeout );
   virtual Falcon::int64 lastError() const;
   virtual bool get( Falcon::uint32 &chr );
   virtual bool put( Falcon::uint32 chr );
};


#endif

/* end of fxchat_stream.h */
