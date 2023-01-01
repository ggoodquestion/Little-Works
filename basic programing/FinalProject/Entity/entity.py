import pygame as pg
import random

class Entity():

    def __init__(self, screen, hp, atk, x, y, src):
        self.__img = pg.image.load(src)
        self.__hp = hp
        self.__atk = atk
        self.__screen = screen
        self.__x = x
        self.__y = y
        
    @property
    def Hp(self):
        return self.__hp
    
    def setHp(self, hp):
        self.__hp = hp
        
    @property
    def Atk(self):
        return self.__atk
    
    def setAtk(self, atk):
        self.__atk = atk
        
    def blitme(self, coordinate, nc):
        if coordinate[0] != nc[0] or coordinate[1] != nc[1]:
            surf = pg.Surface((48,48))
            surf.fill((100,100,100))
            self.__screen.blit(surf, nc)
        self.__screen.blit(self.__img, coordinate)
    
    @property
    def x(self):
        return self.__x
    
    @property
    def y(self):
        return self.__y
    
    def setPosition(self, x, y):
        self.__x = x
        self.__y = y
       
    @property    
    def img(self):
        return self.__img
        
class Player(Entity):
    
    def __init__(self, screen, hp = 20, atk = 3, x = 0, y = 0, src='./img/player.png'):
        super().__init__(screen, hp, atk, x, y, src)
        self.__inventory = []
        
    def Inventory(self):
        return self.__inventory
    
    def addItem(self, item):
        self.__inventory.append(item)
        
    def dead(self):
        print('lose')

class Monster(Entity):
    def __init__(self, screen, hp, atk, x = 0, y = 0, src = './img/monsterBlock.png'):
        i = random.randint(1, 3)
        src = './img/monster/monster' + str(i) + '.png'
        super().__init__(screen, hp, atk, x, y, src)
        self.__img = pg.image.load(src)
        
    @property    
    def img(self):
        return self.__img