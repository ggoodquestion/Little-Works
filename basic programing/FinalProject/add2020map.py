import os

fname = input('Enter file name:')
os.chdir('./')
file = open(fname, 'w+')
for i in range(20):
    for j in range(20):
        if j != 19:
            file.write('1 ')
        else:
            file.write('1')
    if i != 19:
        file.write('\n')