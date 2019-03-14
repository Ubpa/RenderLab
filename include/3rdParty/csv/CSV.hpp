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

/** Usage and design:

Classes to read/write CSV records from/to a stream.

If a bool in these classes is described as "set" without indicating true or false, it means true.


class CSVread
- A class to read comma separated values from a stream.

class CSVwrite
- A class to write comma separated values to a stream.


These classes use libcsv --a powerful well written C library-- to parse the CSV records. Libcsv will
parse binary CSV data. If you pass in a filename it is opened in binary mode unless you specify the
flag 'text_mode'. Text mode does translations which may be undesirable for your data. If you pass in
an existing istream the 'text_mode' flag is not used; you are responsible for making sure it's in
the right mode for your data.

Here is an example for CSVread:

Car car;
jay::util::CSVread csv( "filename" );
if( csv.error ) { initialization failed, handle it }
while( csv.ReadRecord() )
{
// assuming your field size is fixed at 4, check it first:
if( csv.fields.size() != 4 ) {handle it. error, continue, etc}
car.make = csv.fields[ 0 ];
car.model = csv.fields[ 1 ];
car.year = atoi( csv.fields[ 2 ] );
car.description = csv.fields[ 3 ];
Process(car);
}

For more examples refer to ..\Example\Example.sln
*/

#ifndef JAY_UTIL_CSV_HPP_
#define JAY_UTIL_CSV_HPP_

#include <stdint.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>


struct csv_parser;

namespace jay {
namespace util {



class CSVread
{
public:
    // flags for Open()/Associate()
    enum Flags
    {
        none = 0,

        /* Skip the UTF-8 BOM check.

        'has_utf8_bom' will always be false if the check is skipped.

        There is no UTF-8 processing done on the stream either way; the check is just to identify
        and then ignore the BOM if present. You may pass this flag if you are certain your CSV
        file/istream does not have a UTF-8 BOM.
        */
        skip_utf8_bom_check = 1 << 0,


        /* Process empty records.

        An empty record is a record that has nothing or only whitespace before its terminator. By
        default empty records are ignored and not processed as records.

        A record that consists of just a field separator --eg a comma `,`-- is *not* considered an
        empty record by this parser, it is parsed as two empty fields.

        Regarding its default behavior of ignoring empty records libcsv says:
        "This behavior is meant to accommodate files using only either a linefeed or a carriage
        return as a record separator to be parsed properly while at the same time being able to
        parse files with rows terminated by multiple characters from resulting in blank rows after
        each actual row of data (for example, processing a text CSV file created that was created on
        a Windows machine on a Unix machine)."

        I implemented the processing of empty records by ignoring carriage returns as a record
        terminator in that case. If you are certain all records end in LF or CRLF but never just CR,
        and you want to process empty records then you may pass this flag. An exception to the must
        have LF or CRLF rule is the end record, since it's allowed to not have any terminator.

        All empty records must be terminated to be processed. That means an end record cannot be
        unterminated AND empty, which in that unique case means the stream ends in trailing
        whitespace that is ignored regardless.

        Keep in mind if using this flag that because each empty record is now considered a record
        it will have its own record number; therefore the record number corresponding to each
        record could be different depending on whether or not this flag is passed to Open().

        Internally CSV_REPALL_NL is set for the csv parser when this flag is set.
       */
        process_empty_records = 1 << 1,


        /* Strict mode.

        By default libcsv's parser is forgiving and will parse a field even if it does not strictly
        adhere to the most common CSV usage. More details can be found in libcsv's manual and
        http://www.creativyst.com/Doc/Articles/CSV/CSV01.htm

        You may pass this flag to error when parsing any format that's outside of common usage.

        An end record missing a newline terminator is considered common usage unfortunately, and
        strict mode will not error. To handle that condition eval 'end_record_not_terminated'.

        Internally CSV_STRICT and CSV_STRICT_FINI are set for the csv parser when this flag is set.
        */
        strict_mode = 1 << 2,


