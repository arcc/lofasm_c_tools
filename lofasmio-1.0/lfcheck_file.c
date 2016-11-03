static const char *version = "\
lfstats version 1.0\n\
Copyright (c) 2016 Jing Luo.\n\
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
Usage: %s [OPTION]... [INFILE] [> OUTFILE]\n\
Compute various statistics from a LoFASM file.\n\
\n\
  -h, --help              print this usage information\n\
  -H, --description       print a description of the program\n\
  -V, --version           print program version\n\
  -v, --verbosity=LEVEL   set status message reporting level\n\
\n";


#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include "markdown_parser.h"
#include "lofasmIO.h"

int
main( int argc, char **argv )
{
  
