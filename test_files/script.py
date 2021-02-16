from os import listdir
from os.path import isfile, join
mypath = './'
files = [f for f in listdir(mypath) if isfile(join(mypath, f))]
for file in files:
    string = ""
    with open(file, mode='r') as f:
        string = f.read()
        string = string.replace('X', 'O')
    with open(file, mode='w') as f:
        f.write(string)