        /* Error on NULL byte in field.

        If a field contains null bytes those bytes are considered part of the field by default. A
        field is exposed as std::string, and it may be undesirable to allow NULL bytes in a string.
        */
        error_on_null_in_field = 1 << 3,


        /* Open file in text mode instead of binary mode.

        Libcsv handles files in binary mode and by default files are opened in binary mode by this
        class. This should be fine for most purposes since libcsv autodetects CR,LF,CRLF as record
        terminators. However if you have records with multiline data that is not binary then you
        may prefer the translation. In that case you may associate an already existing istream that
        has that translation enabled or pass in this flag with your file.

        An error will occur if you use this flag when associating an existing istream. This is
        subject to change, should I decide to roll my own translation someday for binary streams.
        */
        text_mode = 1 << 4
    };


    /* Constructor

    The constructor calls Init().
    The constructor also calls Open() if a filename is specified.
    The constructor also calls Associate() if an istream is specified.

    In any case check 'error' to determine whether or not construction succeeded.

    [in] 'filename' : A file to open for input.
    [in] 'stream' : An istream already opened for input.
    [in][opt] 'flags' : Refer to CSVread::Flags. The default is no flags are set.
    */
    CSVread();
    CSVread( std::string filename, Flags flags = none );
    CSVread( std::istream *stream, Flags flags = none );

    // If a file was opened by this class it's automatically closed when the class destructs.
    ~CSVread();


    /* CSVread::Reset()
    - Reset to an empty state.

    Resets most variables, calls ResetParser() and ResetCache().

    The delimiter is not reset. To reset the delimiter call SetDelimiter( ',' ).
    The size of the buffer is not reset. To reset the size of the buffer call ResizeBuffer().

    If the file/istream is open/associated it's clear()'d, then its position is reset. If the
    file/istream is not seekable do not call this function. The return code is undefined if the
    stream is not seekable. Instead call Close() which will close/dissociate first before it calls
    this function.

    It shouldn't be necessary to call this function, typically. If there was an error and you wanted
    to reset without associating/dissociating your istream, or opening/closing your file, you would
    call this function.

    [in] 'partial_reset' : A partial reset does everything described above but does not the reset
        'fields', 'record_num', 'end_record_num', 'end_record_not_terminated'. Typically there
        should be no reason to set this true. The default is false (a full reset).
    [ret][failure] (false) : 'error' and 'error_msg' have been set.
    [ret][success] (true)
    */
    bool Reset( bool partial_reset = false );


    // Closes file if open or dissociates the existing istream, and then calls Reset().
    // If this returns false the file/istream has been closed/dissociated but Reset() failed.
    bool Close();

    // This is the same as Close(). It may make your code easier to understand to call this when you
    // are dissociating a stream not created by the class because it's not closed just dissociated.
    bool Dissociate() { return Close(); };


    /* CSVread::Open(), CSVread::Associate()
    - Open a file or associate an existing istream.

    If there is already an open file or associated istream this function fails.

    If this function fails for any reason you must call Close() to reset before trying again.

    When the class destructs if there is a file that was opened by this function it is automatically
    closed, but you may call Close() before then.

    If the stream starts with a UTF-8 BOM those bytes are ignored unless flag
    CSVread::skip_utf8_bom_check is passed. There is no UTF-8 processing done on the stream
    either way; the check is just to identify and then ignore the BOM if present.

    Once the stream has been opened you may call ReadRecord() to retrieve each record.

    The beginning of the file/istream current position must be the beginning of a record with an
    optional UTF8 BOM. Keep in mind the first record read is 'record_num' 1.

    If the file/istream is not seekable there are some limitations in ReadRecord().

    [in] 'filename' : A file to open for input.
    [in] 'stream' : An istream already opened for input.
    [in][opt] 'flags' : Refer to CSVread::Flags. The default is no flags are set.
    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true) : 'has_utf8_bom' may be set.
    */
    bool Open( std::string filename, Flags flags = none );
    bool Associate( std::istream *stream, Flags flags = none );


