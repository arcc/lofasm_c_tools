/*Lofasm files header */
#ifndef LOFASM_BX_H_
#define LOFASM_BX_H_
#include "lofasmIO.h"
#include "lofasm_data_class.h"

void read_bx2flt(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr, \
                 int store_start_index);
double bx_get_freq_start(lfb_hdr& header);
double bx_get_freq_step(lfb_hdr& header);
double bx_get_time_start(lfb_hdr& header);
double bx_get_time_step(lfb_hdr& header);
#endif
