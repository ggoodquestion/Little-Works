import pygame
import os
from Block import block as bk
from Setting import setting

os.chdir('./')
tick = setting.Setting().tick
fname = 'map.txt'



def drawMap(screen):
    map = open(fname)
    line = map.readline()
    a = 0
    item = []
    while line:
        list = line.split(' ')
        for i in range(len(list)):
            if(int(list[i]) == 1):
                wall = bk.Wall(screen)
                wall.blitme((i*tick, a*tick))
            if(int(list[i]) == 2):
                mb = bk.MonsterBlock(screen, x = i*tick, y = a*tick)
                mb.blitme((i*tick, a*tick))
                item.append(mb)
            if(int(list[i]) == 3):
                door = bk.Door(screen, x = i*tick, y = a*tick)
                door.blitme((i*tick, a*tick))
                item.append(door)
            if(int(list[i]) == 4):
                key = bk.Key(screen, x = i*tick, y = a*tick)
                key.blitme((i*tick, a*tick))
                item.append(key)
            if(int(list[i]) == 6):
                cast = bk.Cast(screen, x = i*tick, y = a*tick)
                cast.blitme((i*tick, a*tick))
                item.append(cast)
            if(int(list[i]) == 7):
                exit = bk.Exit(screen, x = i*tick, y = a*tick)
                exit.blitme((i*tick, a*tick))
                item.append(exit)
        a += 1
        line = map.readline()
        pygame.display.update()
    
    return item

def findBlockByPosition(x, y):
    map = open(fname)
    mapList = map.readlines()
    pos = []
    xl = mapList[int(y/tick)].split(' ')
    try:
        xp = xl[int(x/tick)-1]
        #print(int(xp))
        pos.append(int(xp))
    except:
        pos.append(-1)
        #print('-1')
    try:
        xn = xl[int(x/tick)+1]
        #print(int(xn))
        pos.append(int(xn))
    except:
        pos.append(-1)
        #print('-1')
    try:
        ypl = mapList[int(y/tick)-1].split(' ')
        yp = ypl[int(x/tick)]
        pos.append(int(yp))
        #print(int(yp))
    except:
        pos.append(-1)
        #print('-1')
    try:
        ynl = mapList[int(y/tick)+1].split(' ')
        yn = ynl[int(x/tick)]
        pos.append(int(yn))
        #print(int(yn))
    except:
        pos.append(-1)
        #print('-1')
    
    return pos