    /* CSVread::GetDelimiter(), CSVread::SetDelimiter()
    - Get or set the delimiter character to be used when parsing the stream.

    The default delimiter is a comma and does not need to be set.

    The delimiter is also known as the field separator character. Typically SetDelimiter() would be
    used to set the delimiter to a semi-colon instead of the default comma.

    The delimiter character must not conflict with any whitespace, terminator or qualifier (default:
    double quote) character. No error checking is done because the advanced features of libcsv allow
    all of those to be changed and therefore in rare circumstances one of those characters could be
    technically acceptable as the delimiter.

    The delimiter is persistent and will survive resets. It doesn't need to be set on each open.
    */
    unsigned char GetDelimiter();
    void SetDelimiter( unsigned char delim );


    /* CSVread::ReadRecord()
    - Read and parse a record.

    If the requested record number 'requested_record_num' is specified (ie not the default 0) and it
    is less than the current record number 'record_num' then this function has to parse the stream
    from the beginning to get to the requested record and is inefficient. In addition if the stream
    is not seekable this function will fail in that case.

    The end record is not known until this function attempts to read past it, the same way the EOF
    of a stream is not known until you attempt to read past its end. Since records are parsed and
    cached in advance the end record may or may not be known when you successfully request a record,
    including what may be the end record. For instructions on testing for the end record refer to
    the comment block above the declaration for 'end_record_num'.

    'eof', 'end_record_num' or 'end_record_not_terminated' set on success is not an indication that
    the record returned is the end record. You should keep calling ReadRecord(), until it fails, to
    read any remaining records in the cache.

    [in][opt] 'requested_record_num' : The record number to read. The default is the next record.
    [ret][failure] (false) : 'error' and 'error_msg' are set;
        'eof', 'end_record_num' and 'end_record_not_terminated' may also be set.
    [ret][success] (true) : 'record_num' and 'fields' are set;
        'eof', 'end_record_num' and 'end_record_not_terminated' may also be set.
    */
    bool ReadRecord( const uintmax_t requested_record_num = 0 );


    /* Change the size of the buffer, in bytes.

    The buffer exists for the life of the object. It has a default size of 4096 bytes and is used to
    hold data read from the stream.

    The cache list may allocate the same amount of memory as the size of the buffer at any time.

    Libcsv has its own buffer that is unaffected by this setting.

    [ret][failure] (false) : The buffer could not be resized and has retained its current size.
        'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool ResizeBuffer( const std::streamsize bytes );


    // The size of _buffer. Default 4096. Call ResizeBuffer() to change the size.
    const std::streamsize &buffer_size; // = _buffer_size

    /* Stream EOF.

    EOF is just an indication that the end of the stream was reached. As noted in ReadRecord(), EOF
    does not indicate the state of the records. If you want to determine whether the current record
    is the end record refer to 'end_record_num'.
    */
    const bool &eof; // = _eof

    // Error. Functions will not succeed when this is true. Call Reset() or Close() or Dissociate().
    const bool &error; // = _error

    // Contains an error message when 'error' or '_pending_error'.
    // If 'error' you can read this string before calling Reset() or Close() or Dissociate().
    const std::string &error_msg; // = _error_msg

    // File/istream has a UTF-8 BOM
    const bool &has_utf8_bom; // = _has_utf8_bom

    // The record number of the current record.
    // The first CSV record is record number 1.
    const uintmax_t &record_num; // = _record_num


