import os
import subprocess
import dedsps_config as dpc
import lofasm.bbx.bbx as bbx
from lofasm.data_file_info.data_file_info import LofasmFileInfo
from astropy.table import Table
import astropy.units as u
from astropy import log
import numpy as np
from dedsps_config import DedspsConfig
from utils import compute_min_smear_size
import copy
import argparse


def get_lofamsio_path():
    pass

def split_dedispersion_time(config_cls, sampling_time, sampling_freq, max_time_bin):
    """This is a function to preprocess the dedispersion config file. If the time
    required for dedispersion is larger than the max_data_span, it will break the
    time to truncks and saved into different config files.
    Parameter
    ---------
    config_cls : DedspsConfig object
        The object for get config file
    sampling_time: astropy quantity
        The sampling time for filter bank data.
    sampling_freq: astropy quantity
        The sampling frequency for filter band data.
    max_time_bin: int
        The max time bin for dedispersion.
    """
    config = config_cls
    max_delay = config.max_delay
    print max_delay
    spt =  sampling_time
    max_data_span = spt * max_time_bin
    if max_data_span - max_delay <= 0:
        log.warning("The allowed max data span is smaller than the"\
                    " required data. Data will be down sampled for this"
                    " dedispersion.")
        size = compute_min_smear_size(config_cls.dm_end, config_cls.freq_end, \
                                      sampling_freq, sampling_time)
        spt = spt * size
        max_data_span = spt * max_time_bin
        if max_data_span - max_delay <= 0:
            raise RuntimeError("Allowed max data span it too small. "\
                               "You can try to adjust the max data bin size or " \
                               "split the DM searching band.")
    config.sampling_time = spt
    if config.time_end.value != 0:
        total_data_span = config.time_end - config.time_start + max_delay
    # Breaking the total time to truncks.
    i = 0
    new_config = copy.deepcopy(config)
    while total_data_span > max_data_span:
        i += 1
        new_time_end = max_data_span - max_delay + new_config.time_start
        new_config.time_end = new_time_end
        new_config.config_file = config.config_base_name + '_part%s' % i + '.txt'
        new_config.write_config(os.path.join(new_config.script_dir, new_config.config_file))
        temp = new_config
        new_config = copy.deepcopy(new_config)
        new_config.time_start = temp.time_end
        total_data_span = config.time_end - new_config.time_start + max_delay
        del temp
    # Write the last config file.
    new_config.time_end = config.time_end
    new_config.config_file = config.config_base_name + '_part%s' % (i+1) + '.txt'
    new_config.write_config(os.path.join(new_config.script_dir, new_config.config_file))
    return 0

def select_files_by_config(config_cls, file_info, files=None):
    """
    This is a function to select files by time from a directory or a list of
    files. If both argument are given, it will take the directory as input

    Parameter
    ---------
    config_cls: DedspsConfig object
        The dedispersion configuration object
    file_info: LofasmFileInfo object
        Directory name is pass a directory
    files: list/str, optional
        files for dedispersion
    """
    fs = set([])
    for ii, k in enumerate(config_cls.kw_keys):
        print k, getattr(config_cls, k), config_cls.config_file
        result = file_info.search_files('key', True, column_name=k, \
                                  condition = getattr(config_cls, k))
        if ii == 0:
            fs = set(result)
        else:
            fs = fs.intersection(set(result))
    if config_cls.time_start.value != 0.0 and config_cls.time_end.value != 0.0:
        time_result = file_info.search_files('time', True, search_range=[config_cls.time_start.value,
                                        config_cls.time_end.value + config_cls.max_delay.value])
    fs = fs.intersection(set(time_result))
    return list(fs)

def combine_bbx_files(files, outfile=None):
    """
    This is a function that sort a list of lofasm bx format filter bankdata
    files and combine them to one file using lofasmio c code.
    Parameter
    ---------
    list: A list of lofasm BBX format files.
    """
    start_times = []
    end_times = []
    stations = []
    channels = []
    for f in files:
        if not bbx.is_lofasm_file(f):
            raise IOError("File '%s' is not a lofasm bbx format file." % f)
        bbxf = bbx.LofasmFile(f)
        if bbxf.header['hdr_type'] != 'LoFASM-filterbank':
            raise IOError("File '%s' is not a LoFASM filterbank data file."
                           % f)
        st = float(bbxf.header['dim1_start']) + \
             float(bbxf.header['time_offset_J2000'].split()[0])
        start_times.append(st)
        end_times.append(st + float(bbxf.header['dim1_span']))
        channels.append(bbxf.header['channel'])
        stations.append(bbxf.header['station'])
    files = np.array(files)
    t = Table([files, stations, channels, start_times, end_times], names = [
               'filename', 'station', 'channel', 'start_time','end_time'])
    # Check consistance of data origin.
    for chk_info in ['station', 'channel']:
        if np.count_nonzero(t[chk_info] == t[chk_info][0]) != len(t[chk_info]):
            raise IOError("All the data files must come from the same '%s'."
                            % chk_info)
    # sort time
    t.sort('start_time')
    # Start combine the files.
    if outfile is None:
        outfile = 'Combined_lofasm_' + str(t['start_time'][0]) + '_' + \
                  str(t['end_time'][-1]) + '_' + t['channel'][0] + '.bbx.gz'
    #TODO need to change here
    cmd = 'lfcat '
    for fn in t['filename']:
        cmd += fn + ' '
    cmd += outfile
    print('RUNNING: '+ cmd)
    subprocess.call(cmd, shell=True)
    print('Finished combining data')
    return outfile

