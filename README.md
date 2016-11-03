lofasmio(7)  2016-08-04
===========

## Name

`lofasmio` - reading, writing, and preprocessing routines for LoFASM

## Synopsis

`make`  
`make man`  
`make check`

## Description

This suite provides basic functionality for reading, writing, viewing,
and processing LoFASM data.  Currenltly all LoFASM-parsing functions
are in the module `lofasmIO.c`, along with several simple data
analysis programs that may serve as examples.

After downloading/unpacking this directory, run `make` to compile the
module and programs, and `make man` to generate documentation.
Documentation is in the form of Unix manpages installed in the `doc`
directory; you may install them in your man(1) search path if desired.
The program documentation can also be viewed with _prog_ `-H`.

The `make check` target runs a simple test pipeline on sample files
located in the `testdata` subdirectory.

## See also

bxresample(1),
lfcat(1),
lfplot2d(1),
lfslice(1),
lftest(1),
lftype(1),
bxRead(3),
bxReadData(3),
bxSkipHeader(3),
bxWrite(3),
bxWriteData(3),
lfbxRead(3),
lfbxWrite(3),
lfdopen(3),
lfopen(3),
lofasm_error(3),
abx(5),
bbx(5),
lfb_hdr(5),
lofasm-filterbank(5)
