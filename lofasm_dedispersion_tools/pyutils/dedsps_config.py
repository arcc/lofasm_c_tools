import astropy.units as u
import utils as ut


class DedspsConfig(object):
    def __init__(self, config_file=None):
        """If the config_file name is given. It will automaticlly read the
        configuration file.
        Parameter
        ----------
        config_file : str, optional
            configuration file name.
        """
        self.keys = ['station', 'channel', 'time_start', 'time_end', 'freq_start',
                     'freq_end', 'dm_start', 'dm_end', 'dm_step']
        self.config_file = config_file
        self.dm_start = 0.0 * u.pc/(u.cm)**3
        self.dm_end = 0.0 * u.pc/(u.cm)**3
        self.time_start = 0.0 * u.second
        self.time_end = 0.0 * u.second
        self.dm_step = 0.0 * u.pc/(u.cm)**3
        self.freq_start = 0.0 * u.MHz
        self.freq_end = 0.0 * u.MHz
        self.max_delay = 0.0 * u.s
        self.channel = ''
        self.station = ''
        if config_file is not None:
            self.read_config(config_file)
            self.process_info()

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
            line = l.split(' ')
            if hasattr(self, line[0]):
                att = getattr(self, line[0])
                if hasattr(att, 'unit'):
                    unit = getattr(self, line[0]).unit
                else:
                    unit = None
            try:
                val = float(line[1])
            except ValueError:
                val = line[1]
            if unit is not None:
                setattr(self, line[0], val*unit)
            else:
                setattr(self, line[0], val)
            if line[0] not in self.keys:
                 self.keys.append(line[0])
        f.close()

    def write_config(self, filename):
        """ Write out a Configruation file.
        """
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
        f.close()
        
    def process_info(self):
        self.max_delay = ut.compute_time_delay(self.dm_end, self.freq_start,
                         self.freq_end)
        self.time_end_delay = self.time_end + self.max_delay

    def get_required_data_file(self, file_info, add_delay=False):
        """This is a function to select the required file from the keys:
        LoFASM station,
        channel,
        start_time,
        end_time,
        start_freq,
        end_freq.
        """
        info_table = table.Table.read(info_file, format='ascii.ecsv')
        match_key_map = {'station': 'lofasm_station', 'channel': 'channel'}
        time_end = self.time_end
        time_start = self.time_start
        if add_delay:
            time_end += self.time_end_delay
        range_key_map = {'time' : [time_start, time_end],
                         'freq' : [self.freq_start, self.freq_end]}
        select_file = []

        # Process the match key first
        for key, val in zip(match_key_map.keys(), match_key_map.values()):
            condition = getattr(self, key)
            return_file = info_table.get_files_by_key_name(val, condition)
            for rf in return_file:
                if rf not in select_file:
                    select_file.append(rf)
                else:
                    continue
        # Process the range key
        for key, val in zip(match_key_map.keys(), match_key_map.values()):
            return_file = info_table.get_files_by_range(val, val[0], val[1])
            for rf in return_file:
                if rf not in select_file:
                    select_file.append(rf)
                else:
                    continue
        return select_file
