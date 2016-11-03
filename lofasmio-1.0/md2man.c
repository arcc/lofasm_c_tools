static const char *usage = "\
Usage: %s [OPTION]... [INFILE]... [> OUTFILE]\n\
Convert markdown document(s) to Unix manpage(s).\n\
\n\
  -h, --help         print this usage information\n\
  -H, --description  print a description of the program\n\
  -V, --version      print version and copyright information\n\
  -m, --multi        print multiple manpages to their own files\n\
  -b, --begin=BTAG   start parsing after line beginning with BTAG\n\
  -e, --end=ETAG     stop parsing before line beginning with ETAG\n\
  -q, --quit=QTAG    terminates all parsing at line beginning with QTAG\n\
  -s, --strip=L[+R]  strip L and R characters from start and end of line\n\
\n";

static const char *description = "\
# md2man(1) 2016-07-11\n\
\n\
## NAME\n\
\n\
`md2man` - convert markdown to Unix manpage\n\
\n\
## SYNOPSIS\n\
\n\
`md2man` [_OPTION_]... [_INFILE_]... [`>` _OUTFILE_]\n\
\n\
## DESCRIPTION\n\
\n\
This program reads markdown-formatted (.md) input, and writes output\n\
in the groff_man(7) format used for Unix manpages.  Zero or more input\n\
files are specified on the command line, with multiple input files\n\
being concatenated; if none are specified, the program reads from\n\
standard input.  An input filename of `-` also indicates standard\n\
input.  The formatted manpage is written to standard output (which\n\
will usually be piped to less(1) or redirected to _OUTFILE_).  The\n\
`--multi` option will instead write each top-level document section to\n\
its own file; see below.\n\
\n\
The program is essentially a front end to the markdown_to_manpage(3)\n\
function, an adaptation of John MacFarlane's PEG-markdown parser.  See\n\
that function's documentation for details.\n\
\n\
## OPTIONS\n\
\n\
`-h, --help`:\n\
    Prints basic usage information to `stdout` and exits.\n\
\n\
`-H, --description`:\n\
    Prints this description of the program.\n\
\n\
`-v, --version`:\n\
    Prints version and copyright information and exits.\n\
\n\
`-m, --multi`:\n\
    Writes each top-level section to its own manpage file rather than\n\
    standard output.  A section _command_`(`_#_`)` will be written to\n\
    the file _command_`.`_#_ in the current directory.\n\
\n\
`-b, --begin=`_BTAG_:\n\
    Starts parsing only after a line that begins with the string\n\
    _BTAG_.  The line containing the tag itself is not parsed (unless\n\
    parsing had already started due to another instance of _BTAG_).\n\
\n\
    In combination with the `--end` option, below, this allows you to\n\
    embed markdown documentation blocks within other files, such as a\n\
    code source files.\n\
\n\
`-e, --end=`_ETAG_:\n\
    Stops parsing at a line that begins with the string _ETAG_.  The\n\
    line containing the tag itself is not parsed.  The file will\n\
    continue to be read, and if a tag specified by the `--begin`\n\
    option (above) is encountered, parsing will resume.\n\
\n\
    If the same tag is specified for both the `--begin` and `--end`\n\
    options, then the tag toggles parsing on and off (starting with\n\
    off).\n\
\n\
`-q, --quit=`_QTAG_:\n\
    Stops reading input at a line that begins with the string _QTAG_,\n\
    and parses only the text read up to that point.  Unlike the\n\
    `--end` option, the program will read no further input even if\n\
    there are subsequent `--begin` tags.  If parsing was underway when\n\
    _QTAG_ was encountered, the line beginning with that tag is not\n\
    parsed.\n\
\n\
`-s, --strip=`_L_[`+`_R_]:\n\
    Removes _L_ characters from the start, and, optionally, _R_\n\
    characters from the end, of each line parsed.  The two (positive)\n\
    integers should be delimited by a `+` character, interpreted as\n\
    the sign of the second number.\n\
\n\
    This option is particularly useful when embedding documentation\n\
    within source files with the `--begin` and `--end` options, above.\n\
    For example, shell script comments may require a `#` character at\n\
    the start of each line; a block of markdown documentation can be\n\
    accomodated with `--strip=1` (or `2` if a single leading space is\n\
    also to be skipped).  For another example, if you comment C files\n\
    with blocks always of the form\n\
\n\
>>    `/* ` _comment_ ` */`\n\
\n\
:\n\
    then `--strip=3+3` will skip over both comment delimiters (plus\n\
    the single space of padding).\n\
\n\
## EXIT STATUS\n\
\n\
The program returns 0, or 1 if it could not parse a command-line\n\
option or open a file, or 2 on a memory error.\n\
\n\
## SEE ALSO\n\
\n\
markdown_to_manpage(3), markdown_to_manterm(3), charvector(5)\n\
\n";

static const char *version = "\
md2man version 0.1,\n\
Copyright (c) 2016 Teviet Creighton.\n\
\n\
Based on peg-markdown version 0.4.14,\n\
Copyright (c) 2008-2009 John MacFarlane.\n\
\n\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License or the MIT\n\
license.  See LICENSE for details.\n\
\n\
This program is distributed in the hope that it will be useful, but\n\
WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n\
General Public License for more details.\n\
\n";

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "markdown_parser.h"
#include "charvector.h"

