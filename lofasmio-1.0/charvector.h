// 2>&-### SELF-EXTRACTING DOCUMENTATION ###############################
// 2>&-#                                                               #
// 2>&-# Run "bash <thisfile> > <docfile.md>" to extract documentation #
// 2>&-#                                                               #
// 2>&-#################################################################
// 2>&-; awk '/^\/\* <BEGIN>/{f=1;next}/^<END> \*\//{f=0}f' $0; exit 0

/***********************************************************************
charvector.h
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

#ifndef _CHARVECTOR_H
#define _CHARVECTOR_H
#ifdef  __cplusplus
extern "C" {
#if 0
};
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define CHARVECTOR_BLOCKSIZE 1024

/* <BEGIN>
# charvector(5) 2016-07-13

## NAME

`charvector` - a resizeable string structure

## SYNOPSIS

`#include "charvector.h"`

`typedef struct {  
   char *str;  
   size_t size;  
 } charvector;`

## DESCRIPTION

This structure stores a dynamically-allocated character array along
with its allocated size.  The functions and macros provided in
`charvector.h` manipulate such structures to write and modify strings,
resizing them as necessary.

There is no explicit routine to initialize a charvector.  It should be
initialized to an empty structure, which is a valid input to routines
such as charvector_append(3):

    charvector cv = {};
    charvector_append( &cv, "Hello, world!" );

This is not intended as an opaque type: the user is free to access,
manipulate, and free the structure elements directly.  However, the
user must then ensure that:

1. The `str` element is an allocated freeable pointer (or NULL).

2. The `size` element is not greater than the allocated size.

3. The value of `str`[`size`-1] is `\0`.

The routines in `charvector.h` will usually test the third condition
to ensure that the string is in fact nul-terminated.  When allocating
new memory, these routines will also pad all allocated space following
the nul terminator of `str` with nul bytes: this is a security feature
to prevent random system memory from appearing in the array.  The
routines do not automatically enforce this padding condition on
user-supplied strings, unless charvector_pad(3) is called.

When a charvector function needs to increase the allocated size of the
character array, it will generally increase it to a power of 2 times a
basic size stored in the `#define`d constant `CHARVECTOR_BLOCKSIZE`.
This means that the allocated size will typically be no more than 2
times what is needed; while the number of allocations required to
build a large string incrementally scales logarithmically with its
size.  If necessary, charvector_trim(3) can be called to remove excess
padding.

To accommodate coding styles where structure elements are only
accessed through functions, we provide the `charvector_str(3)`,
`charvector_size(3)`, and `charvector_free(3)` macros.  Using these
may make your code more portable (e.g. if custom memory handlers are
hooked in).

### Charvectors Are Not Gstrings

The `GString` structure and functions defined in `glib.h` provide much
greater functionality than charvectors, but also make up a substantial
non-standard external dependency on any program using them.  By
contrast, charvectors are a lightweight substitute, with only one
module and header file, meant to be included within other projects
rather than installed separately.

## EXAMPLE

Compile the following test program with `gcc -Wall -D_GNU_SOURCE` and
run the result.  (The `-D` flag is needed to make the declaration of
vasprintf(3) visible in `stdio.h`.)

    #include <stdio.h>
    #include "charvector.h"

    int main( ) {
        charvector cv = {};
        charvector_append( &cv, "Hello, world!" );
        charvector_append_printf( &cv,
            " You're %f years old.", 4.5e9 );
        charvector_append_c( &cv, '\n' );
        charvector_insert( &cv, " Happy birthday!", 13 );
        charvector_replace( &cv, "world", "Jupiter" );
        fputs( cv.str, stdout );
        charvector_free( &cv );
        return 0;
    }

## SEE ALSO

charvector_append(3),
charvector_append_c(3),
charvector_append_vprintf(3),
charvector_append_printf(3),
charvector_free(3),
charvector_insert(3),
charvector_length(3),
charvector_pad(3),
charvector_replace(3),
charvector_resize(3),
charvector_str(3),
charvector_size(3),
charvector_trim(3)

<END> */
typedef struct {
  char *str;
  size_t size;
} charvector;

