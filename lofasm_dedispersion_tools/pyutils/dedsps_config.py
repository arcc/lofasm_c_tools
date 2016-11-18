import astropy.units as u
import utils as ut


class dedsps_config(object):
    def __init__(self, config_file=None):
        """If the config_file name is given. It will automaticlly read the
        configuration file.
        Parameter
        ----------
        config_file : str, optional
            configuration file name.
        """
        self.config_file = config_file
        self.dm_start = 0.0 * u.pc/(u.cm)**3
        self.dm_end = 0.0 * u.pc/(u.cm)**3
        self.time_start = 0.0 * u.day
        self.time_end = 0.0 * u.day
        self.freq_start = 0.0 * u.MHz
        self.freq_end = 0.0 * u.MHz
        self.max_delay = 0.0 * u.s
        self.channel = ''
        self.station = ''
        if config_file is not None:
            self.read_config(config_file)
            self.max_delay = ut.compute_time_delay(self.dm_end, self.freq_start,
                             self.freq_end)
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
                try:
                    unit = getattr(self, line[0]).unit
                except AttributeError:
                    unit = None
            try:
                val = float(line[1])
            except ValueError:
                val = line[1]
            if unit is not None:
                setattr(self, line[0], val*unit)
            else:
                setattr(self, line[0], val)

    def process_info(self):
        self.max_delay = ut.compute_time_delay(self.dm_end, self.freq_start,
                         self.freq_end)
        self.time_start_J2000 = self.time_start - (2451545.0 - 2400000.5) * u.day
        self.time_end_J2000 = self.time_end - (2451545.0 - 2400000.5) * u.day

    def get_required_data_file(self, file_info):
        """This is a function to select the required file from the keys:
        LoFASM station,
        channel,
        start_time,
        end_time,
        start_freq,
        end_freq.
        """
        info_table = table.Table.read(info_file, format='ascii.ecsv')
        tbgrp = info_table.group_by(['lofasm_station','channel'])
        pass
