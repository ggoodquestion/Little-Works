import urllib.request as ur

def WebFindString(string, url):
    re = ur.urlopen(url)
    htmlb = re.read()
    html = htmlb.decode(encoding = "utf-8", errors = "ignore")
    if(html.find(string) == -1):
        print(string + " is not in " + url)
        
    else:
        print(string + " is in " + url)
