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

/** A class to read comma separated values (CSV) from a stream.

Documentation is in CSV.hpp.
*/

#include <3rdParty/csv/CSV.hpp>

#include <stdint.h>

#include <fstream>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "csv.h"

#include "strerror.hpp"


using namespace std;


namespace jay {
namespace util {


struct cb_stuff
{
    cb_stuff(
        list<vector<string>> &_cache,
        CSVread::Flags &_flags,
        bool &_error_pending,
        std::string &_error_msg,
        bool &_end_record_not_terminated,
        uintmax_t &pending,
        uintmax_t &requested
    ) :
        _cache( _cache ), _flags( _flags ), _error_pending( _error_pending ), _error_msg( _error_msg ),
            _end_record_not_terminated( _end_record_not_terminated ),
            pending( pending ), requested( requested )
    {
    }

    // A reference to the CSVread::_cache list.
    list<vector<string>> &_cache;

    // A reference to the CSVread::_flags.
    const CSVread::Flags &_flags;

    // A reference to CSVread::_error_pending.
    bool &_error_pending;

    // A reference to CSVread::_error_msg.
    std::string &_error_msg;

    // A reference to CSVread::_end_record_not_terminated.
    bool &_end_record_not_terminated;

    // A reference to the record number of the pending record.
    uintmax_t &pending;