    /* The record number of the end record.

    This is set if 'eof' is true and all records were parsed successfully.

    The value of this variable is undefined when 'eof' is false.

    In order to parse the end record, every record before it will have been parsed successfully,
    even if those records were ignored (eg you skipped them). Therefore when this is set all
    records were parsed successfully.

    Testing for the end record should come after a call to ReadRecord() fails, not before. There is
    no guarantee on ReadRecord() success that the record read is known to be the end record even if
    it is actually the end record.

    // process all records including the end record
    while( csv_read.ReadRecord() )
    {
    }

    // determine if the end record was the last successfully read
    if( obj.eof && ( obj.record_num == obj.end_record_num ) )
    {
    }

    The example above assumes you are reading consecutively. Assuming the if statement is true,
    since ReadRecord() does not set 'fields' or 'record_num' on failure they would still represent
    the end record in that case.
    */
    const uintmax_t &end_record_num; // = _end_record_num


    /* The end record was not terminated.

    This is true if 'end_record_num' is set and the end record was not terminated.

    Common usage of the CSV format does not require the end record to be terminated using a newline
    sequence, defined as LF, CRLF or just CR (unless flag 'process_empty_records' was specified).
    Despite this it's strongly recommended to always terminate in a newline and correct the
    condition where it's missing. If you have this condition and something appends records to your
    CSV file it likely will not check earlier records, so what was previously the end record in your
    file and the first appended record would become one corrupted record.
    */
    const bool &end_record_not_terminated; // = _end_record_not_terminated


    // The current record parsed into fields (columns), eg fields[0], fields[1], etc.
    // Depending on your CSV data the number of fields may or may not differ between records.
    // You can check fields.size() after each ReadRecord() to confirm the number is what's expected.
    const std::vector<std::string> &fields; // = _fields


    /* The libcsv parse object.

    You may use this to set more advanced behavior.

    To access this object you must include libcsv's "csv.h" in your code.

    Do not call these functions on parse_obj:
    csv_init(), csv_set_opts(), csv_parse(), csv_fini(), csv_free(), csv_get_delim(),
    csv_set_delim().

    To get/set the delimiter call Get/SetDelimiter() instead.

    If 'error' parse_obj is not guaranteed != NULL or a good state; don't call any libcsv function.
    */
    struct ::csv_parser *parse_obj;


private:
    CSVread( const CSVread & );
    CSVread & operator=( const CSVread & );

    // Call this to reset parse_obj.
    bool ResetParser();

    // A file stream if one was opened by this class.
    std::ifstream _file;

    // The stream the records are read from.
    // This points to the user specified istream or _file.
    std::istream *_input_ptr;

    // A list of the records (each as a vector containing the fields) most recently parsed.
    // The back element is always used for the pending record. The size of the cache should never
    // be less than 1.
    // To clear this list call ResetCache(), which starts a new list with an empty back element.
    std::list<std::vector<std::string>> _cache;

    // Call this to reset _cache.
    // The new list starts with one empty vector as the pending record.
    void ResetCache();

    // Whether or not an error is pending. Sometimes this is set instead of '_error' if there are
    // completed records still in the cache when an error occurs. When the completed records are
    // emptied if '_error_pending' then '_error' is set. See ReadRecord() definition.
    bool _error_pending;

    // The flags passed to Open()/Associate().
    Flags _flags;

    // The buffer used to hold data read from the stream.
    char *_buffer;

    // The field separator character.
    unsigned char _delimiter; // = ,

    // For a description of any of these refer to their public const references.
    std::streamsize _buffer_size;
    bool _eof;
    bool _error;
    std::string _error_msg;
    bool _has_utf8_bom;
    uintmax_t _record_num;
    uintmax_t _end_record_num;
    bool _end_record_not_terminated;
    std::vector<std::string> _fields;

    // Initialization to be called from the constructor only.
    bool Init();
};

inline CSVread::Flags operator | (CSVread::Flags a, CSVread::Flags b)
{
    return CSVread::Flags( ( (int)a ) | ( (int)b ) );
}
inline CSVread::Flags & operator |= (CSVread::Flags &a, CSVread::Flags b)
{
    return (CSVread::Flags &)( ( (int &)a ) |= ( (int)b ) );
}



class CSVwrite
{
public:
    // flags for Open()/Associate()
    enum Flags
    {
        none = 0,

