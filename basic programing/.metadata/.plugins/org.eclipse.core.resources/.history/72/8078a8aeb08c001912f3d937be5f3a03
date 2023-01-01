import pygame as pg
import Entity.entity as entity
import Setting.setting as Setting
import random

class Wall():
    def __init__(self, screen, src = './img/wall.png'):
        self.is_solid = True
        self.__screen = screen
        self.__img = pg.image.load(src)
        self.__id = 1
    
    def blitme(self, coordinate=(0,0)):
        self.__screen.blit(self.__img, coordinate)
        
    def is_Solid(self):
        return self.is_solid
    
    @property
    def Id(self):
        return self.__id
    
    @property
    def Screen(self):
        return self.__screen
    
class MonsterBlock(Wall):
    def __init__(self, screen, x, y, src = './img/monsterBlock.png'):
        super().__init__(screen, src)
        self.is_solid = False
        self.__id = 2
        hp = random.randint(1, 10)
        atk = random.randint(1, 5)
        self.__body = entity.Monster(screen, hp, atk)
        self.__x = x
        self.__y = y
        self.__dead = False
        
    @property
    def dead(self):
        return self.__dead
    
    def setDead(self, value):
        self.__dead = value
        
    @property
    def x(self):
        return self.__x
    
    @property
    def y(self):
        return self.__y    
    
    @property
    def Id(self):
        return self.__id
    
    @property
    def body(self):
        return self.__body

    def toBattle(self, player, ml):
        if not self.dead:
            hp = self.body.Hp
            atk = self.body.Atk
            self.body.setHp(hp - player.Atk)
            player.setHp(player.Hp - atk)
            ahp = self.body.Hp
            if ahp <= 0:
                self.setDead(True)
                ml.remove(self)
                surf = pg.Surface((48,48))
                surf.fill((100,100,100))
                self.Screen.blit(surf, (self.x, self.y))
        return self.dead
    
class Exit(Wall):
    def __init__(self, screen, x, y, src = './img/exit.png'):
        super().__init__(screen, src)
        self.__id = 7
        self.__x = x
        self.__y = y

    @property
    def x(self):
        return self.__x
    
    @property
    def y(self):
        return self.__y    
    
    @property
    def Id(self):
        return self.__id
    
    def escape(self):
        print('Escape!')
    
class Item(Wall):
    def __init__(self, screen, x, y, src):
        super().__init__(screen, src)
        self.__x = x
        self.__y = y
        self.__screen = screen
        self.__isAvalible = True
        
    def is_player_got(self, player, id, playerPanel = None):
        if(player.x == self.x and player.y == self.y) and self.__isAvalible:
            player.addItem(self)
            surf = pg.Surface((48,48))
            surf.fill((100,100,100))
            self.__screen.blit(surf, (self.x, self.y))
            self.__screen.blit(player.img, (self.x, self.y))
            pg.display.update()
            self.__isAvalible = False
            if id == 4:
                pg.mixer.Sound('./music/key.wav').play()
            if id == 6:
                pg.mixer.Sound('./music/cast.wav').play()
                Cast.randEffect(self, player, playerPanel, self.__screen)
            #print('key')
    
    def setPosition(self, x, y):
        self.__x = x
        self.__y = y
    
    @property
    def x(self):
        return self.__x
    
    @property
    def y(self):
        return self.__y

class Door_open(Item):
    def __init__(self, screen, x, y, src = './img/door_open.png'):
        super().__init__(screen, x, y, src)
        self.__id = 5
        self.__screen = screen
        
    @property
    def Id(self):
        return self.__id

class Door(Item):
    def __init__(self, screen, x, y, src = './img/door_lock.png'):
        super().__init__(screen, x, y, src)
        self.is_solid = True
        self.__is_open = 0
        self.__id = 3
        self.__img = pg.image.load(src)
        self.__screen = screen
    
    @property
    def Id(self):
        return self.__id
    
    def setSrc(self, src):
        self.__img = pg.image.load(src)
    
    @property
    def is_Open(self):
        return self.__is_open
            
    def setOpen(self):
        self.__is_open = 1
        
    def playerPass(self, player, screen, is_open, list):
        hasKey = False
        for i in player.Inventory():
            if i.Id == 4:
                self.setOpen()
                hasKey = True
                player.Inventory().remove(i)
                break
        if is_open == 0:
            if hasKey:
                #self.is_Open(True)
                self.blitme((self.x, self.y))
                od = Door_open(screen, self.x, self.y)
                list.remove(self)
                list.append(od)
            else:
                print('key')
                
        if is_open == 1:
            return True
        return hasKey
    
    def blitme(self, coordinate=(0,0)):
        self.__screen.blit(self.__img, coordinate)



class Key(Item):
    def __init__(self, screen, x = 0, y = 0, src = './img/key.png',):
        super().__init__(screen, x, y, src)
        self.is_solid = False
        self.__id = 4
        self.__isAvalible = True
        
    @property
    def Id(self):
        return self.__id
class Cast(Item):
    def __init__(self, screen, x = 0, y = 0, src = './img/cast.png'):
        super().__init__(screen, x, y, src)
        self.is_solid = False
        self.__id = 6
        
    @property
    def Id(self):
        return self.__id
    
    def randEffect(self, player, playerPanel, screen):
        i = random.randint(1,2)
        if i == 1:
            a = random.randint(1, 8)
            player.setHp(player.Hp + a)
            Setting.printMsg(playerPanel, screen, 'HP UPUP! -- '  + str(a), (255,255,255), (0,220,100))
        elif i == 2:
            a = 1
            player.setAtk(player.Atk + a)
            Setting.printMsg(playerPanel, screen, 'ATK UPUP! -- ' + str(a), (255,255,255), (0,0,220))
            