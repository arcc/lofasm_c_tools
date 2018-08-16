#include<iostream>
#include<math.h>
#include<string>
#include<vector>
#include "lofasm_file.h"
#include "dedispersion_io.h"

int main(){
  int i, j;
  int ftype;
  DataFileContainer f_c;
  std::string dir_path = "./tests/testdata/bbx/AA/";
  std::vector<std::string> filenames;
  std::map<double,FileList>::iterator it;
  std::map<double,DataFile>::iterator it1, it2;
  // Add test files
  filenames.push_back(dir_path + "20180609_001737_AA.bbx");
  filenames.push_back(dir_path + "20180609_001234_AA.bbx");
  filenames.push_back(dir_path + "20180609_000731_AA.bbx");
  filenames.push_back(dir_path + "20180609_002240_AA.bbx");
  filenames.push_back(dir_path + "20180609_002744_AA.bbx");
  std::cout<< "Input files:\n";
  for (i = 0; i < filenames.size(); i++){
    std::cout << filenames[i] << "\n";
    f_c.add_file(filenames[i]);
  }

  std::cout<<f_c.obs << " " << f_c.frame <<"\n";

  for (it = f_c.file_lists.begin(); it!= f_c.file_lists.end(); it++){
    std::cout << it->first<<std::endl;
    std::cout << (it->second).max_freq<<std::endl;
    std::cout << (it->second).num_files<<std::endl;
    for (it1 = (it->second).files.begin(); it1 != (it->second).files.end(); it1++){
      std::cout << it1->first << " " << (it1->second).start_time << " ";
      std::cout << (it1->second).filenamecpp<<"\n";
    }
  }
  (f_c.file_lists.begin()->second).get_files_by_time(581731960.0, 581732570.0, it1, it2);
  while (it1 != std::next(it2,1)){
    std::cout << it1->second.filenamecpp <<"\n";
    it1 ++;
  }
  return 0;
}