        /* Truncate file.

        Any contents that existed in the file before it is open are discarded.

        An error will occur if you use this flag when associating an existing ostream. This is
        subject to change, should I decide to handle discarding data in an existing ostream.
        */
        truncate = 1 << 0,

        /* Open file in text mode instead of binary mode.

        Records are written in binary mode by default. That means that characters are
        untranslated.

        You may prefer the translation done by text mode. In that case you may associate an already
        existing stream that has that translation enabled or pass in this flag with your file.

        An error will occur if you use this flag when associating an existing ostream. This is
        subject to change, should I decide to roll my own translation someday for binary streams.
        */
        text_mode = 1 << 1,

        /* Process empty records.

        By default empty records (no fields) are ignored and not processed as records.

        Empty records are completely empty. If you choose to process empty records only a
        terminator is written for the empty record.

        This flag changes the behavior of WriteRecord() but not WriteTerminator(). If you call
        WriteTerminator() without first writing a field or record then you have written an empty
        record, regardless of whether or not this flag is used.
       */
        process_empty_records = 1 << 2
    };


    /* Constructor

    The constructor calls Init().
    The constructor also calls Open() if a filename is specified.
    The constructor also calls Associate() if an ostream is specified.

    In any case check 'error' to determine whether or not construction succeeded.

    [in] 'filename' : A file to open for output.
    [in] 'stream' : An ostream already opened for output.
    [in][opt] 'flags' : Refer to CSVwrite::Flags. The default is no flags are set.
    */
    CSVwrite();
    CSVwrite( std::string filename, Flags flags = none );
    CSVwrite( std::ostream *stream, Flags flags = none );

    // If a file was opened by this class it's automatically closed when the class destructs.
    ~CSVwrite();


    // Closes file if open or dissociates the existing ostream, and then calls Reset().
    // If this returns false the file/ostream has been closed/dissociated but Reset() failed.
    bool Close();

    // This is the same as Close(). It may make your code easier to understand to call this when you
    // are dissociating a stream not created by the class because it's not closed just dissociated.
    bool Dissociate() { return Close(); };


    /* CSVwrite::Open(), CSVwrite::Associate()
    - Open a file or associate an existing ostream.

    A file is opened in append mode by default. To truncate the file instead pass the flag
    CSVwrite::truncate.

    If there is already an open file or associated istream this function fails.

    If this function fails for any reason you must call Close() to reset before trying again.

    When the class destructs if there is a file that was opened by this function it is automatically
    closed, but you may call Close() before then.

    If you are writing UTF-8 data and are writing from the beginning of the file/ostream and need
    the UTF-8 BOM call WriteUTF8BOM() after opening.

    Once the stream has been opened you may call WriteRecord() to write each record or WriteField()
    to write fields one at a time.

    The current position of the file/ostream must be the beginning of a record with an optional UTF8
    BOM if it's the beginning of the file/ostream. If you are appending it's important that the end
    record already in the file/ostream is terminated, otherwise there will be corruption when
    appending the first record. For more on this refer to the comment block above the declaration
    for CSVread::end_record_not_terminated.

    [in] 'filename' : A file to open for output.
    [in] 'stream' : An ostream already opened for output.
    [in][opt] 'flags' : Refer to CSVwrite::Flags. The default is no flags are set.
    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool Open( std::string filename, Flags flags = none );
    bool Associate( std::ostream *stream, Flags flags = none );


    /* CSVwrite::WriteUTF8BOM()
    - Write a UTF-8 BOM.

    If at the beginning of a file/ostream you can call this to write a UTF-8 BOM, if necessary.

    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool WriteUTF8BOM();


    /* CSVwrite::WriteTerminator()
    - Write a record terminator.

    If you are writing fields individually and you do not use the terminate option, or if you are
    writing a record and you do not use the terminate option, you can terminate the record by
    calling this function.

    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool WriteTerminator();


    /* CSVwrite::WriteField()
    - Write a field.

    When writing the last field in the record set 'terminate' true or call WriteTerminator().

    Every field is automatically qualified with quotes; you do not need to add your own qualifiers.

    [in] 'field' : The field.
    [in][opt] 'terminate' : Terminate the record. The default is false.
    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool WriteField( const std::string &field, const bool terminate = false );


    /* CSVwrite::WriteRecord()
    - Write a record.

    All records should be terminated. If you set 'terminate' false make sure to call
    WriteTerminator().

    If this function is called and the previous record written by either this function or
    WriteField() wasn't terminated then it is terminated before processing 'fields'.

    If 'fields' has a size of 0 (no fields- empty record) nothing is written unless
    flag CSVwrite::process_empty_records was specified, in which case a terminator is written.

    Every field is automatically qualified with quotes; you do not need to add your own qualifiers.

    [in] 'fields' : The record.
    [in][opt] 'terminate' : Terminate the record. The default is true.
    [ret][failure] (false) : 'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool WriteRecord( const std::vector<std::string> &fields, const bool terminate = true );


    /* Change the size of the buffer, in bytes.

    The buffer exists for the life of the object. It has a default size of 4096 bytes and is used to
    hold data before it's written to the stream.

    The buffer will hold at most a single field before writing it, therefore it's not as useful to
    change the buffer size for this as it is for CSVread's buffer, unless your fields typically
    are greater than 4096 bytes.

    [ret][failure] (false) : The buffer could not be resized and has retained its current size.
        'error' and 'error_msg' are set.
    [ret][success] (true)
    */
    bool ResizeBuffer( const std::streamsize bytes );


