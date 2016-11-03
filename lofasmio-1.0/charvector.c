// 2>&-### SELF-EXTRACTING DOCUMENTATION ###############################
// 2>&-#                                                               #
// 2>&-# Run "bash <thisfile> > <docfile.md>" to extract documentation #
// 2>&-#                                                               #
// 2>&-#################################################################
// 2>&-; awk '/^\/\* <BEGIN>/{f=1;next}/^<END> \*\//{f=0}f' $0; exit 0

/***********************************************************************
charvector.c
Copyright (c) 2016 Teviet Creighton.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "charvector.h"

/* <BEGIN>
# charvector_insert(3) 2016-07-13

## NAME

`charvector_insert` - insert string into a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_insert( charvector *`_dst_`, const char *`_src_`,
                         size_t` _offset_ `);`

## DESCRIPTION

Inserts a nul-terminated string _src_ into _dst_`->str` starting at
byte _offset_.  The trailing portion of _dst_`->str` is not
overwritten but pushed back to follow the inserted string; if
necessary, _dst_`->str` is reallocated and _dst_`->size` is increased.
See charvector(5) for more details.

The value of _offset_ must be less than or equal to than the length of
_dst_`->str`. as reported by strlen(3) (which in turn will be less
than the allocated size _dst_`->size`).  If _offset_ is too large, an
error is returned.

## RETURN VALUE

On success, returns the string _dst_`->str` with the insertion.  If
passed bad arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

charvector_append(3),
charvector_append_c(3),
charvector_append_printf(3),
charvector_append_vprintf(3),
charvector(5)

<END> */
char *charvector_insert( charvector *dst, const char *src, size_t offset )
{
  int n, m; /* lengths of dst->str and src */

  /* Do error checking and get lengths. */
  if ( !dst || ( dst->str && dst->str[dst->size-1] ) )
    return NULL;
  n = ( dst->str ? strlen( dst->str ) : 0 );
  if ( offset > n )
    return NULL;
  if ( !src || !src[0] )
    return dst->str;
  m = strlen( src );

  /* Reallocate space if needed. */
  if ( n + m + 1 > dst->size ) {
    size_t s = CHARVECTOR_BLOCKSIZE;
    char *tmp;
    while ( s < n + m + 1 )
      s *= 2;
    if ( !( tmp = (char *)( calloc( s, 1 ) ) ) )
      return NULL;
    if ( offset > 0 )
      memcpy( tmp, dst->str, offset );
    memcpy( tmp + offset, src, m );
    if ( n - offset > 0 )
      memcpy( tmp + offset + m, dst->str + offset, n - offset );
    if ( dst->str )
      free( dst->str );
    dst->str = tmp;
    dst->size = s;
    return tmp;
  }

  /* Otherwise insert in-place. */
  if ( n - offset > 0 )
    memmove( dst->str + offset + m, dst->str + offset, n - offset );
  memcpy( dst->str + offset, src, m );
  return dst->str;
}

/* <BEGIN>
# charvector_append(3) 2016-07-13

## NAME

`charvector_append` - append string to a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_append( charvector *`_dst_`, const char *`_src_ `);`

## DESCRIPTION

Attaches a nul-terminated string _src_ to the end of _dst_`->str`,
reallocating _dst_`->str` and increasing _dst_`->size` as needed.

## RETURN VALUE

On success, returns the expanded string _dst_`->str`.  If passed bad
arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

charvector_append_c(3),
charvector_insert(3),
charvector_append_printf(3),
charvector_append_vprintf(3),
charvector(5)

<END> */
char *charvector_append( charvector *dst, const char *src )
{
  int n, m; /* lengths of dst->str and src */

  /* Do error checking and get lengths. */
  if ( !dst || ( dst->str && dst->str[dst->size-1] ) )
    return NULL;
  n = ( dst->str ? strlen( dst->str ) : 0 );
  if ( !src || !src[0] )
    return dst->str;
  m = strlen( src );

  /* Reallocate space if needed. */
  if ( n + m + 1 > dst->size ) {
    size_t s = CHARVECTOR_BLOCKSIZE;
    char *tmp;
    while ( s < n + m + 1 )
      s *= 2;
    if ( !( tmp = (char *)( calloc( s, 1 ) ) ) )
      return NULL;
    memcpy( tmp, dst->str, n );
    memcpy( tmp + n, src, m );
    if ( dst->str )
      free( dst->str );
    dst->str = tmp;
    dst->size = s;
    return tmp;
  }

  /* Otherwise insert in-place. */
  memcpy( dst->str + n, src, m );
  return dst->str;
}