def chop_bbx_file(filename, chop_start_time, chop_end_time=None, outfile=None):
    """
    This is a function for wrapper the lofasmio lfchop function.
    Parameter
    ---------
    filename: str
        File name
    chop_start_time: float
        The chop starting time in second(time past J2000).
    chop_end_time: float, optional
        The chop ending time. If None, it will assume the end of file.
    outfile: str
        Output file name.
    """
    bbxf = bbx.LofasmFile(filename)
    start_time = float(bbxf.header['dim1_start']) + \
                 float(bbxf.header['time_offset_J2000'].split()[0])
    end_time = start_time + float(bbxf.header['dim1_span'])
    if chop_start_time > end_time or chop_start_time < start_time:
        raise ValueError("The chop start time is not in the file time range.")

    if chop_end_time is None:
        chop_end_time = end_time
    if chop_end_time > end_time or chop_end_time < start_time:
        raise ValueError("The chop end time is not in the file time range.")

    # call lofasmio
    # TODO need to change here
    if outfile is None:
        outfile = 'Chopped_lofasm_' + str(chop_start_time) + '_' + \
                  str(chop_end_time) + '_' + bbxf.header['channel'] + '.bbx.gz'
    cmd = 'lfchop -t ' + str(chop_start_time) + '+' \
          + str(chop_end_time) + ' ' + filename + ' ' + outfile
    subprocess.call(cmd, shell=True)
    print('Finished chopping data')
    return outfile

def slice_bbx_file(filename, slice_start_freq, slice_end_freq, outfile=None):
    """
    This is a function for wrapper the lofasmio lfchop function.
    Parameter
    ---------
    filename: str
        File name
    slice_start_freq: float
        The slice starting frequency in Hz.
    slice_end_freq: float, optional
        The slice ending frequency.
    """
    bbxf = bbx.LofasmFile(filename)
    if outfile is None:
        outfile = 'slice_lofasm_' + str(slice_start_freq/1e6) + '_' + \
                  str(slice_end_freq/1e6) + '_' + bbxf.header['channel'] + '.bbx.gz'
    cmd = 'lfslice -f ' + str(slice_start_freq) + '+' \
          + str(slice_end_freq) + ' ' + filename + ' ' + outfile
    print("RUNNING: "+ cmd)
    subprocess.call(cmd, shell=True)
    print('Finished slicing data')
    return outfile

def write_dedispersion_script(config_cls, file_info):
    """This is a function to write out the script for doing dedispersion based
    on the config class and directory information class.
    Parameter
    ---------
    config_cls: DedspsConfig object
        Configruation class
    file_info: LofasmFileInfo object
        Lofasm file directory information system.
    Note
    ----
    We have not consider the frequency band yet
    """
    files = select_files_by_config(config_cls, file_info)
    config_cls.include_files = files
    config_cls.write_config(config_cls.config_file)
    file_example = file_info.get_one_file_info(files[0])
    sampling_time = file_example['sampling_time'][0] * u.second
    down_sample_size = (int)(config_cls.sampling_time / sampling_time)
    out_name = os.path.join(config_cls.script_dir, config_cls.config_base_name + '.sh')
    f = open(out_name, 'w')
    f.write("#!/bin/bash\n")
    out_line = "echo 'Starting dedispersion for configuration %s.'\n" % \
               config_cls.config_base_name
    # Combining data
    cmd = 'lfcat '
    for fn in files:
        cmd += fn + ' '
    cmd += '-'
    out_line += "echo 'Preparing data.'\n"
    out_line += cmd + ' | \n'
    # Chop the data
    chop_start_time = config_cls.time_start.value
    chop_end_time = config_cls.time_end.value + config_cls.max_delay.value
    cmd = 'lfchop -t ' + str(chop_start_time) + '+' \
          + str(chop_end_time) + ' ' + '-' + ' ' + '-'
    out_line += cmd + ' | \n'
    # Slice data
    slice_start_freq = config_cls.freq_start.value * 1e6
    slice_end_freq = config_cls.freq_end.value * 1e6
    out_data_file = os.path.join(config_cls.result_dir, \
                    config_cls.config_base_name + '.bbx.gz')
    cmd = 'lfslice -f ' + str(slice_start_freq) + '+' \
          + str(slice_end_freq) + ' ' + '-'+ ' ' + out_data_file
    out_line += cmd + '  \n'

    out_line += "echo Normalizing data.\n"
    # Normalize data.
    ###TODO need to complete here.
    cmd = "normalize_data.py " + out_data_file +
    if down_sample_size > 1:
        out_line += "echo 'Down sample'"

    out_line += "echo 'Finishing Preparing data.'\n"
    # dedispersion program
    out_dedsps_file = os.path.join(config_cls.result_dir, "dedispersed_" + \
                                    config_cls.config_base_name + ".bbx.gz")
    cmd = "dedispersion -df "+out_data_file + " -cf " + config_cls.config_file +\
         " -o " + out_dedsps_file + "\n"
    out_line += cmd
    f.write(out_line)
    f.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test run dedispersion functions.")
    parser.add_argument("files",help="Filter bank data functions", nargs='*')
    parser.add_argument("--st")
    args = parser.parse_args()
    st = args.st
    config = DedspsConfig('test_config.txt')
    #print select_files_by_config(config, args.files[1])
    #print len(select_files_by_config(config, args.files[1]))
    lfi = LofasmFileInfo(args.files[1], check_subdir=True)
    print lfi.info_table
    print lfi.get_info_all_dirs('time_span')
