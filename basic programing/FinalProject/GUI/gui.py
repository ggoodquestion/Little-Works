import pygame

class Loby(pygame.sprite.Sprite):
    src = './img/title.png'
    bg = './img/bg.jpg'
    def __init__(self, screen):
        font = pygame.font.SysFont('fangsong', 40)
        txtStart = font.render('START', True, (100,0,0), (220,220,220))
        txtQuit = font.render('QUIT', True, (100,0,0), (220,220,220))
        pygame.sprite.Sprite.__init__(self)
        start = pygame.Surface((400, 100))
        start.fill((220,220,220))
        start.blit(txtStart, (160, 40))
        quit = pygame.Surface((400, 100))
        quit.fill((220,220,220))
        quit.blit(txtQuit, (170, 40))
        title = pygame.image.load(self.src)
        bg = pygame.image.load(self.bg)
        self.image = pygame.Surface(screen.get_size())
        self.image.blit(bg, (0,0))
        self.image.blit(start, (376, 500))
        self.image.blit(quit, (376, 650))
        self.image.blit(title, (176, 100))
        self.rect = self.image.get_rect()
        
class Final(pygame.sprite.Sprite):
    src = './img/gameover.png'
    bg = './img/death.png'
    def __init__(self, screen, win):
        pygame.sprite.Sprite.__init__(self)
        if win:
            self.src = './img/win.png'
            self.bg = './img/escape.png'
        font = pygame.font.SysFont('fangsong', 40)
        txtHome = font.render('HOME', True, (100,0,0), (220,220,220))
        home = pygame.Surface((400, 100))
        home.fill((220,220,220))
        home.blit(txtHome, (170, 40))
        f = pygame.image.load(self.src)
        self.image = pygame.Surface(screen.get_size())
        bg = pygame.image.load(self.bg)
        self.image.blit(bg, (0,0))
        self.image.blit(home, (376, 750))
        self.image.blit(f, (326,100))
        self.rect = self.image.get_rect()
            