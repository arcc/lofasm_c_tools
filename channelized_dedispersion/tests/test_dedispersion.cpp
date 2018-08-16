// This is a file for testing the dedispersion

#include<iostream>
#include <fstream>
#include<math.h>
#include<string>
#include<vector>
#include "dedispersion.h"
#include "dedispersion_utils.h"
#include "dedispersion_class.h"

int main(){
  ChanData cdata;
  int i, j;
  int sig_start;
  int sig_end;
  double dm_start;
  double dm_step;
  int dm_num;
  std::ofstream in_data;
  std::ofstream out_result;
  // populate cdata
  cdata.freq = 5e7; // 50 MHz
  cdata.chan_bandwidth = 0.09e7; // 0.09 MHz bandwidth
  cdata.start_time = 0;
  cdata.time_step = 0.08; // 0.08 second of time resolution
  cdata.num_time_bin = 3000;
  cdata.data.resize(cdata.num_time_bin, 1.0);
  sig_start = 100;
  sig_end = 620;

  for (i = sig_start; i <= sig_end; i++){
    cdata.data[i] = 3;
  }
  dm_start = 0.0;
  dm_step = 0.02;
  dm_num = 1000;
  ChanDedsprs result = init_chan_sum(cdata, 8.8e7, dm_start, dm_step,
                                              dm_num);
  std::cout<<"DM   Sum index start   Sum index end    method    Dedsps map  result idx   result ref idx"<<std::endl;
  for ( i = 0; i < result.sum_idxs.size(); i++ ){
    std::cout<< dm_start + i * dm_step << " " <<result.sum_idxs[i][0]<< " ";
    std::cout<< result.sum_idxs[i][1] << " " << result.dm_method[i].method_id;
    std::cout<<" " <<result.dedsprs_ref_map[i] << " ";
    std::cout<< result.dm_method[i].result_idx << " "<< result.dm_method[i].ref_result_idx <<std::endl;
  }

  // std::cout << " Dedsps indexs    Dedsps map"<<std::endl;
  // for (i = 0; i < result.dedsprs_idxs_dm.size(); i++){
  //   std::cout << result.dedsprs_idxs_dm[i] << " ";
  // }

  chan_sum(0, 300, cdata, result);

  in_data.open("example_data.txt");
  out_result.open("out_result");
  for (i = 0; i < cdata.data.size(); i++){
    in_data << cdata.data[i] << " ";
  }

  for ( i = 0 ; i < result.data.size(); i++ ){
    for ( j = 0; j < result.data[i].size(); j++){
      out_result << result.data[i][j] << " ";
    }
    out_result << std::endl;
  }

  return 0;
}
