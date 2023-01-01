import GUI.gui as gui
import pygame as pg
import os
import sys
import drawMap
from pygame.locals import *
from Block import block
from Entity import entity
from Setting import setting
from Setting.setting import isNear, isMouseOnIt, printMsg, bgColor
import pygame

def displayLoby(screen):
    loby = gui.Loby(screen)
    gp = pygame.sprite.Group()
    gp.add(loby)
    gp.draw(screen)
    pg.display.update()
    run = True
    while run:
        for event in pg.event.get():
            if event.type == QUIT:
                exit()
        pos = pg.mouse.get_pos()
        pressed = pg.mouse.get_pressed()
        img = loby.image
        if pressed[0]:
            if pos[0] <= 776 and pos[0] >= 376:
                if pos[1] <= 600 and pos[1] >= 500:
                    run = False
                elif pos[1] <= 750 and pos[1] >= 650:
                    exit()
        else:
            pg.time.delay(80)
    gp.empty()
    
def gameOver(screen, win):
    final = gui.Final(screen, win)
    gp = pygame.sprite.Group()
    gp.add(final)
    gp.draw(screen)
    pg.display.update()
    run = True
    while run:
        for event in pg.event.get():
            if event.type == QUIT:
                exit()
        pos = pg.mouse.get_pos()
        pressed = pg.mouse.get_pressed()
        img = final.image
        if pressed[0]:
            if pos[0] <= 776 and pos[0] >= 376:
                if pos[1] <= 850 and pos[1] >= 750:
                    run = False
        else:
            pg.time.delay(80)
    gp.empty()
    
def inGame(screen):
    #init
    pg.init()
    pg.mixer.init()
    
    displayLoby(screen)
    
    sets = setting.Setting()
    tick = sets.tick
    bgColor = (100,100,100)
    run = True
    x = 0
    y = 0
    playerPanel = pg.Surface((192, 960))
    player = entity.Player(screen)
    screen.fill(bgColor)
    itemList = drawMap.drawMap(screen)
    font = pg.font.SysFont('fangsong', 60)
    bigHead = pg.image.load('./img/bigHead.png')
    bgmusic = pg.mixer.music.load('./music/bgMusic.mp3')
    pg.mixer.music.play(loops=20)
    pg.mixer.music.set_volume(0.4)
    
    while run:
        pg.time.delay(80) #save cup usitily
        
        
        
        pg.display.set_caption("KeyBroad Commander")
        nextBlockID = -1
        playerPanel.fill((100, 120, 150))
        infoHp = font.render('HP: ' + str(player.Hp), True, (255, 255, 255), (0,0,0))
        infoAtk = font.render('Atk: ' + str(player.Atk), True, (255, 255, 255), (0,0,0))
        playerPanel.blit(infoHp, (40, 150))
        playerPanel.blit(infoAtk, (40, 200))
        playerPanel.blit(bigHead, (40, 40))
        
        
        #Moving control
        nx = x
        ny = y
        for event in pg.event.get():
            if event.type == QUIT:
                exit()
            if event.type == pg.KEYDOWN:
                key_press = pg.key.get_pressed()
                pos = drawMap.findBlockByPosition(x, y)
                if key_press[K_w]:
                    if sets.isAllowed(pos[2]):
                        y -= tick
                    nextBlockID = pos[2]
                if key_press[K_s]:
                    if sets.isAllowed(pos[3]):
                        y += tick
                    nextBlockID = pos[3]
                if key_press[K_a]:
                    if sets.isAllowed(pos[0]):
                        x -= tick
                    nextBlockID = pos[0]
                if key_press[K_d]:
                    if sets.isAllowed(pos[1]):
                        x += tick
                    nextBlockID = pos[1]
        
        for i in itemList:
            if i.Id == 2:
                if isMouseOnIt(pg.mouse.get_pos(), i.x, i.y):
                    mHp = font.render('Hp: ' + str(i.body.Hp), True, (255, 255, 255), (0,0,0))
                    mAtk = font.render('Atk: ' + str(i.body.Atk), True, (255, 255, 255), (0,0,0))
                    playerPanel.blit(mHp, (40, 400))
                    playerPanel.blit(mAtk, (40, 450))
                    playerPanel.blit(i.body.img, (40, 300))
                if nextBlockID == 2 and isNear(nx, ny, i):
                    hit = pg.mixer.Sound('./music/punch.wav')
                    hit.play()
                    dead = i.toBattle(player, itemList)
                    if player.Hp <= 0:
                        run = False
                        pg.mixer.music.stop()
                        return False
                    if not dead:
                        x = nx
                        y = ny
            elif i.Id == 3 and nextBlockID == 3 and isNear(nx, ny, i):
                if i.playerPass(player, screen, i.is_Open, itemList):
                    m = pg.mixer.Sound('./music/door.wav')
                    m.play()
                    x = i.x
                    y = i.y
                else:
                    printMsg(playerPanel, screen, 'You Need A Key!', (255,255,255), (220,0,0))
            elif i.Id == 4:
                player.setPosition(x, y)
                i.is_player_got(player, i.Id)
            elif i.Id == 5 and nextBlockID == 3 and isNear(nx, ny, i):
                x = i.x
                y = i.y
            elif i.Id == 6:
                i.is_player_got(player, i.Id, playerPanel)
        player.setPosition(x, y)
        player.blitme((x,y), (nx, ny))
        
        for i in itemList:
            if i.Id == 5:
                i.blitme((i.x, i.y))
            if i.Id == 7:
                if x == i.x and y == i.y:
                    run = False
                    pg.mixer.music.stop()
                    return True
                
        screen.blit(playerPanel, (961, 0))
        pg.display.update()