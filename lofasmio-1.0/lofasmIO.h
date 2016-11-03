// 2>&-### SELF-EXTRACTING DOCUMENTATION ###############################
// 2>&-#                                                               #
// 2>&-# Run "bash <thisfile> > <docfile.md>" to extract documentation #
// 2>&-#                                                               #
// 2>&-#################################################################
// 2>&-; awk '/^\/\* <BEGIN>/{f=1;next}/^<END> \*\//{f=0}f' $0; exit 0

/***********************************************************************
lofasmIO.h
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

#ifndef _LOFASMIO_H
#define _LOFASMIO_H
#ifdef  __cplusplus
extern "C" {
#if 0
};
#endif
#endif

#include <stdio.h>

#define LOFASMIO_VERSION 1 /* current version of lofasmIO */
#define LFB_DMAX 4 /* number of dimensions in a LoFASM file */

/* <BEGIN>
# lofasm_error(3)

## NAME

`lofasm_error, lofasm_warning, lofasm_info` - print status messages

## SYNOPSIS

`#include "lofasmIO.h"`

`extern int lofasm_verbosity;`

`void lofasm_error( const char *`_format_`, ... );`  
`void lofasm_warning( const char *`_format_`, ... );`  
`void lofasm_info( const char *`_format_`, ... );`

## DESCRIPTION

Programs compiled with `lofasmIO.h` may optionally use these functions
to standardize their error/warning/information reporting.  At present,
the functions simply wrap a call to fprintf(3), testing the value of
`lofasm_verbosity` before writing the message to `stderr`.  The values
of `lofasm_verbosity` are:

`0`:
    Quiet: No messages are written.

`1`:
    Default: Writes error messages (abnormal termination).

`2`:
    Verbose: As above, plus warnings (execution may proceed).

`3`:
    Very verbose: As above, plus info (normal status logging).

The functions may be implemented as macros: the arguments are used at
most once (in the call to fprintf(3)), but may not be used at all
depending on `lofasm_verbosity`, so arguments should not be
expressions with side effects.  Even for small values of
`lofasm_verbosity`, each invocation requires testing this value, so it
should not be used in an inner program loop: if you need a message
solely for debugging, it's better to wrap it in
`#ifdef DEBUG` ... `#endif`.

## RETURN VALUES

These functions do not return a value.

## SEE ALSO

fprintf(3)

<END> */
extern int lofasm_verbosity;
#define lf_error( ... ) \
do \
  if ( lofasm_verbosity > 0 ) { \
    const char *c = strrchr( __FILE__, '/' ); \
    c = ( c ? c + 1 : __FILE__ ); \
    fprintf( stderr, "\x1b[31;1m%s:\x1b[0m %s line %d: ", \
	     __func__, c, __LINE__ ); \
    fprintf( stderr, __VA_ARGS__ ); \
    putc( '\n', stderr ); \
  } \
while (0)
#define lf_warning( ... ) \
do \
  if ( lofasm_verbosity > 1 ) { \
    const char *c = strrchr( __FILE__, '/' ); \
    c = ( c ? c + 1 : __FILE__ ); \
    fprintf( stderr, "\x1b[33;1m%s:\x1b[0m %s line %d: ", \
	     __func__, c, __LINE__ ); \
    fprintf( stderr, __VA_ARGS__ ); \
    putc( '\n', stderr ); \
  } \
while (0)
#define lf_info( ... ) \
do \
  if ( lofasm_verbosity > 2 ) { \
    const char *c = strrchr( __FILE__, '/' ); \
    c = ( c ? c + 1 : __FILE__ ); \
    fprintf( stderr, "\x1b[32;1m%s:\x1b[0m %s line %d: ", \
	     __func__, c, __LINE__ ); \
    fprintf( stderr, __VA_ARGS__ ); \
    putc( '\n', stderr ); \
  } \
while (0)


/* Routines to read/write compressed data. */
#ifndef NO_ZLIB
#include <zlib.h>
#endif
FILE *lfopen( const char *filename, const char *mode );
FILE *lfdopen( int fd, const char *mode );


/* <BEGIN>
# lfb_hdr(5)

## Name

`lfb_hdr` - LoFASM filterbank header

## Synopsis

    typedef struct {
        char *hdr_type;
        float hdr_version;
        char *station;
        char *channel;
        char *start_time;
        double time_offset_J2000;
        double frequency_offset_DC;
        char *dim1_label;
        double dim1_start;
        double dim1_span;
        char *dim2_label;
        double dim2_start;
        double dim2_span;
        char *data_label;
        double data_offset;
        double data_scale;
        char *data_type;
        int dims[LFB_DMAX];
    } lfb_hdr;

## Description

This structure stores the required and some optional header metadata
from a LoFSAM filterbank file: see lofasm-filterbank(5) for
information on the fields.  It is populated by the readLFBX(3)
function and used by writeLFBX(3).

## See Also

readLFBX(3),
writeLFBX(3),
lofasm-filterbank(5)

<END> */
typedef struct {
  char *hdr_type;
  float hdr_version;
  char *station;
  char *channel;
  char *start_time;
  double time_offset_J2000;
  double frequency_offset_DC;
  char *dim1_label;
  double dim1_start;
  double dim1_span;
  char *dim2_label;
  double dim2_start;
  double dim2_span;
  char *data_label;
  double data_offset;
  double data_scale;
  char *data_type;
  int dims[LFB_DMAX];
} lfb_hdr;

int
bxRead( FILE *fp, int *headc, char ***headv, int *dimc, int **dimv,
	char **encoding, void **data, int checks );
int
bxReadData( const char *encoding, void *buf, size_t n, FILE *fp );
int
bxWrite( FILE *fp, char **headv, const int *dimv,
	 const char *encoding, const void *data, int checks );
int
bxWriteData( const char *encoding, const void *buf, size_t n,
	     size_t nrow, FILE *fp );
void
bxSkipHeader( FILE *fp );


/* I/O function prototypes. */
int
lfbxRead( FILE *fp, lfb_hdr *header, void **data );
int
lfbxWrite( FILE *fp, lfb_hdr *header, void *data );

#ifdef  __cplusplus
#if 0
{
#endif
}
#endif
#endif /* _LOFASMIO_H */
