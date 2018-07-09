// This cpp file defines the dedispersion high-level functions

#include<math.h>
#include<string>
#include<vector>
#include<stdio.h>
#include<iostream>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"
#include "dedispersion.h"

ChanDedsprs init_chan_sum (ChanData & channel_data, double ref_freq,
                                    double dm_start, double dm_step, int num_dm)
{
  int i;
  ChanDedsprs chan_dedsprs_result(channel_data.freq, channel_data.chan_bandwidth);

  // Initialize the dedisperison parameters.
  chan_dedsprs_result.ref_freq = ref_freq;
  chan_dedsprs_result.start_dm = dm_start;
  chan_dedsprs_result.dm_step = dm_step;
  chan_dedsprs_result.num_dm_bin = num_dm;
  chan_dedsprs_result.time_step = channel_data.time_step;

  // Compute sum index and orgnaize dedispersion.
  chan_dedsprs_result.get_sum_idxs();
  chan_dedsprs_result.get_identical_sum_idx();
  chan_dedsprs_result.get_dedispersion_idx();
  chan_dedsprs_result.organize_dedispersion();

  return chan_dedsprs_result;
}

int chan_sum (int dedsprs_start_idx, int dedsprs_end_idx,
              ChanData & channel_data, ChanDedsprs & dedsprs_res)
// NOTE: This function only sums the data does not normalize the result.
{
  int i, j;
  int num_dm_out;
  int num_time_out;
  int ref_result_idx;
  int ref_dm_idx;
  int dm_idx;
  int result_idx;

  num_time_out = dedsprs_end_idx - dedsprs_start_idx + 1;
  num_dm_out = dedsprs_res.dedsprs_idxs_dm.size();

  // resize data
  dedsprs_res.data.resize(num_dm_out, std::vector<double> (num_time_out, 0.0) );
  //start Dedisperison
  for (i = 0 ; i < num_dm_out; i++){
    dm_idx = dedsprs_res.dedsprs_idxs_dm[i];
    ref_dm_idx = dedsprs_res.dm_method[dm_idx].ref_dm_idx;
    ref_result_idx = dedsprs_res.dm_method[dm_idx].ref_result_idx;
    result_idx = dedsprs_res.dm_method[dm_idx].result_idx;
    dedsprs_res.dm_method[dm_idx].method( dedsprs_res.sum_idxs[ref_dm_idx][0],
                                          dedsprs_res.sum_idxs[ref_dm_idx][1],
                                          dedsprs_res.sum_idxs[dm_idx][0],
                                          dedsprs_res.sum_idxs[dm_idx][1],
                                          num_time_out, channel_data.data,
                                          dedsprs_res.data[ref_result_idx],
                                          dedsprs_res.data[result_idx]);
  }
  return 0;
}

int combine_chan_result(ChanDedsprs & chan_res, DedsprsResult & result)
// This function assums the channlized results have the time resolutiona and
// time resolution.
// The combine process uses the final result's time and DM as the reference.
// In other words chan_res will be matched to result.
{
  int i, j;
  int start_cmb_time_idx;
  int start_cmb_dm_idx;
  double start_time_diff;
  double start_dm_diff;

  std::vector<double> chan_time_axis = chan_res.get_time_axis();
  std::vector<double> chan_dm_axis = chan_res.get_dm_axis();

  // Match the start time index.
  start_time_diff =  result.time_axis.front() - chan_time_axis.front();
  if (start_time_diff < 0){
    std::cerr << "Channelized dedispersion result does not have the requested ";
    std::cerr << " requested time (0)." << std::endl;
    std::exit(1);
  }
  start_cmb_time_idx = (int)(start_time_diff / result.time_step);
  if (start_cmb_time_idx + result.num_time_bin > chan_time_axis.back()) {
    std::cerr << "Channelized dedispersion result does not have enough data";
    std::cerr << " for requested time (1).";
    std::exit(1);
  }

  // Match the DM index
  start_dm_diff = result.dm_axis.front() - chan_dm_axis.front();
  if (start_dm_diff < 0){
    std::cerr << "Channelized dedispersion result does not have the requested ";
    std::cerr << " requested DM (0)." << std::endl;
    std::exit(1);
  }

  start_cmb_dm_idx = (int) (start_time_diff / result.time_step);
  if (start_cmb_dm_idx + result.num_dm_bin > chan_dm_axis.back()) {
    std::cerr << "Channelized dedispersion result does not have enough data";
    std::cerr << " for requested time (1).";
    std::exit(1);
  }

  for ( i = 0; i < result.num_dm_bin; i++ ){
    for ( j = 0; j < result.num_time_bin; j++ ){
      result.data[i][j] += chan_res.data[start_cmb_dm_idx][start_cmb_time_idx];
    }
  }
}
