// This is a header file to declear the dedispersion high-level functions
// Author: Jing Luo

#ifndef DEDISPERSION_H_
#define DEDISPERSION_H_

#include<math.h>
#include<string>
#include<vector>
#include "dedispersion_class.h"
#include "dedispersion_utils.h"

int chan_dedispersion (double chan_freq, double freq_step,
                       std::vector<double> & channel_data);


#endif   // DEDISPERSION_H_