char *charvector_insert( charvector *dst, const char *src, size_t offset );
char *charvector_append( charvector *dst, const char *src );
char *charvector_append_c( charvector *dst, char c );
char *charvector_append_vprintf( charvector *dst, const char *format,
				 va_list list );
char *charvector_append_printf( charvector *dst, const char *format, ... );
char *charvector_replace( charvector *dst, const char *find,
			  const char *replace );
char *charvector_pad( charvector *cv );
char *charvector_trim( charvector *cv );
char *charvector_resize( charvector *cv, size_t size, int truncate );

/* <BEGIN>
# charvector_str(3) 2016-07-13

## NAME

`charvector_str` - return a charvector's string element

## SYNOPSIS

`#include "charvector.h"`

`char *charvector_str( charvector *`_cv_ `);`

## DESCRIPTION

This macro returns the string _cv_`->str`, after first checking that
it is a valid nul-terminated string shorter than _cv_`->size`.  Note
that this is a macro, not a function: if the argument is an
expression, it may be evaluated multiple times.

## RETURN VALUE

The macro evaluates to _cv_`->str`, or NULL on an error.

## SEE ALSO

charvector_length(3),
charvector_size(3),
charvector(5)

<END> */
#define charvector_str( cv ) ( (cv)->str && !(cv)->str[(cv)->size-1] \
			       ? (cv)->str : NULL )

/* <BEGIN>
# charvector_length(3) 2016-07-13

## NAME

`charvector_length` - return the length of a charvector's string

## SYNOPSIS

`#include "charvector.h"`

`int *charvector_length( charvector *`_cv_ `);`

## DESCRIPTION

This macro returns the length of the string _cv_`->str`, after first
checking that it is a valid nul-terminated string shorter than
_cv_`->size`.  Note that this is a macro, not a function: if the
argument is an expression, it may be evaluated multiple times.

## RETURN VALUE

The macro evaluates to `strlen( `_cv_`->str )`, or 0 if _cv_`->str` is
NULL, or -1 if the last character of the array is non-nul.

## SEE ALSO

charvector_size(3),
charvector_str(3),
strlen(3),
charvector(5)

<END> */
#define charvector_length( cv ) \
( (cv)->str ? ( (cv)->str[(cv)->size-1] ? -1 : strlen( (cv)->str ) ) : 0 )

/* <BEGIN>
# charvector_size(3) 2016-07-13

## NAME

`charvector_size` - return a charvector's size element

## SYNOPSIS

`#include "charvector.h"`

`int charvector_size( charvector *`_cv_ `);`

## DESCRIPTION

This macro returns the value of _cv_`->size`.  Note that this is a
macro, not a function: if the argument is an expression, it may be
evaluated multiple times.

## RETURN VALUE

The macro evaluates to _cv_`->size`.

## SEE ALSO

charvector_length(3),
charvector_str(3),
charvector(5)

<END> */
#define charvector_size( cv ) ( (cv)->size )

/* <BEGIN>
# charvector_free(3) 2016-07-13

## NAME

`charvector_free` - free space allocated to a charvector.

## SYNOPSIS

`#include "charvector.h"`

`int charvector_free( charvector *`_cv_ `);`

## DESCRIPTION

This macro frees _cv_`->str` and sets it to NULL (if it is non-NULL to
begin with), and sets _cv_`->size` to zero.  Note that this is a
macro, not a function: if the argument is an expression, it may be
evaluated multiple times.

## RETURN VALUE

The macro evaluates to 0 always.

## SEE ALSO

charvector_size(3),
charvector_str(3),
charvector(5)

<END> */
#define charvector_free( cv ) \
( ( (cv)->str ? free( (cv)->str ), 0 : 0 ), \
  !memset( (cv), 0, sizeof(charvector) ) )


#ifdef  __cplusplus
#if 0
{
#endif
}
#endif
#endif
