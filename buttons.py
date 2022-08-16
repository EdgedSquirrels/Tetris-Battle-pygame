import const
import pygame as pg
from eventmanager import *

class BaseButton:
    """
    BaseButton with basic fuctionalities.
    """

    def __init__(self, evManager):
        self.evManager = evManager
        self.onClick = None
        self.width, self.height = 128, 37
        self.left, self.top = 168, 347
        self.border = 3
        self.color = (40, 140, 230)
        self.font = pg.font.Font("src/KRKZ.ttf", 20)
        self.isVisible = True
        self.isHover = False
        self.text = ["Button", "Button2", "Button3"]
        self.border_radius = 10
        self.textIndex = 0

    def get_rect(self):
        return pg.Rect(self.left, self.top, self.width, self.height)

    def click(self):
        if self.onClick != None:
            self.evManager.Post(self.onClick)
        elif type(self.text) == list:
            self.textIndex = (self.textIndex + 1) % len(self.text)

    def draw(self, screen):
        surface = pg.Surface((self.width, self.height), pg.SRCALPHA)
        surface.fill((0, 0, 0, 0))

        # render border
        pg.draw.rect(
            surface,
            (255, 255, 255),
            pg.Rect(0, 0, self.width, self.height),
            border_radius=self.border_radius,
        )

        # render button
        color = self.color
        if self.isHover:
            color = tuple(min(x + 50, 255) for x in color)  # make the button brighter
        pg.draw.rect(
            surface,
            color,
            pg.Rect(
                self.border,
                self.border,
                self.width - self.border * 2,
                self.height - self.border * 2,
            ),
            border_radius=self.border_radius - self.border,
        )

        # render message
        msg = self.text[self.textIndex] if type(self.text) == list else self.text
        surface.blit(
            self.font.render(msg, True, (255, 255, 255)),
            (self.border + self.border_radius, self.border * 2),
        )

        screen.blit(surface, (self.left, self.top))


class StartButton(BaseButton):
    def __init__(self, evManager):
        super().__init__(evManager)
        self.evManager = evManager
        self.onClick = StateChangeEvent(const.STATE_COUNTDOWN)
        self.text = "Start"
        self.width, self.height = (163, 63)
        self.left, self.top = (148, 268)
        self.border = 5
        self.color = (75, 200, 0)
        self.font = pg.font.Font("src/KRKZ.ttf", 38)
        self.border_radius = 15


class PlayerNumButton(BaseButton):
    def __init__(self, evManager):
        super().__init__(evManager)
        self.evManager = evManager
        self.text = ["Player: 1", "Player: 2"]

    def click(self):
        self.textIndex = (self.textIndex + 1) % len(self.text)
        self.evManager.Post(PlayerNumberChangeEvent(self.textIndex + 1))


class PlayerModeButton(BaseButton):
    def __init__(self, evManager, id=0, isVisible=False):
        super().__init__(evManager)
        self.id = id
        self.left, self.top = 152, 397 + id * 50
        self.width = 160
        self.text = ["P%d:keyboard" % (id + 1), "P%d:log" % (id + 1)]
        self.isVisible = isVisible

    def click(self):
        self.textIndex = (self.textIndex + 1) % len(self.text)