/* <BEGIN>
# charvector_append_c(3) 2016-07-13

## NAME

`charvector_append_c` - append single character to a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_append_c( charvector *`_dst_`, char` _c_ `);`

## DESCRIPTION

Adds a single character to the end of _dst_`->str`, reallocating
_dst_`->str` and increasing _dst_`->size` as needed.

## RETURN VALUE

On success, returns the expanded string _dst_`->str`.  If passed bad
arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

charvector_append(3),
charvector_insert(3),
charvector_append_printf(3),
charvector_append_vprintf(3),
charvector(5)

<END> */
char *charvector_append_c( charvector *dst, char c )
{
  int n; /* length of dst->str */

  /* Do error checking and get lengths. */
  if ( !dst || ( dst->str && dst->str[dst->size-1] ) )
    return NULL;
  n = ( dst->str ? strlen( dst->str ) : 0 );

  /* Reallocate space if needed. */
  if ( n + 2 > dst->size ) {
    size_t s = CHARVECTOR_BLOCKSIZE;
    char *tmp;
    while ( s < n + 2 )
      s *= 2;
    if ( !( tmp = (char *)( calloc( s, 1 ) ) ) )
      return NULL;
    memcpy( tmp, dst->str, n );
    tmp[n] = c;
    if ( dst->str )
      free( dst->str );
    dst->str = tmp;
    dst->size = s;
    return tmp;
  }

  /* Otherwise insert in-place. */
  dst->str[n] = c;
  return dst->str;
}

/* <BEGIN>
# charvector_append_vprintf(3) 2016-07-13

## NAME

`charvector_append_vprintf` - append formatted output to a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_append_vprintf( charvector *`_dst_`, const char *`_format_`,
                                 va_list` _list_ `);`

## DESCRIPTION

Appends a string formatted according to printf(3) to the end of
_dst_`->str`, reallocating _dst_`->str` and increasing _dst_`->size`
as needed.

Internally, this routine calls vasprintf(3) to generate the formatted
string, then charvector_append(3) to attach it.  The variable-length
argument list to be formatted is passed as a `va_list` object; the
calling routine is responsible for calling va_start(3) and va_end(3).

## RETURN VALUE

On success, returns the expanded string _dst_`->str`.  If passed bad
arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

charvector_append(3),
charvector_append_printf(3),
vasprintf(3),
va_end(3),
va_start(3),
charvector(5)

<END> */
char *charvector_append_vprintf( charvector *dst, const char *format,
				 va_list list )
{
  char *str = NULL;
  int ret = vasprintf( &str, format, list );
  if ( ret < 0 || !charvector_append( dst, str ) ) {
    if ( str )
      free( str );
    return NULL;
  }
  free( str );
  return dst->str;
}

/* <BEGIN>
# charvector_append_printf(3) 2016-07-13

## NAME

`charvector_append_printf` - append formatted output to a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_append_printf( charvector *`_dst_`, const char *`_format_`,
                                ... );`

## DESCRIPTION

Appends a string formatted according to printf(3) to the end of
_dst_`->str`, reallocating _dst_`->str` and increasing _dst_`->size`
as needed.

Internally, this routine calls va_start(3),
charvector_append_vprintf(3), and va_end(3) to process the
variable-length argument list.  The charvector_append_vprintf(3)
function in turn calls vasprintf(3) to generate the formatted string,
then charvector_append(3) to attach it.

## RETURN VALUE

On success, returns the expanded string _dst_`->str`.  If passed bad
arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

charvector_append(3),
charvector_append_vprintf(3),
vasprintf(3),
va_end(3),
va_start(3),
charvector(5)

<END> */
char *charvector_append_printf( charvector *dst, const char *format, ... )
{
  char *ret;
  va_list list;
  va_start( list, format );
  ret = charvector_append_vprintf( dst, format, list );
  va_end( list );  
  return ret;
}

