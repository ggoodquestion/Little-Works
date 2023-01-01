#-*- coding: cp950 -*-
import re
import urllib.request as ur

def sortDict(r1, r2):
    for i in range(len(r1)):
        for j in range(len(r2) - i):
            st = len(r1) - j - 1
            if r1[st] < r1[st-1]:
                tmp = r1[st]
                r1[st] = r1[st-1]
                r1[st-1] = tmp
                tmp = r2[st]
                r2[st] = r2[st-1]
                r2[st-1] = tmp

url = "http://data.ntpc.gov.tw/od/data/api/54DDDC93-589C-4858-9C95-18B2046CC1FC?$format=xml"
res = ur.urlopen(url)
html = res.read().decode(encoding = "utf-8", errors = "ignore")

tmp1 = re.compile("<sarea>\w+")
res1 = tmp1.findall(html)
dis = []
total = []
for i in res1:
    try:
        dis.index(i)
    except:
        dis.append(i)
        total.append(0)

tmp2 = re.compile("<tot>\d+")
res2 = tmp2.findall(html)

for i in range(len(res2)):
    a = dis.index(res1[i])
    str = res2[i].replace("<tot>", "")
    total[a] += int(str)
sortDict(total, dis)

for i in range(len(total)):
    a = dis[i].replace("<sarea>","")
    print("[{}] 車位共有 :  {}    格".format(a, total[i]))
#這份文件的XinZhuang有兩個