import pygame as pg
import model
from eventmanager import *
import const


class InputDevices(object):
    """
    Handles keyboard and mouse input.
    """

    def __init__(self, evManager, model: model.GameEngine):
        """
        evManager (EventManager): Allows posting messages to the event queue.
        model (GameEngine): a strong reference to the game Model.
        """
        self.evManager = evManager
        evManager.RegisterListener(self)
        self.model = model
        self.lastKey = None
        self.lastKeyDuration = 0

    def notify(self, event):
        """
        Receive events posted to the message queue.
        """

        if isinstance(event, TickEvent):
            # Called for each game tick. We check our keyboard presses here.
            for event in pg.event.get():
                # handle window manager closing our window
                if event.type == pg.QUIT:
                    self.evManager.Post(QuitEvent())
                if self.model.cur_state == const.STATE_PLAY:
                    # handle key down events
                    if event.type == pg.KEYDOWN:
                        if event.key == pg.K_ESCAPE:
                            self.evManager.Post(QuitEvent())
                        elif self.model.n_players == 1 and event.key in const.MOVEMENTS:
                            self.evManager.Post(
                                PlayerMoveEvent(*const.MOVEMENTS[event.key])
                            )
                        elif (
                            self.model.n_players == 2
                            and event.key in const.MOVEMENTS_2PLAYERS
                        ):
                            self.evManager.Post(
                                PlayerMoveEvent(*const.MOVEMENTS_2PLAYERS[event.key])
                            )
                        else:
                            # post any other keys to the message queue for everyone else to see
                            self.evManager.Post(InputEvent(event.unicode, None))
                if self.model.cur_state == const.STATE_MENU:
                    # handle mouse events
                    self.model.mouse_pos = pg.mouse.get_pos()
                    for button in self.model.buttons:
                        if button.isVisible and button.get_rect().collidepoint(
                            self.model.mouse_pos
                        ):
                            button.isHover = True
                        else:
                            button.isHover = False

                    if event.type == pg.MOUSEBUTTONDOWN:
                        for button in self.model.buttons:
                            if button.isVisible and button.get_rect().collidepoint(
                                event.pos
                            ):
                                button.click()

            if self.model.cur_state == const.STATE_PLAY:
                # Delay Auto Shift
                keys = pg.key.get_pressed()
                if self.lastKey != None and keys[self.lastKey]:
                    self.lastKeyDuration += 1 / const.FPS
                    if self.lastKeyDuration >= 0.15:
                        self.evManager.Post(
                            PlayerMoveEvent(*const.MOVEMENTS[self.lastKey])
                        )
                else:
                    self.lastKey = None
                    self.lastKeyDuration = 0
                    for key in [pg.K_LEFT, pg.K_RIGHT, pg.K_DOWN]:
                        if keys[key]:
                            # print(key)
                            self.lastKey = key
                            # print(key, self.lastKey)