/* <BEGIN>
# charvector_replace(3) 2016-07-13

## NAME

`charvector_replace` - find and replace substrings in a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_replace( charvector *`_dst_`, const char *`_find_`,
                          const char *`_replace_ `);`

## DESCRIPTION

Searches _dst_`->str` for all occurences of the string _find_,
replacing each in turn with the string _replace_.  Both _find_ and
_replace_ must be nul-terminated strings.

If necessary, _dst_`->str` will be reallocated and _dst_`->size`
increased to accomodate the modified string.  This will only occur if
_replace_ is longer than _find_, and maybe not even then if the
allocated space is large enough to begin with.

Internally, the function steps through _dst_`->str`, using strncmp(3)
to check for matches to _find_, after first checking that _dst_`->str`
is safely nul-terminated.  It does not (indeed cannot) check that
_find_ and _replace_ are valid nul-terminated strings.  If _replace_
is longer than _find_, the function will step through _dst_`->str`
twice, once to compute the needed size and again to perform the
replacement.  This may be somewhat slower in cases of large strings
with few matches, but scales much better when there are many matches.

## RETURN VALUE

On success, returns the expanded string _dst_`->str`.  If passed bad
arguments, or if memory reallocation fails, returns NULL.

## SEE ALSO

strncmp(3),
charvector(5)

<END> */
char *charvector_replace( charvector *dst, const char *find,
			  const char *replace )
{
  int l, m, n;       /* lengths of dst->str, find, and replace */
  char *srcp, *dstp; /* pointers within source and destination */
  char *tmp = NULL;  /* temporary storage for reallocated string */
  size_t s = CHARVECTOR_BLOCKSIZE; /* size of reallocated string */

  /* Do error checking and get lengths. */
  if ( !dst || !dst->str || dst->str[dst->size-1] || !find || !find[0] )
    return NULL;
  m = strlen( find );
  n = ( replace ? strlen( replace ) : 0 );

  /* Allocate new destination string if necessary. */
  if ( n > m ) {
    for ( l = 0, srcp = dst->str; *srcp; )
      if ( !strncmp( srcp, find, m ) ) {
	l++;
	srcp += m;
      } else
	srcp++;
    l = strlen( dst->str ) + l*( n - m ) + 1;
    while ( s < l )
      s *= 2;
    if ( !( tmp = (char *)( calloc( s, 1 ) ) ) )
      return NULL;
  }

  /* Do replacement. */
  for ( srcp = dst->str, dstp = ( tmp ? tmp : dst->str ); *srcp; )
    if ( !strncmp( srcp, find, m ) ) {
      if ( replace )
	memcpy( dstp, replace, n );
      srcp += m;
      dstp += n;
    } else
      *( dstp++ ) = *( srcp++ );

  /* Set return values. */
  if ( tmp ) {
    free( dst->str );
    dst->str = tmp;
    dst->size = s;
  } else if ( ( l = dstp - dst->str ) < dst->size - 1 )
    memset( dstp, 0, dst->size - 1 - l );
  return dst->str;
}

/* <BEGIN>
# charvector_pad(3) 2016-07-13

## NAME

`charvector_pad` - zero out the unused portion of a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_pad( charvector *`_cv_ `);`

## DESCRIPTION

Checks that _cv_`->str` is a nul-terminated string shorter than
_cv_`->size`, and sets to nul all the bytes from the end of the string
to the allocated size.

This function is useful for "sanitizing" user input that might
otherwise contain leftover random memory content.  However,
charvectors created and manipulated solely by the routines in this
module should automatically be nul-padded without aving to call this
routine.  Also, of course, the routine has no way of verifying that
the provided _dst_`->size` is in fact the size of the block allocated
to _dst_`->str`.

This routine does not change the space allocated to _cv_; use
charvector_resize(3) or charvector_trim(3) for that.

## RETURN VALUE

On success, returns _dst_`->str`.  If passed bad arguments (in
particular if the last byte of the char array was not nul to begin
with), returns NULL.

## SEE ALSO

charvector_resize(3),
charvector_trim(3),
memset(3),
charvector(5)

<END> */
char *charvector_pad( charvector *cv )
{
  int n;
  if ( !cv || !cv->str || cv->str[cv->size-1] )
    return NULL;
  if ( ( n = strlen( cv->str ) ) < cv->size - 2 )
    memset( cv->str + n + 1, 0, cv->size - n - 2 );
  return cv->str;
}


