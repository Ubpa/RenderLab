/*
Copyright (C) 2014 Jay Satiro <raysatiro@yahoo.com>
All rights reserved.

This file is part of CSV/jay::util.

https://github.com/jay/CSV

jay::util is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jay::util is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with jay::util. If not, see <http://www.gnu.org/licenses/>.
*/

/** A class to write comma separated values (CSV) to a stream.

Documentation is in CSV.hpp.
*/

#include <3rdParty/csv/CSV.hpp>

#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "strerror.hpp"


using namespace std;


namespace jay {
namespace util {


CSVwrite::CSVwrite() :
    buffer_size( _buffer_size), error( _error ), error_msg( _error_msg )
{
    if( !Init() )
        return;
}


CSVwrite::CSVwrite( string filename, Flags flags /* = none */ ) :
    buffer_size( _buffer_size), error( _error ), error_msg( _error_msg )
{
    if( !Init() )
        return;

    Open( filename, flags );
}


CSVwrite::CSVwrite( ostream *stream, Flags flags /* = none */ ) :
    buffer_size( _buffer_size), error( _error ), error_msg( _error_msg )
{
    if( !Init() )
        return;

    Associate( stream, flags );
}


CSVwrite::~CSVwrite()
{
    free( _buffer );
}


bool CSVshared_ResizeBuffer(
    const streamsize bytes,   // IN
    char *&buffer,   // INOUT
    streamsize &buffer_size,   // INOUT
    bool &error,   // OUT
    string &error_msg   // OUT
); // This function defined and documented above CSVread::ResizeBuffer().


bool CSVwrite::ResizeBuffer( const streamsize bytes )
{
    return CSVshared_ResizeBuffer( bytes, _buffer, _buffer_size, _error, _error_msg );
}


// REM This function is also called by Init() for initialization
bool CSVwrite::Reset()
{
    if( _output_ptr )
    {
        _error = true;
        _error_msg = "Not implemented";
        return false;
    }

    if( !_buffer )
    {
        if( !ResizeBuffer( 4096 ) )
            return false;
    }

    _flags = CSVwrite::none;
    _error = false;
    _error_msg = "";
    _is_first_field = true;

    return true;
}


bool CSVwrite::Close()
{
    if( _file.is_open() )
    {
        _file.close();
    }

    _output_ptr =  NULL;

    return Reset();
}


// Initialization to be called from the constructor only, first thing.
bool CSVwrite::Init()
{
    // The vars here must be zeroed before any possible error aborts the initialization.
    _buffer = NULL;
    _buffer_size = 0;
    _output_ptr =  NULL;

    delimiter = ",";
    terminator = "\n";

    return Reset();
}




bool CSVwrite::Associate( ostream *stream, const Flags flags /* = none */ )
{
    if( _error )
        return false;

    if( !stream )
    {
        _error = true;
        _error_msg = "The stream parameter is NULL.";
        return false;
    }

    if( _output_ptr )
    {
        _error = true;
        _error_msg = "A stream is already associated. Call Close() to dissociate.";
        return false;
    }

    if( ( flags & text_mode ) && ( stream != &_file ) )
    {
        // For the time being text mode is only valid if it's a file opened by this class.
        _error = true;
        _error_msg = "Text mode is only valid for files opened by this class.";
        return false;
    }

    _flags = flags;
    _output_ptr = stream;

    if( !_output_ptr->good() )
    {
        _error = true;
        _error_msg = "ostream: " + ios_strerror( _output_ptr->rdstate() );
        return false;
    }

    return true;
}


bool CSVwrite::Open( string filename, const Flags flags /* = none */ )
{
    if( _error )
        return false;

    if( _file.is_open() )
    {
        _error = true;
        _error_msg = "A file is already open. Call Close() to close the file.";
        return false;
    }

    if( _output_ptr )
    {
        _error = true;
        _error_msg = "A stream is already associated. Call Close() to dissociate.";
        return false;
    }

    ios::openmode mode = ( ( flags & text_mode ) ) ? 0 : ios::binary;
    mode |= ( ( flags & truncate ) ) ? ios::trunc : ios::app;

    _file.open( filename, mode );
    if( !_file )
    {
        _error = true;
        _error_msg = "Failed opening " + filename;
        return false;
    }

    return Associate( &_file, flags );
}




bool CSVwrite::WriteUTF8BOM()
{
    if( _error )
        return false;

    if( !_output_ptr )
    {
        _error = true;
        _error_msg = "A stream is not associated with the object.";
        return false;
    }

    _output_ptr->write( "\xEF\xBB\xBF", 3 );
    if( !_output_ptr->good() )
    {
        _error = true;
        _error_msg = "ostream: " + ios_strerror( _output_ptr->rdstate() );
        return false;
    }

    return true;
}


bool CSVwrite::WriteTerminator()
{
    if( _error )
        return false;

    if( !_output_ptr )
    {
        _error = true;
        _error_msg = "A stream is not associated with the object.";
        return false;
    }

    _output_ptr->write( terminator.c_str(), terminator.length() );
    if( !_output_ptr->good() )
    {
        _error = true;
        _error_msg = "ostream: " + ios_strerror( _output_ptr->rdstate() );
        return false;
    }

    _is_first_field = true;
    return true;
}


bool CSVwrite::WriteField( const string &field, bool terminate /* = false */ )
{
    if( _error )
        return false;

    if( !_output_ptr )
    {
        _error = true;
        _error_msg = "A stream is not associated with the object.";
        return false;
    }

    char *p = _buffer;
    const char *const buffer_end = _buffer + (size_t)_buffer_size;

    list<char> prepend, append;

    if( !_is_first_field )
    {
        prepend.assign( delimiter.begin(), delimiter.end() );
    }

    if( terminate )
    {
        append.assign( terminator.begin(), terminator.end() );
    }

    // All fields are qualified with double quotes since that is what libcsv write functions do
    prepend.push_back( '"' );
    append.push_front( '"' );

    // This is set when there is a quote that needs to be escaped but there's no room in the buffer
    bool quote = false;

    // This is set when the buffer needs to be written to the stream even if the buffer is not full
    bool flush = false;

    for( string::const_iterator it = field.begin();; )
    {
        if( flush || ( p == buffer_end ) )
        {
            _output_ptr->write( _buffer, ( p - _buffer ) );
            if( !_output_ptr->good() )
            {
                _error = true;
                _error_msg = "ostream: " + ios_strerror( _output_ptr->rdstate() );
                return false;
            }

            if( flush && ( it == field.end() ) )
            {
                break;
            }

            p = _buffer;
        }

        if( it == field.begin() )
        {
            while( prepend.size() && ( p != buffer_end ) )
            {
                *p++ = *prepend.begin();
                prepend.pop_front();
            }

            if( p == buffer_end )
            {
                continue;
            }
        }

        if( quote )
        {
            *p++ = '"';
            quote = false;

            if( p == buffer_end )
            {
                continue;
            }
        }

        if( it == field.end() )
        {
            while( append.size() && ( p != buffer_end ) )
            {
                *p++ = *append.begin();
                append.pop_front();
            }

            if( !append.size() || ( p != buffer_end ) )
            {
                flush = true;
            }

            continue;
        }

        while( ( p != buffer_end ) && ( it != field.end() ) )
        {
            *p = *it++;
            if( *p++ == '"' )
            {
                if( p == buffer_end )
                {
                    quote = true;
                    break;
                }

                *p++ = '"';
            }
        }
    }

    _is_first_field = terminate;
    return true;
}


bool CSVwrite::WriteRecord( const vector<string> &fields, bool terminate /* = true */ )
{
    if( _error )
        return false;

    if( !_output_ptr )
    {
        _error = true;
        _error_msg = "A stream is not associated with the object.";
        return false;
    }

    if( !_is_first_field )
    {
        if( !WriteTerminator() )
        {
            return false;
        }
    }

    if( !fields.size() && !(( _flags & CSVwrite::process_empty_records )) )
    {
        return true;
    }

    for( vector<string>::const_iterator it = fields.begin(); it != fields.end(); ++it )
    {
        if( !WriteField( *it ) )
        {
            return false;
        }
    }

    if( terminate )
    {
        if( !WriteTerminator() )
        {
            return false;
        }
    }

    return true;
}


} // namespace util
} // namespace jay
