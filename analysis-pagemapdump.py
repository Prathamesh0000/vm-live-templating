import re
import sys
if len(sys.argv) <= 1 :
    print("python3 analysis-pagemapdump.py <pid>")
    exit()

pid = sys.argv[1]

import subprocess

with open(pid + ".txt", 'w') as f:
    p = subprocess.call(["sudo", "/home/prathamesh/Documents/vm-live-templating/pagemap.o", pid], stdout=f)
    # p_status = p.wait()

# maps_file = open("/proc/"+str(pid)+"/maps", 'r')
maps_file = open("./"+str(pid)+".txt", 'r')
lines =  maps_file.readlines()[1:]
pft_to_vfn = {}

for line in lines:  # for each mapped region
    m = re.match(r'([0-9A-Fa-f]+) ([0-9A-Fa-f]+) ([0-9]) ([0-9]) ([0-9]) ([0-9]) (.?)+', line)
    if not m :
        print(line)
        continue
    # print(m)
    if m.group(2) not in pft_to_vfn:
        pft_to_vfn[m.group(2)] = {m.group(1)}
    else:
        pft_to_vfn[m.group(2)].add(m.group(1))
    
multi_cnt = 0
for key in pft_to_vfn:
    if key != "0" and len(pft_to_vfn[key]) >= 2 :
        multi_cnt = multi_cnt + 1
        print(key + " " + str(pft_to_vfn[key]))

print(str(len(pft_to_vfn)) + " " + str(multi_cnt))