/* <BEGIN>
# charvector_trim(3) 2016-07-13

## NAME

`charvector_trim` - deallocate unneded space in a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_trim( charvector *`_cv_ `);`

## DESCRIPTION

Checks that _cv_`->str` is a nul-terminated string shorter than
_cv_`->size`, reallocates the memory to the string length plus 1 (to
include the nul terminator), and sets _cv_`->size` accordingly.

This function is useful for reducing the memory footprint if string
manipulations have shortened a string dramatically, or if you just
want to eliminate the padding (up to 2x) that arises from calls to
charvector_append(3) and related functions.  Use charvector_resize(3)
to expand the allocated size, or shorten it to a specified size.

Note that if _cv_`->str` is a non-NULL but empty string (i.e. if
_cv_`->str[0]` is nul), then the routine will free the string
altogether and set _cv_`->size` to zero.

## RETURN VALUE

On success, returns _dst_`->str`.  If passed bad arguments (in
particular if the last byte of the char array was not nul to begin
with), or if reallocation fails, returns NULL.

## SEE ALSO

charvector_resize(3),
charvector_trim(3),
realloc(3),
charvector(5)

<END> */
char *charvector_trim( charvector *cv )
{
  int n;
  if ( !cv || !cv->str || cv->str[cv->size-1] )
    return NULL;
  if ( ( n = strlen( cv->str ) ) > 0 && n < cv->size - 1 ) {
    char *tmp = (char *)realloc( cv->str, n + 1 );
    if ( !tmp )
      return NULL;
    cv->str = tmp;
    cv->size = n + 1;
  } else if ( n == 0 ) {
    free( cv->str );
    memset( cv, 0, sizeof(charvector) );
  }
  return cv->str;
}

/* <BEGIN>
# charvector_resize(3) 2016-07-13

## NAME

`charvector_resize` - set allocated space in a charvector(5)

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_resize( charvector *`_cv_`, size_t` _size_`, int` _truncate_ `);`

## DESCRIPTION

Reallocates _cv_`->str` to the specified _size_, and sets _cv_`->size`
accordingly.  If _cv_`->str` is longer than _size_-1, the behaviour is
governed by the _truncate_ flag: if _truncate_ is nonzero (true), then
the requested _size_ will be honored, and _cv_`->str` will be
shortened and truncated with a nul byte at index _size_-1; whereas if
_truncate_ is zero (false), then the size will not be less than the
length of _cv_`->str` plus its nul terminator (equivalent to
charvector_trim(3)).

If _size_ is zero and _truncate_ is nonzero, this routine is
equivalent to charvector_free(3).  Otherwise, if _cv_`->str` is NULL
to begin with, then it is allocated; if it is non-NULL, then it must
be a nul-terminated string shorter than its own specified
_cv_`->size`, or the routine will fail.  The routine ensures that, on
successful return, _cv_`->str` is shorter than _cv_`->size`, and
nul-padded between the end of the string and the end of the allocated
memory.

Given that the goal of the charvector(5) routines is to allow strings
to resize automatically as needed, it may seem counterproductive for
the user to request a specific size.  However, it may be useful if the
user knows in advance approximately (but not certainly) how much space
will be needed, or if a specific size is expected by downstream
functions.

## RETURN VALUE

On success, returns _dst_`->str`.  If passed bad arguments (in
particular if the last byte of the char array was not nul to begin
with), or if (re)allocation fails, returns NULL.

## SEE ALSO

charvector_free(3),
charvector_trim(3),
memset(3),
realloc(3),
charvector(5)

<END> */
char *charvector_resize( charvector *cv, size_t size, int truncate )
{
  int n;
  if ( !cv || ( cv->str && cv->str[cv->size-1] ) )
    return NULL;
  if ( !cv->str && size > 0 ) {
    cv->str = (char *)calloc( size, 1 );
    cv->size = ( cv->str ? size : 0 );
    return cv->str;
  }
  if ( ( n = strlen( cv->str ) ) > cv->size - 1 && !truncate )
    size = n + 1;
  if ( size > 0 && size != cv->size ) {
    char *tmp = (char *)realloc( cv->str, size );
    if ( !tmp )
      return NULL;
    if ( n < size - 1 )
      memset( tmp + n + 1, 0, size - n - 1 );
    else
      tmp[size-1] = '\0';
    cv->str = tmp;
    cv->size = size;
  } else if ( size == 0 ) {
    free( cv->str );
    memset( cv, 0, sizeof(charvector) );
  }
  return cv->str;
}