    // The size of _buffer. Default 4096. Call ResizeBuffer() to change the size.
    const std::streamsize &buffer_size; // = _buffer_size

    // Error. Functions will not succeed when this is true. Call Close() or Dissociate().
    const bool &error; // = _error

    // Contains an error message when 'error'.
    // If 'error' you can read this string before calling Close() or Dissociate().
    const std::string &error_msg; // = _error_msg

    // Set this to change the delimiter.
    // The delimiter should be a single character but prepended/appended whitespace is ok.
    // The delimiter is persistent and will survive resets. It doesn't need to be set on each open.
    std::string delimiter; // = ,

    // Set this to change the terminator.
    // The terminator should be a single character or \r\n but prepended/appended whitespace is ok.
    // \n could be automatically translated and written as \r\n in text mode, depending on your OS.
    // It's not recommended to set the terminator as \r\n explicitly.
    // The terminator is persistent and will survive resets. It doesn't need to be set on each open.
    std::string terminator; // = \n

private:
    CSVwrite( const CSVwrite & );
    CSVwrite & operator=( const CSVwrite & );

    // A file stream if one was opened by this class.
    std::ofstream _file;

    // The stream the records are written to.
    // This points to the user specified ostream or _file.
    std::ostream *_output_ptr;

    // The flags passed to Open()/Associate().
    Flags _flags;

    // The buffer used to hold data written to the stream.
    char *_buffer;

    // Whether or not the field to be written is the first field in the record.
    bool _is_first_field;

    // For a description of any of these refer to their public const references.
    std::streamsize _buffer_size;
    bool _error;
    std::string _error_msg;

    // Initialization to be called from the constructor only.
    bool Init();

    // Resets most variables. Does not reset the buffer size, delimiter or terminator.
    bool Reset();
};

inline CSVwrite::Flags operator | (CSVwrite::Flags a, CSVwrite::Flags b)
{
    return CSVwrite::Flags( ( (int)a ) | ( (int)b ) );
}
inline CSVwrite::Flags & operator |= (CSVwrite::Flags &a, CSVwrite::Flags b)
{
    return (CSVwrite::Flags &)( ( (int &)a ) |= ( (int)b ) );
}


} // namespace util
} // namespace jay
#endif // JAY_UTIL_CSV_HPP_
