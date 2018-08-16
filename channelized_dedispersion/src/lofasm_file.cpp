// This is a .cpp file defines the class function for lofasm_file classes.
// Author: Jing Luo

#include<stdio.h>
#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "lofasmIO.h"
#include "lofasm_file.h"

LfBbx::LfBbx(const char *fname, const char *input_mode) : DataFile(fname, input_mode)
{
  open_file();
}


int LfBbx::close_file(){
  fclose(f_ptr);
  return 0;
}

int LfBbx::open_file(){
  f_ptr =  lfopen(filename, mode);
  return 0;
}

int LfBbx::read_head()
// A function to read the lofasm file head
{
  int res;
  std::string f_str(filename);
  res = lfbxRead(f_ptr, &head, NULL);
  if (res > 1){
    std::cerr << "Reading file head for : " << f_str << " failed." << std::endl;
    exit(1);
  }
  return 0;
}

std::string LfBbx::get_obs()
{
  char str[80];
  strcpy(str, "LoFASM");
  strcat(str, head.station);
  return str;
}

std::string LfBbx::get_frame()
{
  return head.channel;
}

double LfBbx::get_dim1_start(){
  return head.time_offset_J2000 + head.dim1_start;
}

double LfBbx::get_dim1_step(){
  return head.dim1_span / (double)(head.dims[0]);
}
double LfBbx::get_dim2_start(){
  //NOTE: Here need to be updated for the frequency_offset_from_DC
  return head.dim2_start;
}
double LfBbx::get_dim2_step(){
  return head.dim2_span / (double)(head.dims[1]);
}

double LfBbx::get_min_freq()
{
  return get_dim2_start();
}

double LfBbx::get_max_freq()
{
  return get_dim2_start() + head.dim2_span;
}

double LfBbx::get_bandwidth()
{
  return head.dim2_span;
}

int LfBbx::read_data_real(int request_time_bin,
                       std::vector< std::vector<double> > & data){
  // Since the LoFASM file stores two-D array. The data read from the LoFASM
  // files is always in 2D.
  int i, j;
  double *databuff;
  if (curr_time_index + request_time_bin > head.dims[0]){
    std::cerr << " File : "<< std::string(filename) <<" does not have enough time bins." << std::endl;
    exit(1);
  }

  if (request_time_bin > data.size()){
    std::cerr << "Too much time bin to put into the data vector!\n";
    exit(1);
  }


  databuff =(double *) malloc(head.dims[1]*sizeof(double));

  if (!databuff){
    std::cerr<< "Unable to allocate the data buffer!"<<std::endl;
    exit(1);
  }
  // Read spectrum from each time
  for(i = 0; i < request_time_bin; i++){
    fread(databuff, sizeof(double), head.dims[1], f_ptr);
    for(j=0; j<head.dims[1]; j++){
      data[i][j] = databuff[j];
    }
  }
  curr_time_index += request_time_bin;
  return 0;
}

int LfBbx::read_data_cplx(int request_time_bin,
                         std::vector< std::vector<double> > & real_data,
                         std::vector< std::vector<double> > & cplx_data)
// TODO need to implement the complex reading.
{
  return 0;
}

int LfBbx::read_next_subint_2d (int num_next_time_bin, int cplx_flag,
                            std::vector < std:: vector <double> > & real_data,
                            std::vector < std:: vector <double> > & cplx_data)
// Since this is a higher level wrapper of reading data it handles both complex
// and real data cases. If it is a real data. the cplx_data can be passed as Null
{
  // Not a complex data.
  if (cplx_flag == 0){
    read_data_real(num_next_time_bin, real_data);
  }
  else{
    read_data_cplx(num_next_time_bin, real_data, cplx_data);
  }
  return 0;
};

//
