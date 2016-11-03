import gzip

def check_lofasm_file(filename):
    hdr = {}
    if filename.endswith('.gz'):
        f = gzip.open(filename, 'rb')
    else:
        f = open(filename, 'rb')
    line = f.readline().strip().replace('%','')
    if line not in ['\x02BX','BX']:
        print line
        raise  "Not a lofasm file skip"
    for ii in range(17):
        line = f.readline()
        line = line.replace('%', '')
        line = line.replace(':', '')
        line = line.split()
        hdr[line[0]] = line[1]
    f.close()
    return hdr

if __name__=='__main__':
    filename  = 'testdata/20160619_000326_AA.bbx.gz'
    print check_lofasm_file(filename)
