static const char *version = "\
lfscale version 1.0\n\
Copyright (c) 2016 Teviet Creighton.\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or (at\n\
your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful, but\n\
WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n\
General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\
\n";

static const char *usage = "\
Usage: %s [OPTION]... [INFILE [OUTFILE]]\n\
Rescale dimensions and data of a LoFASM file.\n\
\n\
  -h, --help             print this usage information\n\
  -H, --description      print a description of the program\n\
  -V, --version          print program version\n\
  -v, --verbosity=LEVEL  set status message reporting level\n\
  -y, --rows=SPEC        specify number of rows\n\
  -x, --cols=SPEC        specify number of columns\n\
  -z, --component=COMP   convert complex to real\n\
  -s, --lin=SCALE[+OFF]  convert on a linear scale\n\
  -l, --log=BASE         convert to log scale\n\
  -c, --colourmap=MAP    specify colour scale for output\n\
  -p, --postscript       generate PostScript output\n\
\n";

static const char *description = "\
# lfscale(1)\n\
\n\
## NAME\n\
\n\
`lfscale` - plot lofasm-filterbank(5) data in two dimensions\n\
\n\
## SYNOPSIS\n\
\n\
`lfscale` [_OPTION_]... [_INFILE_ [_OUTFILE_]]\n\
\n\
## DESCRIPTION\n\
\n\
This program reads data from _INFILE_ in lofasm-filterbank(5) format,\n\
rescales the dimensions or data according to the specified _OPTION_s,\n\
and writes the passed band to _OUTFILE_ in the same format.  If\n\
_INFILE_ or _OUTFILE_ is not specified, or is a single `-` character,\n\
then standard input or standard output is used instead.  Without\n\
options, the program will trivially copy input to output.\n\
\n\
The alternative program bxresample(1) can adjust the dimensions of any\n\
abx(5) or bbx(5) file (not just those in lofasm-filterbank(5) format),\n\
but does not perform numerical rescaling of the data.\n\
\n\
## OPTIONS\n\
\n\
`-h, --help`:\n\
    Prints basic usage information to stdout and exits.\n\
\n\
`-H, --description`:\n\
    Prints this description of the program.\n\
\n\
`-V, --version`:\n\
    Prints version and copyright information.\n\
\n\
`-v, --verbosity=`_LEVEL_:\n\
    Sets the verbosity level for error reporting.  _LEVEL_ may be `0`\n\
    (quiet, no messages), `1` (default, error messages only), `2`\n\
    (verbose, errors and warnings), or `3` (very verbose, errors,\n\
    warnings, and extra information).\n\
\n\
`-y, --rows=`_SPEC_:\n\
    Specifies the length of the first dimension in the rescaled data.\n\
    _SPEC_ may be a number, a multiple preceded by `x`, or a divisor\n\
    preceded by '/'.  For example, `--rows=x10` will increase the\n\
    first dimension by a factor 10, and `--rows=/10` will reduce it by\n\
    a similar factor.  Numbers are read as (positive) floating point\n\
    values, and the final dimension is rounded to the nearest integer.\n\
\n\
    At present, rows are resampled using \"nearest point\" sampling,\n\
    without any averaging or interpolation.  Other options may be\n\
    added in future.\n\
\n\
`-x, --cols=`_SPEC_:\n\
    Specifies the length of the second dimension in the rescaled data;\n\
    otherwise the same as `-y, --rows` above.\n\
\n\
`-z, --component=`_COMP_:\n\
    Specifies how complex data is converted to real numbers before\n\
    mapping to the colour scale.  This option is ignored for real\n\
    data.  The recognized values of _COMP_ are `re`, `im`, `abs`, or\n\
    `arg`, giving the real part, imagimary part, complex magnitude, or\n\
    complex argument (arctangent) of the input.  If not specified,\n\
    `abs` is assumed.  The argument specified by `arg` will range from\n\
    -pi/2 to +pi/2, though it can be remapped by the `-s, --lin`\n\
    option, below.\n\
\n\
`-s, --lin=`_SCALE_[+_OFF_]:\n\
    Performs a linear remapping of the data.  The argument consists of\n\
    a number _SCALE_, with an optional second number _OFF_ directly\n\
    appended with its sign character as a delimiter.  The data are\n\
    first multiplied by _SCALE_, and then optionally added to _OFF_.\n\
\n\
`-l, --log=`_BASE_:\n\
    Converts the output from linear to log scale.  If _BASE_ is a\n\
    number, it is taken as the base of the logarithm; e.g. `2` or\n\
    `10`.  A _BASE_ of `e` specifies the natural logarithm: equivalent\n\
    to base `2.718281828459045`.  A _BASE_ of `dB` specifies output in\n\
    decibels: equivalent to base `1.258925411794167`.  Any rescaling\n\
    specified by the `-s, --lin` option is done before taking the\n\
    logarithm.\n\
\n\
`-c, --colourmap=`_MAP_:\n\
    Specifies a colourmap for representing data levels.  _MAP_ may be\n\
    either a named colourmap; a colourmap array (see below); or the\n\
    name of a file containing a colourmap array, preceded by a `^`\n\
    character.  If _MAP_ is `^-` then a colourmap is read from\n\
    standard input (whereas `^./-` will read from an actual file named\n\
    `-` in the current directory).  If _MAP_ is a `?` character then\n\
    the program will list all named colourmaps and then exit.  The\n\
    default colourmap is `gray`.\n\
\n\
    A colourmap array is a sequence of 4N numbers.  In each group of 4\n\
    numbers, the first corresponds to an input level, and the next 3\n\
    are the red, green, and blue colour levels (in the range 0 to 1)\n\
    corresponding to that input level.  Inputs between specified\n\
    levels will have their RGB colour linearly interpolated.\n\
    Optionally, the fist input level may be given as \"nan\",\n\
    specifying a colour to be used for NaN floating-point data;\n\
    otherwise the first entry will be used for NaNs.  Subsequent input\n\
    levels must be given in non-descending order (consecutive entries\n\
    may have the same input level in order to produce a sharp break in\n\
    the colourmap).  Levels out of range are mapped to the nearest\n\
    end.\n\
\n\
    If the colourmap array is specified directly as the option\n\
    argument, then any whitespace must be escaped to prevent the shell\n\
    from breaking up the argument.  Normally one can simply\n\
    concatenate the numbers into a single token, using sign characters\n\
    `+` or `-` to delimit numbers.  The named colourmaps listed with\n\
    the `?` option argument use this convention.  If the array is read\n\
    from a file, then any whitespace between numbers is skipped.\n\
\n\
`-p, --postscript`:\n\
    Specifies that the output will be an encapsulated PostScript (EPS)\n\
    file.  Note that the BX image format is almost completely\n\
    compatible with PostScript; one merely needs to define the\n\
    _encoding_ tag (in this case `raw256`) to be a PostScript function\n\
    that passes the preceding dimensions and subsequent data to the\n\
    PostScript `colorimage` operator.  The output can then be piped\n\
    directly to gv(1) for viewing or gs(1) for conversion to another\n\
    format.\n\
\n\
## EXIT STATUS\n\
\n\
The proram exits with status 0 normally, 1 if there is an error\n\
parsing its arguments, 2 on read/write errors, or 3 if the file has\n\
incorrect syntax.\n\
\n\
## SEE ALSO\n\
\n\
lfbxRead(3),\n\
lfbxWrite(3),\n\
lofasm-filterbank(5)\n\
\n";