    // A reference to the record number of the requested record.
    const uintmax_t &requested;

private:
    cb_stuff( const cb_stuff & );
    cb_stuff & operator=( const cb_stuff & );
};


// Called by libcsv when a new field has been parsed for the pending record.
// The pending record is the back element in the cache list. If the record number of the pending
// record is less than the record number of the requested record then it's ignored.
static void Callback_Field( void *data, size_t data_size, void *userptr )
{
    cb_stuff *s = (cb_stuff *)userptr;

    if( s->_error_pending )
    {
        return;
    }

    if( s->pending >= s->requested )
    {
        // Push the field to the back of the pending record.
        s->_cache.back().push_back( string( (const char *)data, data_size ) );

        if( ( s->_flags & CSVread::error_on_null_in_field )
            && ( s->_cache.back().back().find_first_of( '\0' ) != string::npos )
        )
        {
            s->_error_pending = true;
            ostringstream ss;
            ss << "Record #" << s->pending << " Field #" << s->_cache.back().size()
                << " is invalid due to NULL byte in field.";
            s->_error_msg = ss.str();
            return;
        }
    }
}


// Called by libcsv when the pending record is complete (no more fields to parse).
// The pending record is the back element in the cache list. If the record number of the pending
// record is less than the record number of the requested record then it's ignored.
static void Callback_Record( int terminator, void *userptr )
{
    cb_stuff *s = (cb_stuff *)userptr;

    if( s->_error_pending
        || ( ( terminator == CSV_CR ) && ( s->_flags & CSVread::process_empty_records ) )
    )
    {
        return;
    }

    if( terminator == -1 )
    {
        s->_end_record_not_terminated = true;
    }

    if( s->pending >= s->requested )
    {
        // Push a vector to the back of the list to make a new pending record.
        s->_cache.push_back( vector<string>() );
    }

    ++s->pending;
}


CSVread::CSVread() :
    buffer_size( _buffer_size), eof( _eof ), error( _error ), error_msg( _error_msg ),
        has_utf8_bom( _has_utf8_bom), record_num( _record_num ), end_record_num( _end_record_num ),
        end_record_not_terminated( _end_record_not_terminated ), fields( _fields )
{
    if( !Init() )
        return;
}


CSVread::CSVread( string filename, Flags flags /* = none */ ) :
    buffer_size( _buffer_size), eof( _eof ), error( _error ), error_msg( _error_msg ),
        has_utf8_bom( _has_utf8_bom), record_num( _record_num ), end_record_num( _end_record_num ),
        end_record_not_terminated( _end_record_not_terminated ), fields( _fields )
{
    if( !Init() )
        return;

    Open( filename, flags );
}


CSVread::CSVread( istream *stream, Flags flags /* = none */ ) :
    buffer_size( _buffer_size), eof( _eof ), error( _error ), error_msg( _error_msg ),
        has_utf8_bom( _has_utf8_bom), record_num( _record_num ), end_record_num( _end_record_num ),
        end_record_not_terminated( _end_record_not_terminated ), fields( _fields )
{
    if( !Init() )
        return;

    Associate( stream, flags );
}


CSVread::~CSVread()
{
    if( parse_obj )
    {
        csv_free( parse_obj );
        delete parse_obj;
    }
    free( _buffer );
}


/* Resize a buffer.

Both CSVread and CSVwrite use this to resize their respective buffers.

[ret][failure] (false) : The buffer could not be resized and has retained its current size.
    'error' and 'error_msg' are set.
[ret][success] (true)
*/
bool CSVshared_ResizeBuffer(
    const streamsize bytes,   // IN
    char *&_buffer,   // INOUT
    streamsize &_buffer_size,   // INOUT
    bool &_error,   // OUT
    string &_error_msg   // OUT
)
{
    if( _buffer && ( _buffer_size == bytes ) )
        return true;

    if( bytes <= 0 )
    {
        _error = true;
        _error_msg = "buffer allocation failed. size of bytes is <= 0.";
        return false;
    }

    // CSV classes may cast 'buffer_size' to a size_t at any point so it can't be larger than that.
    if( static_cast<size_t>(bytes) > (numeric_limits<size_t>::max)() )
    {
        _error = true;
        _error_msg = "buffer allocation failed. size of bytes is > numeric_limits<size_t>::max.";
        return false;
    }

    char *temp = (char *)realloc( _buffer, (size_t)bytes );
    if( !temp )
    {
        _error = true;

        ostringstream ss;
        ss << "buffer allocation failed. size in bytes: " << bytes;
        _error_msg = ss.str();

        return false;
    }

    _buffer = temp;
    _buffer_size = bytes;
    return true;
}


bool CSVread::ResizeBuffer( const streamsize bytes )
{
    return CSVshared_ResizeBuffer( bytes, _buffer, _buffer_size, _error, _error_msg );
}


bool CSVread::ResetParser()
{
    /* libcsv resets the parser in csv_fini() but not in csv_free(). If there is an error in the
    parser object then csv_fini() may not have been called or if it was it may not have reset.
    Also libcsv's manual advises against calling csv_init() on a parse object more than once:
    "memory allocated for the original structure will be lost"
    Probably if any allocated memory is freed first then it can be reinitialized without leak.
    But instead to be safer I'm recreating parse_obj.
    */
    if( parse_obj )
    {
        csv_free( parse_obj );
        delete parse_obj;
        parse_obj = NULL;
    }

    parse_obj = new csv_parser;
    if( csv_init( parse_obj, 0 /* Options must be set in Associate(), not here! */ ) )
    {
        _error = true;
        _error_msg = "libcsv: ";
        _error_msg += csv_strerror( csv_error( parse_obj ) );

        delete parse_obj;
        parse_obj = NULL;

        return false;
    }

    SetDelimiter( _delimiter );
    return true;
}


void CSVread::ResetCache()
{
    _cache = list<vector<string>>( 1, vector<string>() );
}


// REM this function is also called by Init() for initialization.
bool CSVread::Reset( bool partial_reset /* = false */ )
{
    if( !_buffer )
    {
        if( !ResizeBuffer( 4096 ) )
            return false;
    }

    if( !ResetParser() )
        return false;

    ResetCache();

    if( _input_ptr )
    {
        _input_ptr->clear();
        _input_ptr->seekg( _has_utf8_bom ? 3 : 0 );
        _eof = _input_ptr->eof();
    }
    else
    {
        _flags = CSVread::none;
        _eof = false;
        _has_utf8_bom = false;
    }

    csv_set_opts( parse_obj, ( ( _flags & process_empty_records ) ? CSV_REPALL_NL : 0 )
            | ( ( _flags & strict_mode ) ? ( CSV_STRICT | CSV_STRICT_FINI ) : 0 )
    );

    _error = false;
    _error_pending = false;
    _error_msg = "";

    if( !partial_reset )
    {
        _record_num = 0;
        _end_record_num = 0;
        _end_record_not_terminated = false;
        _fields = vector<string>();
    }

    return true;
}


bool CSVread::Close()
{
    if( _file.is_open() )
    {
        _file.close();
    }

    _input_ptr =  NULL;

    return Reset();
}


// Initialization to be called from the constructor only, first thing.
bool CSVread::Init()
{
    // The vars here must be zeroed before any possible error aborts the initialization.
    _buffer = NULL;
    _buffer_size = 0;
    parse_obj = NULL;
    _input_ptr =  NULL;

    _delimiter = (unsigned char)CSV_COMMA;

    return Reset();
}




bool CSVread::Associate( istream *stream, const Flags flags /* = none */ )
{
    if( _error )
        return false;

    if( !stream )
    {
        _error = true;
        _error_msg = "The stream parameter is NULL.";
        return false;
    }

    if( _input_ptr )
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
    _input_ptr = stream;

    if( !_input_ptr->good() )
    {
        _error = true;
        _error_msg = "istream: " + ios_strerror( _input_ptr->rdstate() );
        return false;
    }

    csv_set_opts( parse_obj, ( ( _flags & process_empty_records ) ? CSV_REPALL_NL : 0 )
            | ( ( _flags & strict_mode ) ? ( CSV_STRICT | CSV_STRICT_FINI ) : 0 )
    );

    uintmax_t pending = 1;
    uintmax_t requested = 1;

    bool parsed_end_record = false;

    cb_stuff args( _cache, _flags, _error_pending, _error_msg, _end_record_not_terminated, pending, requested );

    /* At least 3 bytes need to be read to detect the UTF-8 BOM. If the _buffer has a size of less
    than 3 then use temporary buffer a[] instead.
    */
    char a[ 3 ];
    char *p = ( _buffer_size >= 3 ) ? _buffer : a;
    streamsize p_size = ( _buffer_size >= 3 ) ? _buffer_size : (streamsize)sizeof a;

    _input_ptr->read( p, p_size );
    streamsize len = _input_ptr->gcount();
    _eof = _input_ptr->eof();

    if( len > 0 )
    {
        if( !( _flags & skip_utf8_bom_check )
            && ( len >= 3 )
            && ( p[ 0 ] == '\xEF' ) && ( p[ 1 ] == '\xBB' ) && ( p[ 2 ] == '\xBF' )
        )
        {
            _has_utf8_bom = true;
        }

        if( !_has_utf8_bom || ( len > 3 ) )
        {
            char *adjusted_p = &p[ _has_utf8_bom ? 3 : 0 ];
            size_t adjusted_len = (size_t)( _has_utf8_bom ? ( len - 3 ) : len );

            // REM the callbacks can modify most of the 'args'
            if( csv_parse(
                    parse_obj,
                    adjusted_p,
                    adjusted_len,
                    Callback_Field,
                    Callback_Record,
                    &args
                ) != adjusted_len
            )
            {
                if( !_error_pending )
                {
                    _error_pending = true;
                    _error_msg = "libcsv: ";
                    _error_msg += csv_strerror( csv_error( parse_obj ) );
                }
            }
        }
    }

    // REM this block of code is duplicated in ReadRecord()
    if( !_error_pending )
    {
        if( !_input_ptr->good() || ( len != p_size ) )
        {
            // REM the callbacks can modify most of the 'args'
            if( csv_fini( parse_obj, Callback_Field, Callback_Record, &args ) )
            {
                _error_msg = "libcsv: ";
                _error_msg += csv_strerror( csv_error( parse_obj ) );
            }
            else
            {
                _error_msg = "istream: " + ios_strerror( _input_ptr->rdstate() );

                if( _eof )
                {
                    parsed_end_record = true;
                }
            }

            _error_pending = true;
        }
    }

    // REM this block of code is duplicated in ReadRecord()
    if( parsed_end_record )
    {
        _end_record_num = pending - 1;
        // _end_record_not_terminated is handled via csv_fini() @ Callback_Record()
    }
    else
    {
        _end_record_num = 0;
        _end_record_not_terminated = false;
    }

    return true;
}


bool CSVread::Open( string filename, const Flags flags /* = none */ )
{
    if( _error )
        return false;

    if( _file.is_open() )
    {
        _error = true;
        _error_msg = "A file is already open. Call Close() to close the file.";
        return false;
    }

    if( _input_ptr )
    {
        _error = true;
        _error_msg = "A stream is already associated. Call Close() to dissociate.";
        return false;
    }

    ios::openmode mode = ( ( flags & text_mode ) ) ? 0 : ios::binary;

    _file.open( filename, mode );
    if( !_file )
    {
        _error = true;
        _error_msg = "Failed opening " + filename;
        return false;
    }

    return Associate( &_file, flags );
}




unsigned char CSVread::GetDelimiter()
{
    return _delimiter;
}


void CSVread::SetDelimiter( unsigned char delim )
{
    _delimiter = delim;

    if( parse_obj )
    {
        csv_set_delim( parse_obj, _delimiter );
    }
}




bool CSVread::ReadRecord( const uintmax_t requested_record_num /* = 0 */ )
{
    if( _error )
        return false;

    if( !_input_ptr )
    {
        _error = true;
        _error_msg = "A stream is not associated with the object.";
        return false;
    }

    if( !_cache.size() )
    {
        _error = true;
        _error_msg = "The cache list does not contain any elements";
        return false;
    }

    _eof = _input_ptr->eof();
    uintmax_t pending = _record_num + _cache.size();
    uintmax_t requested = requested_record_num;

    if( !requested )
    {
        if( _record_num == SIZE_MAX )
        {
            _error = true;
            _error_msg = "The maximum number of lines have been read (SIZE_MAX)";
            return false;
        }
        requested = _record_num + 1;
    }

    if( requested > _record_num )
    {
        if( requested < pending ) // the requested record is already in the cache
        {
            for( uintmax_t i = requested - _record_num - 1; i; --i )
            {
                _cache.pop_front();
            }

            _record_num = requested;
            _fields.swap( _cache.front() );
            _cache.pop_front();
            return true;
        }
        else if( requested > pending ) // the requested record is not in the cache
        {
            // Discard all including the pending record.
            ResetCache();
        }
        else // the requested record is the pending record
        {
            // Discard all except the pending record.
            vector<string> temp;
            temp.swap( _cache.back() );
            ResetCache();
            temp.swap( _cache.back() );
        }
    }
    else if( requested < _record_num )
    {
        /* Records can span multiple lines and since the position of each record isn't exposed by
        libcsv there's no way (short of modifying libcsv and keeping a separate cache) to seek
        directly to the requested record's position. Instead do a partial reset here. A partial
        reset does not change _fields, _record_num, _end_record_num and _end_record_not_terminated,
        which are not to be changed unless this function is successful.
        */

        if( !Reset( true ) )
        {
            // _error and _error_msg are already set by Reset() or its helpers if it failed.
            return false;
        }

        if( !_input_ptr->good() )
        {
            _error = true;
            _error_msg = "istream seek failed: " + ios_strerror( _input_ptr->rdstate() );
            return false;
        }

        pending = 1;
    }
    else // requested == _record_num
    {
        return true;
    }

    // At this point the cache does not have any complete records so error if an error is pending.
    if( _error_pending )
    {
        _error_pending = false;
        _error = true;
        return false;
    }

    if( _cache.size() != 1 )
    {
        _error = true;

        ostringstream ss;
        ss << "The cache list has an unexpected size: " << _cache.size();
        _error_msg = ss.str();

        return false;
    }

    if( !_input_ptr->good() )
    {
        _error = true;
        _error_msg = "istream: " + ios_strerror( _input_ptr->rdstate() );
        return false;
    }

    bool parsed_end_record = false;

    cb_stuff args( _cache, _flags, _error_pending, _error_msg, _end_record_not_terminated, pending, requested );

    while( ( _cache.size() == 1 ) && !_error_pending )
    {
        _input_ptr->read( _buffer, _buffer_size );
        streamsize len = _input_ptr->gcount();
        _eof = _input_ptr->eof();

        if( len > 0 )
        {
            // REM the callbacks can modify most of the 'args'
            if( csv_parse( parse_obj, _buffer, (size_t)len, Callback_Field, Callback_Record, &args ) != len )
            {
                if( !_error_pending )
                {
                    _error_pending = true;
                    _error_msg = "libcsv: ";
                    _error_msg += csv_strerror( csv_error( parse_obj ) );
                }
            }
        }

        // REM this block of code is duplicated in Associate()
        if( !_error_pending )
        {
            if( !_input_ptr->good() || ( len != _buffer_size ) )
            {
                // REM the callback can modify most of the 'args'
                if( csv_fini( parse_obj, Callback_Field, Callback_Record, &args ) )
                {
                    _error_msg = "libcsv: ";
                    _error_msg += csv_strerror( csv_error( parse_obj ) );
                }
                else
                {
                    _error_msg = "istream: " + ios_strerror( _input_ptr->rdstate() );

                    if( _eof )
                    {
                        parsed_end_record = true;
                    }
                }

                _error_pending = true;
            }
        }
    }

    // REM this block of code is duplicated in Associate()
    if( parsed_end_record )
    {
        _end_record_num = pending - 1;
        // _end_record_not_terminated is handled via csv_fini() @ Callback_Record()
    }
    else
    {
        _end_record_num = 0;
        _end_record_not_terminated = false;
    }

    if( _cache.size() == 1 )
    {
        _error = true;
        if( !_error_pending )
        {
            _error_msg = "istream: " + ios_strerror( _input_ptr->rdstate() );
        }
        return false;
    }

    _record_num = requested;
    _fields.swap( _cache.front() );
    _cache.pop_front();

    return true;
}


} // namespace util
} // namespace jay
