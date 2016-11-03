/*Lofasm files header */
#ifndef LOFASM_BX_H_
#define LOFASM_BX_H_
#include "lofasmIO.h"
#include "lofasm_data_class.h"

void read_bx2flt(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr, \
                 int store_start_index);

#endif
