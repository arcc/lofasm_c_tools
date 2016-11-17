/* This is a set of functions for reading the lofasm archive data formate. */
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>

#include "lofasm_data_class.h"
#include "lofasm_files.h"
#include "lofasmIO.h"

/*Some handy function for  read lofasm bx data. */
double bx_get_time_start(lfb_hdr& header){
  double start_time;
  start_time = header.time_offset_J2000 + header.dim1_start;
  return start_time;
}

double bx_get_time_step(lfb_hdr& header){
  double time_step;
  time_step = header.dim1_span / header.dims[0];
  return time_step;
}

double bx_get_freq_step(lfb_hdr& header){
  double freq_step;
  freq_step = header.dim2_span / header.dims[1];
  return freq_step;
}

double bx_get_freq_start(lfb_hdr& header){
  double start_freq;
  start_freq = header.frequency_offset_DC + header.dim2_start;
  return start_freq;
}

void read_bx2flt_real(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr){
  /* This function reads next N intgrations of filter bank data to the filter bank
  data class from the beginning time.
  Parameter
  ----------
  FILE *fp:  File pointer to the data file where the data is read from
  lfb_hdr *header:  Header information for the data file. It has to be the file
                   where fp is pointing at.
  FilterBank& flt:  Filter bank data class where the data will be read in
  int num_intgr:  Number of the integration need to read.
  */
  int i, j;
  double *databuff;
  double data_freq_start;
  double data_freq_end;
  double data_freq_step;
  double data_time_start;
  double data_time_end;
  double data_time_step;

  // Convert to MHz
  data_freq_step = bx_get_freq_step(header) / 1e6;
  data_freq_start = bx_get_freq_start(header) / 1e6;
  data_freq_end = data_freq_start + header.dim2_span;

  data_time_step = bx_get_time_step(header);
  data_time_start = bx_get_time_start(header);
  data_time_end = data_time_start + header.dim1_span;
  // Check the reading time bin is not over flow the filter bank data.
  if (num_intgr > flt.numTimeBin){
    cerr << "Too much time integration to put into the filter bank data!\n";
    exit(1);
  }

  if (num_intgr > header.dims[0]){
    cerr << "Not enough time integration in the data file!\n";
    exit(1);
  }

  // Check if the number of frequency bins of filter bank data matchs the file.
  if(header.dims[1] != flt.numFreqBin){
    cerr << "File number of frequency bin does not match frequency bin number of filter bank data!\n";
    exit(1);
  }

  flt.set_freqAxis(data_freq_start, data_freq_step);
  flt.set_timeAxis(data_time_start, data_time_step);
  databuff =(double *) malloc(header.dims[1]*sizeof(double));

  if (!databuff){
    cerr<< "Unable to allocate the data buff!"<<endl;
    exit(1);
  }
  for(i = 0; i < num_intgr; i++){
    fread(databuff, sizeof(double), header.dims[1], fp);
    for(j=0; j<header.dims[1]; j++){
      flt.fltdata[j][i] = databuff[j];
    }
  }
  return;
}

void read_bx2flt_cmplx(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr){
    return;
}

void read_bx2flt(FILE *fp, lfb_hdr& header, FilterBank& flt, int num_intgr, int cmplx){
    if (cmplx == 1){
        read_bx2flt_cmplx(fp, header, flt, num_intgr);
        return;
    }
    else {
        read_bx2flt_real(fp, header, flt, num_intgr);
        return;
    }
}
