import astropy.units as u
import utils as ut
import os
import errno

class DedspsConfig(object):
    def __init__(self, config_file=None, result_dir=None):
        """If the config_file name is given. It will automaticlly read the
        configuration file.
        Parameter
        ----------
        config_file : str, optional
            configuration file name.
        """
        self.keys = {'station': str, 'channel': str, 'time_start': float,
                     'time_end': float, 'freq_start': float,
                     'freq_end': float, 'dm_start': float, 'dm_end': float,
                     'dm_step': float, 'sampling_time': float, 'config_prefix':str}
        self.kw_keys = ['station', 'channel']
        self.range_keys = ['time_start', 'time_end', 'freq_start', 'freq_end']
        self.config_file = config_file
        self.config_dir_path = os.path.dirname(os.path.abspath(config_file))
        self.config_prefix = ''
        self.dm_start = 0.0 * u.pc/(u.cm)**3
        self.dm_end = 0.0 * u.pc/(u.cm)**3
        self.time_start = 0.0 * u.second
        self.time_end = 0.0 * u.second
        self.dm_step = 0.0 * u.pc/(u.cm)**3
        self.freq_start = 0.0 * u.MHz
        self.freq_end = 0.0 * u.MHz
        self.sampling_time = 0.0 * u.second
        self.max_delay = 0.0 * u.s
        self.channel = ''
        self.station = ''
        self.include_files = []
        if config_file is not None:
            self.read_config(config_file)
            self.process_info()
            if self.dm_end.value == 0.0:
                raise ValueError("Missing 'dm_end' in the config file")
        self.input_result_dir = result_dir
        # Config prefix is not a property
        if self.config_prefix == '':
            self.config_prefix = self.config_base_name

    @property
    def config_base_name(self):
        bn = os.path.basename(self.config_file)
        return os.path.splitext(bn)[0]

    @property
    def result_dir_basename(self):
        return 'dedispersion_' + self.config_prefix+ '_dm_' + \
                 str(self.dm_start.value) + '_' + str(self.dm_end.value)
    @property
    def result_dir(self):
        if self.input_result_dir is not None:
            abspath = os.path.abspath(self.input_result_dir)
            return os.path.join(abspath, self.result_dir_basename)
        else:
            return os.path.join(self.config_dir_path, self.result_dir_basename)

    @property
    def script_dir(self):
        return os.path.join(self.result_dir, 'dedispersion_scripts')

    def read_config(self, config_file):
        """Read configuration files.
        Parameter
        ----------
        config_file : str
            Configruation file name
        """
        f = open(config_file, 'r')
        for l in f.readlines():
            if l.startswith("#"):
                continue
            l = l.strip()
            if l == '':
                continue
            line = l.split()
            if line[0] == 'INCLUDE':
                self.include_files.append(line[1])
            if hasattr(self, line[0]):
                att = getattr(self, line[0])
                if hasattr(att, 'unit'):
                    unit = getattr(self, line[0]).unit
                else:
                    unit = None
            if line[0] in self.keys.keys():
                val = self.keys[line[0]](line[1])
                if unit is not None:
                    setattr(self, line[0], val*unit)
                else:
                    setattr(self, line[0], val)
            else:
                 setattr(self, line[0], line[1])
                 self.keys[line[0]] = type(line[1])
        f.close()

    def write_config(self, filename):
        """ Write out a Configruation file.
        """
        if not os.path.exists(os.path.dirname(filename)):
            try:
                os.makedirs(os.path.dirname(filename))
            except OSError as exc: # Guard against race condition
                if exc.errno != errno.EEXIST:
                    raise

        f = open(filename, 'w')
        for k in self.keys:
            val = getattr(self, k)
            if isinstance(val, str):
                outstr = k + ' ' + val + '\n'
            else:
                if hasattr(val, 'unit'):
                    outstr = k + ' ' + str(val.value) + '\n'
                else:
                    outstr = k + ' ' + str(val) + '\n'
            f.write(outstr)
        for ff in self.include_files:
            outstr = 'INCLUDE ' + ff +'\n'
            f.write(outstr)
        f.close()

    def process_info(self):
        self.max_delay = ut.compute_time_delay(self.dm_end, self.freq_start,
                         self.freq_end)
        #self.time_end_delay = self.time_end + self.max_delay
