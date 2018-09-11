// This is a header file to declear the dedispersion algorithm functions
// Author: Jing Luo

#ifndef DEDISPERSION_H_
#define DEDISPERSION_H_

#include<math.h>
#include<stdio.h>
#include<iostream>
#include<string>
#include<vector>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"
#include "dedispersion_io.h"


ChanDedsprs init_chan_sum (ChanData & channel_data, double ref_freq,
                                    double dm_start, double dm_step, int num_dm);

int init_dedsprs_result_class (double dm_start, double dm_step, int num_dm);

int load_data (std::vector <ChanData> &target, DataFileContainer &data);

int chan_sum (int dedsprs_start_idx, int dedsprs_end_idx,
              ChanData & channel_data, ChanDedsprs & dedsprs_res);

//int get_mask_normalize_num(int dedsprs_start_idx, int dedsprs_end_idx,
//                           ChanData & channel_mask, ChanDedsprs & mask_res);

int combine_chan_result(ChanDedsprs & chan_res, DedsprsResult & result);

#endif   // DEDISPERSION_H_
