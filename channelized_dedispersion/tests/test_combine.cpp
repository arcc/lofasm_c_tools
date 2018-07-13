// This .cpp file is to test the combine of result

#include<math.h>
#include<string>
#include<vector>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"
#include "dedispersion.h"

// Testing the dedispersion and combine functions
// Function covered
// init_chan_sum
// chan_sum
// combine_chan_result

int main(){
  double start_freq;
  double freq_step;
  int num_freq;
  double max_freq;
  double start_dm;
  double dm_step;
  int num_dm;
  double start_time;
  double time_step;
  int num_time_bin;
  int num_dedsprs;
  int i, j;
  std::ofstream in_data;
  std::ofstream out_result;

  std::cout<<"Start"<<std::endl;
  std::vector <ChanData> cdatas(2);
  std::vector <ChanDedsprs> cres;

  // Time information
  start_time = 0;
  num_time_bin = 100;
  time_step = 0.08;      // Time step is 1s

  // Frequency information
  start_freq = 2e7;
  freq_step = 0.09e6;

  // Initialize chan data
  for ( i = 0; i < 2; i++){
    cdatas[i].start_time = start_time;
    cdatas[i].time_step = time_step;
    cdatas[i].num_time_bin = num_time_bin;
    cdatas[i].freq = start_freq + i * freq_step;
    cdatas[i].chan_bandwidth = freq_step;
    cdatas[i].data.resize(num_time_bin, 1.0);
    std::cout<<" Size "<< cdatas[i].data.size()<<std::endl;
  }

  // Input the simulated signal into channel data.
  cdatas[1].data[20] = 3.0;
  cdatas[0].data[21] = 3.2;
  cdatas[0].data[22] = 3.3;

  // Output input data
  in_data.open("combine_indata.txt");
  for (i = 0; i < cdatas.size(); i++){
    for ( j = 0; j < num_time_bin; j++ ){
      in_data << cdatas[i].data[j] << " ";
    }
    in_data << std::endl;
  }

  in_data.close();

  // Initialze dedispersion
  start_dm = 0.0;
  dm_step = 0.05;
  num_dm = 400;

  num_dedsprs = 70;
  // init the final result
  DedsprsResult result(start_time, time_step, num_dedsprs, start_dm, dm_step, \
                       num_dm);
  std::cout<< result.data.size()<<std::endl;

  for(i = 0 ; i < cdatas.size(); i++ ){
    cres.push_back(init_chan_sum(cdatas[i], cdatas[1].freq, start_dm, dm_step, num_dm));
    // Match the time bin with final result
    cres[i].num_time_bin = num_time_bin;
    cres[i].start_time = start_time;
  }

  // sum Channelized data
  for(i = 0 ; i < cdatas.size(); i++ ){
    chan_sum(0, num_dedsprs, cdatas[i], cres[i]);
    std::cout<<"Combine "<<i<<std::endl;
    combine_chan_result(cres[i], result);
  }

  // Manually do normalization

  for(i = 0; i < cres.size(); i++){
    for (j=0; j < cres[i].num_dm_bin; j++){
      result.norm_array[j] += cres[i].sum_idxs[j][1] - cres[i].sum_idxs[j][0] + 1;
    }
  }

  for (j=0; j < num_dm; j++){
    std::cout<<result.norm_array[j]<<std::endl;
  }

  // Output input data and apply normalization
  // NOTE the right way should be apply normalization and out put.
  out_result.open("combine_result.txt");
  for (i = 0; i < result.data.size(); i++){
    for ( j = 0; j < result.num_time_bin; j++ ){
      out_result << result.data[i][j] / (double)result.norm_array[i] << " ";
    }
    out_result << std::endl;
  }

  out_result.close();

  return 0;
}
