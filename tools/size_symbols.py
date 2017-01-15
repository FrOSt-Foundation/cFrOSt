import json
with open("bin/FrOSt.bin.sym") as f:
    d = json.load(f)
x = [x for x in d.items() if not x[0].startswith('_')]

x.sort(key=lambda p: p[1]['addr'])

for i in range(0,len(x)-1):
    print("{}: {}".format(x[i+1][1]['addr'] - x[i][1]['addr'], x[i][0]))
