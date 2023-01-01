import pygame as pg

class Setting():
    
    def __init__(self):
        self.__tick = 48
        self.__allowId = [0, 2, 4, 5, 6, 7]
        
        '''
        0 -- no
        1 -- wall
        2 -- monsterBlock
        3 -- door
        4 -- key
        5 -- door open
        6 -- cast
        7 -- exit
        '''
    
    @property
    def tick(self):
        return self.__tick
    
    def isAllowed(self, id):
        for i in self.__allowId:
            if id == i:
                return True
        return False


def bgColor():
    return (100,100,100)
    
def isNear(nx, ny, item):
    sets = Setting()
    if (abs(nx - item.x) + abs(ny - item.y)) == sets.tick:
        return True
    else:
        return False
    
def isMouseOnIt(cor, x, y):
    if cor[0] <= x+48 and cor[0] >= x and cor[1] <= y+48 and cor[1] >= y:
        return True
    return False

def printMsg(playerPanel, screen, msg, color, bg):
    font1 = pg.font.SysFont('fangsong', 34)
    fail = font1.render(msg, True, color, bg)
    playerPanel.blit(fail, (3, 600))
    screen.blit(playerPanel, (961,0))
    pg.display.update()
    pg.time.delay(1000)