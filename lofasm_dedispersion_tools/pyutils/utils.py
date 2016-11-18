import astropy.units as u
DMconst = 1.0/2.41e-4 * u.MHz * u.MHz * u.s * u.cm**3 / u.pc

def compute_time_delay(DM, target_freq, ref_freq):
    """This is a function to compute dispersion time delays for between two freqencies.

    Parameter
    ---------
    DM : astropy quantity
        Dispersion measure value
    target_freq : astropy quantity
        The radio frequency that time delay is computed at.
    ref_freq : astropy quantity
        The reference frequency that use to compute time delay.
    """
    dt = DM * DMconst*(1/target_freq**2 - 1/ref_freq**2)
    return dt 
