#include<stdlib.h>
#include "lofasm_dedispersion.h"

int main(int argc, char* argv[]){
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " config_file_name" <<endl;
        return 1;
    }
    dedsps_config config;

    config.read_config_file(argv[1]);
    cout<<config.time_start<<endl;
    return 0;
}
