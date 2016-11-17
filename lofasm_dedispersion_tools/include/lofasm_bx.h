/*Lofasm files header */
#ifndef LOFASM_BX_H_
#define LOFASM_BX_H_
#include "lofasmIO.h"
#include "lofasm_data_class.h"

double bx_get_freq_start(lfb_hdr& header);
double bx_get_freq_step(lfb_hdr& header);
double bx_get_time_start(lfb_hdr& header);
double bx_get_time_step(lfb_hdr& header);
void read_bx2flt_real(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr);
void read_bx2flt_cmplx(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr);
void read_bx2flt(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr, int cmplx);
#endif