static const char short_opts[] = "hHVmb:e:q:s:";
static const struct option long_opts[] = {
  { "help", 0, 0, 'h' },
  { "description", 0, 0, 'H' },
  { "version", 0, 0, 'V' },
  { "multi", 0, 0, 'm' },
  { "begin", 1, 0, 'b' },
  { "end", 1, 0, 'e' },
  { "quit", 1, 0, 'q' },
  { "strip", 1, 0, 's' },
  { 0, 0, 0, 0} };

#define LEN 1024 /* character buffer size */

int
main( int argc, char **argv )
{
  char opt;             /* option character */
  int multi = 0;        /* whether to write multiple files */
  char *begin = NULL;   /* tag to start parsing */
  char *end = NULL;     /* tag to stop parsing */
  char *quit = NULL;    /* tag to terminate all further parsing */
  int l = 0, r = 0;     /* characters to strip from left and right */
  int parse = 1;        /* whether to parse current section */
  char buf[LEN];        /* file input buffer */
  FILE *fp;             /* input/output file pointer */
  charvector text = {}; /* input file as variable-length string */
  int flag = 0;         /* return code or "no arguments" flag */
  char *a, *b;          /* character return codes */

  /* Parse options. */
  while ( ( opt = getopt_long( argc, argv, short_opts, long_opts, NULL ) )
          != -1 ) {
    switch ( opt ) {
    case 'h':
      fprintf( stdout, usage, argv[0] );
      return 0;
    case 'H':
      markdown_to_manout( description );
      return 0;
    case 'V':
      fprintf( stdout, version );
      return 0;
    case 'm':
      multi = 1;
      break;
    case 'b':
      begin = optarg;
      parse = 0;
      break;
    case 'e':
      end = optarg;
      break;
    case 'q':
      quit = optarg;
      break;
    case 's':
      if ( sscanf( optarg, "%d%d", &l, &r ) < 1 ) {
	fprintf( stderr, "%s: could not read argument to -s,--strip"
		 " option\n", argv[0] );
	return 1;
      } else if ( l + r >= LEN ) {
	fprintf( stderr, "%s: total characters to strip must be less"
		 " than input buffer %d\n", argv[0], LEN );
	return 1;
      } else if ( l < 0 || r < 0 ) {
	fprintf( stderr, "%s: argument(s) to -s,--strip must be"
		 " positive\n", argv[0] );
	return 1;
      }
      break;
    case '?': case ':':
      fprintf( stderr, "Try %s --help for more information\n", argv[0] );
      return 1;
    default:
      fprintf( stderr, "%s: internal error parsing option code %c\n"
	       "Try %s --help for more information\n", argv[0],
	       opt, argv[0] );
      return 1;
    }
  }

  /* Read files from command line, or stdin if there are no
     command-line arguments (set by flag). */
  for ( flag = ( optind >= argc ); flag || optind < argc;
	flag = 0, optind++ ) {
    if ( flag || !strcmp( argv[optind], "-" ) )
      fp = stdin;
    else if ( !( fp = fopen( argv[optind], "r" ) ) ) {
      fprintf( stderr, "%s: could not open %s\n", argv[0], argv[optind] );
      charvector_free( &text );
      return 1;
    }
    /* Loop over lines of input. */
    do {
      /* Check if line starts with begin or end tag. */
      a = fgets( buf, LEN, fp );
      if ( quit && !strncmp( buf, quit, strlen( quit ) ) )
	break;
      if ( ( !parse && begin && !strncmp( buf, begin, strlen( begin ) ) ) ||
	   ( parse && end && !strncmp( buf, end, strlen( end ) ) ) ) {
	parse = !parse;
	while ( a && buf[ strlen( buf ) - 1 ] != '\n' )
	  a = fgets( buf, LEN, fp );
      } else if ( !parse ) {
	while ( a && buf[ strlen( buf ) - 1 ] != '\n' )
	  a = fgets( buf, LEN, fp );
      }
      /* Copy line into text, stripping first l and last r characters. */
      else {
	int start = 1; /* whether this is start of line */
	int i;         /* index over characters to trim */
	do {
	  if ( start && strlen( buf ) <= r + l )
	    b = charvector_append_c( &text, '\n' );
	  else {
	    b = charvector_append( &text, buf + ( start ? l : 0 ) );
	    if ( b && r && buf[ strlen( buf ) - 1 ] == '\n' ) {
	      for ( i = 0, b = text.str + strlen( text.str ) - 1; i < r ;
		    i++, b-- )
		b[0] = '\0';
	      b[0] = '\n';
	    }
	  }
	  if ( !b ) {
	    fprintf( stderr, "%s: memory error\n", argv[0] );
	    charvector_free( &text );
	    return 2;
	  }
	  start = 0;
	} while ( buf[ strlen( buf ) - 1 ] != '\n' &&
		  ( a = fgets( buf, LEN, fp ) ) );
      }
    } while ( a );
    if ( fp != stdin )
      fclose( fp );
    if ( a )
      break;
  }

  /* Parse and write results. */
  fp = ( multi ? NULL : stdout );
  flag = markdown_to_manpage( text.str, fp );
  charvector_free( &text );
  if ( flag == 1 )
    fprintf( stderr, "%s: write error\n", argv[0] );
  else if ( flag /* == 2 */ )
    fprintf( stderr, "%s: write error\n", argv[0] );
  return flag;
}
