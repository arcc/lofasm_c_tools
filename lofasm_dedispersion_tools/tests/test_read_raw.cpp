#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>

#include "lofasm_data_class.h"
#include "lofasm_files.h"


int main(){
  LofasmFile lf;
  int ftype;
  string filename;
  LofasmIntgr itg;
  int i,j;
  filename ="../test_data/20151012_213201.lofasm";
  ftype = lf.check_file_type(filename);
  cout<< "File " << filename <<" is a type " << ftype << " file."<<endl;
  lf.rawFile.open_raw_file(filename);
  lf.rawFile.open_raw_file(filename);
  lf.rawFile.get_lofasm_raw_intgr(itg, 0);
  for (i=0;i<itg.numfBin;i++){
    cout<<itg.AA[i]<<" ";
  }
  cout<<endl;
  return 0;
}
