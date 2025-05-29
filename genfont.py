from PIL import Image
from itertools import islice

def batched(it, n):
    it = iter(it)
    while b := tuple(islice(it, n)):
        yield b

img = Image.open('font.png')
pix = img.load()
data = [0x00]
off = [0]

off.append(1)
for i in range(img.size[0]):
    b = 0
    for j in reversed(range(8)):
        b <<= 1
        b |= pix[i, j][0] & 1
    if b: data.append(b)
    else: off.append(i - len(off) + 3)
off.append(i - len(off) + 4)

with open('c/font.h', 'w', encoding='latin-1') as f:
    f.write('char fontchar[] =\n')
    f.write('  " !\\"#$%&\'()*+,-./0123456789')
    f.write(':;<=>?@ABCDEFGHIJKLMNOPQR"\n')
    f.write('  "STUVWXYZ[\\\\]^_`abcdefghijklm')
    f.write('nopqrstuvwxyz{|}~ÄÅÖäåéö";\n\n')

    f.write('short fontoff[] = {\n')
    for line in batched(off, 12):
        f.write('  ')
        first = True
        for n in line:
            if first: first = False
            else: f.write(', ')
            f.write(f'{n:3d}')
        f.write(f',\n')
    f.write('};\n\n')

    f.write('unsigned char fontdata[] = {\n')
    for line in batched(data, 10):
        f.write('  ')
        first = True
        for b in line:
            if first: first = False
            else: f.write(', ')
            f.write(f'0x{b:02x}')
        f.write(f',\n')
    f.write('};\n')
