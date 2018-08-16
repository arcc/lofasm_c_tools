// This is a test for dedispersion IO routines.

#include<stdio.h>
#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "lofasm_file.h"
#include "dedispersion_io.h"

int main(){
  std::string fns = "/Users/jingluo/Work/research_codes/LoFASM/lofasm_c_tools/channelized_dedispersion/tests/20160619_000828_AA_sliced.bbx";
  int i;
  char mystring [100];
  std::vector <std::vector <double> > data;
  std::cout<< "start "<<std::endl;
  LfBbx test_class(fns.c_str(), "r");
  test_class.read_head();

  std::cout<<test_class.head.dim1_span << " " << test_class.head.dim2_span<<std::endl;
  data.resize(10);
  for (i = 0; i < 10; i++ ){
    data[i].resize(test_class.head.dims[1]);
  }
  
  test_class.read_data_real(10, data);
  std::cout << test_class.curr_time_index << std::endl<<std::endl;
  for (i = 0; i < test_class.head.dims[1]; i++){
    std::cout << data[9][i] << " ";
  }

  test_class.read_data_real(5, data);
  std::cout << test_class.curr_time_index << std::endl<<std::endl;
  for (i = 0; i < test_class.head.dims[1]; i++){
    std::cout << data[9][i] << " ";
  }
  

  return 0;
}