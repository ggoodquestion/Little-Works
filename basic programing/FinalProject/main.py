#-*- coding: cp950 -*-
import GUI.GUIdisplay as gd
import pygame as pg

screen = pg.display.set_mode(size=(960 + 192, 960))
execute = True
while execute:
    pg.init()
    pg.mixer.init()
    gd.displayLoby(screen)
    win = gd.inGame(screen)
    gd.gameOver(screen, win)
