import os
import subprocess
import dedsps_config as dpc
from lofasm.data_file_info.data_file_info import LofasmFileInfo
import argparse
import astropy.units as u
import run_dedispersion_utils as rdu
MAX_TIME_BIN = 10000


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Command line interfact to run"
                                     " dedispersion.")
    parser.add_argument("config",help="Dedispersion Configration file name")
    parser.add_argument("-mode", help="Run dedispersion based on a set of file")
    parser.add_argument("-input",help="input data file or a directory", nargs='*')
    parser.add_argument("--o", help="Out put result directory", default=None)



    # parser.add_argument("--list_generators", help="List all the built-in signal" \
    #                     " and noise generators", action=ListGenAction, nargs=0)
    # parser.add_argument("--generator_help", help="Print the help for a built-in" \
    #                     " data generator", action=GenHelpAction)
    # parser.add_argument("--plot",help="Plot simulated filter bank data",
    #                     action="store_true",default=False)
    # parser.add_argument("--saveplot",help="Save simulate data plot",
    #                     action="store_true",default=False)
    # parser.add_argument("--savefile",help="Save intermeida files",
    #                     action="store_true",default=False)
    args = parser.parse_args()
    configf = args.config
    in_file = args.input
    result_dir = args.o
    # Read configuration.
    config = dpc.DedspsConfig(configf, result_dir=result_dir)
    # get data directory information
    # TODO We need check mood here
    dir_info = LofasmFileInfo(in_file[0], add_col_names=['sampling_time', \
                                                         'sampling_freq'], \
                                                         check_subdir=False)
    # If the time is not specified, the directory start time and end time will
    # be taken as the configuration start and end time.
    if config.time_start.value == 0.0 or config.time_end.value == 0.0:
        _, start_times = dir_info.get_info_all_dirs('start_time_J2000')
        config.time_start = np.array(start_times).min() * u.second
        _, time_spans = dir_info.get_info_all_dirs('time_span')
        end_times = np.array(start_times) + np.array(time_spans)
        config.time_end = end_times.min() * u.second

    # NOTE We are assuming that the sampling frequency will not be changed amount
    # lofasm files. And the unit is Hz
    lofasm_files = dir_info.search_files('key', True, column_name='hdr_type', \
                                         condition='LoFASM-filterbank')
    example_file = dir_info.get_one_file_info(lofasm_files[0])
    sampling_freq = example_file['sampling_freq'].data[0] * u.Hz
    # NOTE we assume the sampling time is all the same here and the unit is second
    # TODO we need to consider the sampling time will be different
    sampling_time = example_file['sampling_time'].data[0] * u.second
    # TODO Before this step we should split the DM band.
    rdu.split_dedispersion_time(config, sampling_time, sampling_freq, MAX_TIME_BIN)
    # Go to dedispersion_scripts directory
    for f in os.listdir(config.script_dir):
        sconfig_path = os.path.join(config.script_dir, f)
        # try the read the splited config files.
        try:
            cfg = dpc.DedspsConfig(sconfig_path, result_dir)
        except:
            continue
        rdu.write_dedispersion_script(cfg, dir_info)