#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include "markdown_parser.h"
#include "lofasmIO.h"

static const char short_opts[] = ":hHVv:y:x:z:l:s:c:p";
static const struct option long_opts[] = {
  { "help", 0, 0, 'h' },
  { "description", 0, 0, 'H' },
  { "version", 0, 0, 'V' },
  { "verbosity", 1, 0, 'v' },
  { "rows", 1, 0, 'y' },
  { "cols", 1, 0, 'x' },
  { "components", 1, 0, 'z' },
  { "log", 1, 0, 'l' },
  { "lin", 1, 0, 's' },
  { "colourmap", 1, 0, 'c' },
  { "postscript", 0, 0, 'p' },
  { 0, 0, 0, 0} };

static char *colour_maps[] = {
  "gray", "nan+1+0+0+0+0+0+0+1+1+1+1",
  "cold", "nan+1+0+0+0+0+0+0+.0722+0+0+1+.7874+0+1+1+1+1+1+1",
  "hot", "nan+1+0+0+0+0+0+0+.2626+1+0+0+.9278+1+1+0+1+1+1+1",
  "jet", "0+0+0+.25+.125+0+0+1+.375+0+1+1+.625+1+1+0+.875+1+0+0+1+.25+0+0",
  "rainbow", "0+1+0+0+.25+1+1+0+.5+0+1+0+.625+0+1+1+.75+0+0+1+1+.75+0+1",
  0 };

