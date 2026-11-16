import struct,sys
d=open(sys.argv[1],'rb').read()
def decomp(src,n):
    out=bytearray(); p=src-0x08000000
    while len(out)<n:
        b=d[p]; p+=1
        lit=b&7
        if lit==0: lit=d[p]; p+=1
        run=b>>4
        if run==0: run=d[p]; p+=1
        for _ in range(lit-1): out.append(d[p]); p+=1
        if b&8:
            off=d[p]; p+=1
            for _ in range(run+2): out.append(out[-off])
        else:
            out.extend(b'\0'*run)
    return bytes(out[:n]),p+0x08000000
ram,end=decomp(0x0800861c,0x2f0)
print('compressed end',hex(end))
open(sys.argv[2],'wb').write(ram)
