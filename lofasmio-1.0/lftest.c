const char *usage = "\
Usage: %s [ARGS]...\n\
Test lofasm routines.\n\
\n";

const char *description = "\
# lftest(1)\n\
\n\
## NAME\n\
`lftest` - test lofasm routines\n\
\n\
## SYNOPSIS\n\
`lftest` [_ARGS_]...\n\
\n\
## DESCRIPTION\n\
This program is a sandbox for testing LoFASM routines.  Its usage and\n\
effects may change without notice.\n\
\n";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lofasmIO.h"
#include "markdown_parser.h"

int
main( int argc, char **argv )
{
  lfb_hdr head = {};
  int i;
  double *d, *data = NULL;
  FILE *fp;

  for ( i = 1; i < argc; i++ )
    if ( !strcmp( argv[i], "-h" ) ||
	 !strcmp( argv[i], "--help" ) ) {
      printf( usage, argv[0] );
      return 0;
    } else if ( !strcmp( argv[i], "-H" ) ||
		!strcmp( argv[i], "-description" ) ) {
      markdown_to_manout( description );
      return 0;
    }

  fp = lfopen( argv[1], "rb" );
  lfbxRead( fp, &head, (void **)( &data ) );
  fclose( fp );

  for ( i = 0, d = data + head.dims[1]*head.dims[2]/4; i < head.dims[0];
	i++, d += head.dims[1]*head.dims[2] )
    printf( "%f\n", *d );
  free( data );
  return 0;
}