#define LEN 1024 /* character buffer size */

int
main( int argc, char **argv )
{
  char opt;                           /* option character */
  char *infile, *outfile;             /* input/output filenames */
  FILE *fpin, *fpout;                 /* input/output file objects */
  double *rowin = NULL;               /* single timestep of data read */
  double *rowdat = NULL;              /* single timestep of data parsed */
  unsigned char *rowout = NULL;       /* single timestep of output */
  char *xarg = NULL, *yarg = NULL;    /* -x and -y options */
  char *zarg = "abs", *carg = "gray"; /* -z and -c options */
  double scale = 1.0, off = 0.0;      /* -s option */
  double base = 0.0;                  /* -l option */
  int ps = 0;                         /* -p flag */
  double *cmap, *cnan;                /* colourmap */
  int clength, csize;                 /* current/allocated length of cmap */
  int done = 0;                       /* whether finished reading cmap */
  int cmapstd = 0;                    /* whether cmap was read from stdin */
  lfb_hdr head = {};                  /* lofasm header */
  int dims[LFB_DMAX] = {};            /* saved input dimensions */
  int i, iin, jin, iout, j;           /* index in infile and outfile */
  int k, m;                           /* indecies in colourmap */
  double f;                           /* fractional position within cmap */
  double d, dmin, dmax;               /* minimum and maximum scaled values */
  int n, nin, ndat, nout;             /* data per row in infile and outfile */
  char *a, *b;                        /* pointers within a string */
  char **c;                           /* pointer to colourmaps */

  /* Parse options. */
  opterr = 0;
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
    case 'v':
      lofasm_verbosity = atoi( optarg );
      break;
    case 'y':
      yarg = optarg;
      break;
    case 'x':
      xarg = optarg;
      break;
    case 'z':
      zarg = optarg;
      if ( strcmp( zarg, "re" ) && strcmp( zarg, "im" ) &&
	   strcmp( zarg, "abs" ) && strcmp( zarg, "arg" ) ) {
	lf_error( "bad argument %s to -z, --components", optarg );
	return 1;
      }
      break;
    case 's':
      if ( sscanf( optarg, "%lf%lf", &scale, &off ) < 1 ) {
	lf_error( "bad argument %s to -s, --lin", optarg );
	return 1;
      }
      break;
    case 'l':
      if ( !strcmp( optarg, "dB" ) )
	base = pow( 10.0, 0.1 );
      else if ( !strcmp( optarg, "e" ) )
	base = M_E;
      else {
	base = strtod( optarg, &b );
	if ( b[0] || base <= 0.0 ) {
	  lf_error( "bad argument %s to -l, --log", optarg );
	  return 1;
	}
      }
      base = 1.0/log( base );
      break;
    case 'c':
      if ( !strcmp( optarg, "?" ) ) {
	fprintf( stdout, "Colour maps:\n" );
	for ( i = 0, c = colour_maps; *c; c += 2 )
	  if ( ( j = strlen( *c ) ) > i )
	    i = j;
	for ( c = colour_maps; *c; c += 2 )
	  fprintf( stdout, "%s:%*s%s\n", c[0], i + 1 - strlen( c[0] ),
		   "", c[1] );
	return 0;
      }
      carg = optarg;
      break;
    case 'p':
      ps = 1;
      break;
    case '?':
      if ( optopt )
	lf_error( "unknown option -%c\n\t"
		  "Try %s --help for more information",
		  optopt, argv[0] );
      else
	lf_error( "unknown option %s\n\t"
		  "Try %s --help for more information",
		  argv[optind-1], argv[0] );
      return 1;
    case ':':
      if ( optopt )
	lf_error( "option -%c requires an argument\n\t"
		  "Try %s --help for more information",
		  optopt, argv[0] );
      else
	lf_error( "option %s requires an argument\n\t"
		  "Try %s --help for more information",
		  argv[optind-1], argv[0] );
      return 1;
    default:
      lf_error( "internal error parsing option code %c\n"
		"Try %s --help for more information",
		opt, argv[0] );
      return 1;
    }
  }

  /* Parse other arguments. */
  if ( optind >= argc || !strcmp( ( infile = argv[optind++] ), "-" ) )
    infile = NULL;
  if ( optind >= argc || !strcmp( ( outfile = argv[optind++] ), "-" ) )
    outfile = NULL;
  if ( optind < argc ) {
    lf_error( "too many arguments" );
    return 1;
  }

  /* Read colourmap. */
  if ( !( cnan = cmap = (double *)malloc( LEN*sizeof(double) ) ) ) {
    lf_error( "memory error" );
    return 4;
  }
  csize = LEN;
  if ( carg[0] == '^' ) {
    if ( strcmp( carg + 1, "-" ) ) {
      fpin = stdin;
      cmapstd = 1;
    } else if ( !( fpin = fopen( carg + 1, "r" ) ) ) {
      lf_error( "could not read colourmap from %s", optarg + 1 );
      free( cnan );
      return 2;
    }
    carg = NULL;
  } else {
    for ( c = colour_maps; *c; c += 2 )
      if ( !strcmp( *c, carg ) ) {
	carg = c[1];
	break;
      }
    fpin = NULL;
  }
  for ( clength = 0, b = carg, a = NULL; !done; clength++ ) {
    if ( fpin )
      done = ( fscanf( fpin, "%lf", cmap + clength ) < 1 );
    else
      done = ( cmap[clength] = strtod( a = b, &b ), a == b );
    if ( clength == csize - 1 ) {
      double *temp = (double *)realloc( cmap, 2*csize*sizeof(double) );
      if ( !temp ) {
	lf_error( "memory error" );
	free( cnan );
	return 4;
      }
      cmap = temp;
      csize *= 2;
    }
  }
  if ( fpin && !cmapstd )
    fclose( fpin );

  /* Check and rescale colourmap. */
  if ( isnan( cmap[0] ) ) {
    cmap += 4;
    clength -= 4;
  }
  if ( ( clength /= 4 ) < 1 ) {
    lf_error( "not enough points in colourmap" );
    free( cnan );
    return 1;
  }
  for ( m = 1; m < clength; m++ )
    if ( cmap[4*m] < cmap[4*m-4] ) {
      lf_error( "non-monotonic colourmap" );
      free( cnan );
      return 1;
    }
  for ( m = 0; m < clength; m++ )
    for ( k = 0; k < 3; k++ )
      cmap[4*m+k+1] = ( cmap[4*m+k+1] < 0.0 ? 0.0 :
			( cmap[4*m+k+1] > 1.0 ? 255.0 :
			  cmap[4*m+k+1]*255.0 ) );

  /* Read input header. */
  if ( !infile ) {
    if ( cmapstd ) {
      lf_error( "cannot read colourmap and data from stdin" );
      free( cnan );
      return 2;
    }
    if ( !( fpin = lfdopen( 0, "rb" ) ) ) {
      lf_error( "could not read stdin" );
      free( cnan );
      return 2;
    }
    infile = "stdin";
  } else if ( !( fpin = lfopen( infile, "rb" ) ) ) {
    lf_error( "could not open input file %s", infile );
    fclose( fpin );
    free( cnan );
    return 2;
  }
  if ( lfbxRead( fpin, &head, NULL ) ) {
    lf_error( "could not parse header from %s", infile );
    fclose( fpin );
    free( cnan );
    return 2;
  }

  /* Compute output dimensions. */
  memcpy( dims, head.dims, LFB_DMAX*sizeof(int) );
  head.dims[2] = 3;
  head.dims[3] = 8;
  if ( yarg ) {
    double y = strtod( yarg + ( strchr( "x/", yarg[0] ) ? 1 : 0 ), &b );
    if ( b[0] || y <= 0.0 ) {
      lf_error( "bad argument %s to -y, --rows", yarg );
      fclose( fpin );
      free( cnan );
      return 1;
    }
    if ( yarg[0] == 'x' )
      head.dims[0] = (int)( dims[0]*y + 0.5 );
    else if ( yarg[0] == '/' )
      head.dims[0] = (int)( dims[0]/y + 0.5 );
    else
      head.dims[0] = (int)( y + 0.5 );
  }
  if ( xarg ) {
    double x = strtod( xarg + ( strchr( "x/", xarg[0] ) ? 1 : 0 ), &b );
    if ( b[0] || x <= 0.0 ) {
      lf_error( "bad argument %s to -x, --cols", xarg );
      fclose( fpin );
      free( cnan );
      return 1;
    }
    if ( xarg[0] == 'x' )
      head.dims[1] = (int)( dims[1]*x + 0.5 );
    else if ( xarg[0] == '/' )
      head.dims[1] = (int)( dims[1]/x + 0.5 );
    else
      head.dims[1] = (int)( x + 0.5 );
  }
  if ( head.dims[0] < 1 || head.dims[1] < 1 ) {
    lf_error( "bad output dimensions" );
    fclose( fpin );
    free( cnan );
    return 1;
  }

  /* Check output type. */
  if ( dims[3] != 64 ) {
    lf_error( "require real64 data" );
    fclose( fpin );
    free( cnan );
    return 1;
  }
  if ( head.data_type ) {
    if ( !( a = strstr( head.data_type, "real64" ) ) )
      lf_warning( "reading as real64 not %s",
		  head.data_type + strlen( "%data_type: " ) );
    else
      strcpy( a, "uchar8" );
  }

  /* Allocate data. */
  nin = dims[1]*dims[2];
  ndat = head.dims[1]*2;
  nout = head.dims[1]*3;
  if ( !( rowin = (double *)malloc( nin*sizeof(double) ) ) ||
       !( rowdat = (double *)malloc( ndat*sizeof(double) ) ) ||
       !( rowout = (unsigned char *)malloc( nout*sizeof(unsigned char) ) ) ) {
    lf_error( "memory error" );
    fclose( fpin );
    if ( rowin )
      free( rowin );
    if ( rowdat )
      free( rowdat );
    free( cnan );
    return 4;
  }

  /* Open output file. */
  if ( !outfile ) {
    if ( !( fpout = lfdopen( 1, "wb" ) ) ) {
      lf_error( "could not write to stdout" );
      fclose( fpin );
      free( rowin );
      free( rowdat );
      free( rowout );
      free( cnan );
      return 2;
    }
    outfile = "stdout";
  } else if ( !( fpout = lfopen( outfile, "wb" ) ) ) {
    lf_error( "could not open output file %s", outfile );
    fclose( fpin );
    free( rowin );
    free( rowdat );
    free( rowout );
    free( cnan );
    return 2;
  }

  /* Write PostScript header. */
  if ( ps ) {
    fprintf( fpout, "%%!PS-Adobe-3.0 EPSF-3.0\n" );
    if ( outfile )
      fprintf( fpout, "%%%%Title: %s\n", outfile );
    fprintf( fpout, "%%%%Creator: writeEPSfile()\n" );
    fprintf( fpout, "%%%%Orientation: Portrait\n"
	     "%%%%Pages: 0\n"
	     "%%%%LanguageLevel: 2\n"
	     "%%%%BoundingBox: 0 0 %i %i\n"
	     "%%%%EndComments\n\n"
	     "%%%%BeginProlog\n"
	     "/raw256 {\n"
	     "  4 -1 roll 3 1 roll exch 3 index 3 index scale\n"
	     "  [ 4 index 0 0 6 index dup neg exch 0 exch ] exch\n"
	     "  currentfile exch false exch colorimage\n"
	     "} def\n"
	     "%%%%EndProlog\n", head.dims[1], head.dims[0] );
  }

  /* Write BX output header. */
  if ( lfbxWrite( fpout, &head, NULL ) ) {
    lf_error( "error writing header to %s", outfile );
    fclose( fpout );
    fclose( fpin );
    free( rowin );
    free( rowdat );
    free( rowout );
    free( cnan );
    return 2;
  }

  /* Set up extrema. */
  dmin = strtod( "+inf", 0 );
  dmax = strtod( "-inf", 0 );

  /* Step through files. */
  i = -1;
  for ( iout = 0; iout < head.dims[0]; iout++ ) {

    /* Read row. */
    iin = ( iout*dims[0] )/head.dims[0];
    if ( !feof( fpin ) ) {
      for ( ; i < iin; i++ )
	if ( ( n = fread( rowin, sizeof(double), nin, fpin ) ) < nin ) {
	  lf_warning( "read %d numbers from %s, expected %d",
		      ( i + 1 )*nin + n, infile, dims[0]*nin );
	  memset( rowin + n, 0, ( nin - n )*sizeof(double) );
	}
    } else
      memset( rowin, 0, nin*sizeof(double) );
    for ( j = 0; j < head.dims[1]; j++ ) {
      jin = ( j*dims[1] )/head.dims[1];
      memcpy( rowdat + j*2, rowin + jin*dims[2], dims[2]*sizeof(double) );
    }

    /* Apply transformations. */
    if ( dims[2] == 2 ) {
      if ( !strcmp( zarg, "im" ) )
	for ( j = 0; j < head.dims[1]; j++ )
	  rowdat[2*j] = rowdat[2*j+1];
      else if ( !strcmp( zarg, "abs" ) )
	for ( j = 0; j < head.dims[1]; j++ )
	  rowdat[2*j] = hypot( rowdat[2*j], rowdat[2*j+1] );
      else if ( !strcmp( zarg, "arg" ) )
	for ( j = 0; j < head.dims[1]; j++ )
	  rowdat[2*j] = atan2( rowdat[2*j+1], rowdat[2*j] );
    }
    if ( scale != 1.0 || off != 0.0 )
      for ( j = 0; j < head.dims[1]; j++ )
	rowdat[2*j] = scale*rowdat[2*j] + off;
    if ( base != 0.0 )
      for ( j = 0; j < head.dims[1]; j++ )
	rowdat[2*j] = log( rowdat[2*j] )*base;

    /* Compute colour. */
    for ( j = 0; j < head.dims[1]; j++ ) {
      if ( ( d = rowdat[2*j] ) < dmin )
	dmin = d;
      if ( d > dmax )
	dmax = d;
      if ( isnan( d ) )
	for ( k = 0; k < 3; k++ )
	  rowout[3*j+k] = (unsigned char)( cnan[k+1] );
      else if ( d < cmap[0] )
	for ( k = 0; k < 3; k++ )
	  rowout[3*j+k] = (unsigned char)( cmap[k+1] );
      else {
	for ( m = 1; m < clength; m++ )
	  if ( d < cmap[4*m] ) {
	    f = ( d - cmap[4*m-4] )/( cmap[4*m] - cmap[4*m-4] );
	    for ( k = 0; k < 3; k++ )
	      rowout[3*j+k] = (unsigned char)
		( ( 1.0 - f )*cmap[4*m+k-3] + f*cmap[4*m+k+1] );
	    break;
	  }
	if ( m == clength )
	  for ( k = 0; k < 3; k++ )
	    rowout[3*j+k] = (unsigned char)( cmap[4*clength+k-3] );
      }
    }

    /* Write line out. */
    if ( fwrite( rowout, sizeof(unsigned char), nout, fpout ) < nout ) {
      lf_error( "error writing to %s", outfile );
      fclose( fpout );
      fclose( fpin );
      free( rowin );
      free( rowdat );
      free( rowout );
      free( cnan );
      return 2;
    }
  }
  lf_info( "scaled data: [%g,%g]", dmin, dmax );
  fclose( fpout );
  fclose( fpin );
  free( rowin );
  free( rowdat );
  free( rowout );
  free( cnan );
  return 0;
}
