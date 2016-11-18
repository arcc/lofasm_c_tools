import os
import subprocess
import dedsps_config as dpc
# Find local directory
# NOTE This is not working when the program copy to the bin dir
py_path = os.path.dirname(os.path.realpath(__file__))
base = py_path[:-34]
base_dir_names = [name for name in os.listdir(base)]
dedsps_dir_name = None
io_dir_name = None
for n in base_dir_names:
    if n.startswith('lofasm_dedispersion_tool'):
        dedsps_dir_name = n
    elif n.startswith('lofasmio-'):
        io_dir_name = n
io_path = os.path.join(base, io_dir_name)
dedsps_path = os.path.join(base, dedsps_dir_name)
# First read the dedispersion config file.
cf = dpc.dedsps_config(os.path.join(io_path, 'testdata/test_config.txt'))
att = dir(cf)
for n in att:
    print n, getattr(cf, n)
#
