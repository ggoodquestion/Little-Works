import os 

#Choose one of below method to implement

#This way use union of set to compare
#Hung-Ta doesn't like this way
def compareFile(str1, str2):
    os.chdir("./")
    f1 = open(str1)
    f2 = open(str2)
    line1 = f1.readline()
    line2 = f2.readline()
    isIdentical = 1
    contain = []

    while line1 and line2:
    
        set1 = set(line1.split(" "))
        set2 = set(line2.split(" "))
        
        if(len(set1) != len(set1.union(set2))):
            isIdentical = 0
            contain.append(line2.upper())
        else:
            contain.append(line2)
        
        line1 = f1.readline()
        line2 = f2.readline()
    
    if isIdentical == 1:
        print("Identical file!")
    else:
        dif = open("difference1.txt", "w+", encoding = "utf-8")
        for x in contain:
            dif.write(x)
        dif.close()
    
    f1.close()
    f2.close()

#This way use readLines to compare
#Hung-Ta may like this way
def compareFile2(str1, str2):
    os.chdir("./")
    f1 = open(str1)
    f2 = open(str2)
    list1 = f1.readlines()
    list2 = f2.readlines()
    
    isIdentical = 1
    
    for x in range(len(list1)):
        if(list1[x] != list2[x]):
            list2[x] = list2[x].upper()
            isIdentical = 0
    
    if(isIdentical == 0):
        dif = open("difference2.txt", "w+")
        for x in list2:
            dif.write(x)
        dif.close()
    else:
        print("Identical file!")      
      
    f1.close()
    f2.close()

#This way is used to show off
#Hung-Ta may not like this way
#May be a little Hard to explain, welcome to challenge
def compareFile3(str1, str2):
    os.chdir("./")
    f1 = open(str1)
    f2 = open(str2)
    list1 = f1.readlines()
    list2 = f2.readlines()
    set1 = set(list1)
    set2 = set(list2)
    res = set1.union(set2)
    if(len(set1) == len(res)):
        print("Identical file!")
    else:
        dif = open("difference3.txt", "w+")
        for x in range(len(list1)):
            if list1[x] != list2[x]:
                dif.write(list2[x].upper())
            else:
                dif.write(list2[x])
        dif.close()
        
    f1.close()
    f2.close()
            

compareFile("first.txt", "second.txt")
compareFile("first.txt", "third.txt")

compareFile2("first.txt", "second.txt")
compareFile2("first.txt", "third.txt")

compareFile3("first.txt", "second.txt")
compareFile3("first.txt", "third.txt")