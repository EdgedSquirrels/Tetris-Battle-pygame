import const
import pygame as pg
from eventmanager import *
from components.buttons import *
from components.player import *


class GameEngine(object):
    """
    Tracks the game state.
    """

    def __init__(self, evManager):
        """
        evManager (EventManager): Allows posting messages to the event queue.

        Attributes:
        running (bool): True while the engine is online. Changed via QuitEvent().
        """

        self.evManager = evManager
        evManager.RegisterListener(self)
        self.n_players = 1
        self.players = []
        self.buttons = []
        self.running = False
        self.cur_state = const.STATE_MENU
        self.winner_id = None
        self.loser_id = None
        self.mouse_pos = None
        self.player_mode = "keyboard"
        self.game_timer = pg.time.get_ticks()

    def notify(self, event):
        """
        Called by an event in the message queue.
        """
        if isinstance(event, QuitEvent):
            self.running = False

        elif isinstance(event, PlayerNumberChangeEvent):
            self.n_players = event.n_players
            for button in self.buttons:
                if isinstance(button, PlayerModeButton):
                    button.isVisible = True if self.n_players > button.id else False

        elif isinstance(event, StateChangeEvent):
            if event.cur_event == const.STATE_ENDGAME:
                self.winner_id = event.winner
                self.loser_id = event.loser
            self._handleStateChangeEvent(event.cur_event)

        elif isinstance(event, PlayerMoveEvent):
            if event.id >= self.n_players:
                return
            player = self.players[event.id]
            if player.KOed_timer != None or player.mode != const.MODE_KEYBOARD:
                # In the state of KOed
                return
            if event.action == "Rotate_Clockwise":
                player.rotate()
            elif event.action == "Rotate_CounterClockwise":
                player.rotate(False)
            elif event.action == "Drop":
                player.drop()
            elif event.action == "Hold":
                player.hold()
            elif event.action == "Left":
                player.move(0, -1)
            elif event.action == "Right":
                player.move(0, 1)
            elif event.action == "Down":
                player.move(-1, 0)

        elif isinstance(event, TickEvent):
            for player in self.players:
                # handle KOed
                if (
                    player.KOed_timer != None
                    and pg.time.get_ticks() - player.KOed_timer >= 1000
                ):
                    # resume
                    player.KOed_timer = None
                    player.garbages = []
                    player.initCurrentPiece()
                if self.cur_state == const.STATE_PLAY and player.KOed_timer == None:
                    # handle gravity fall
                    if (
                        player.mode == const.MODE_KEYBOARD
                        and pg.time.get_ticks() - player.gravity_timer >= 1000
                    ):
                        player.move(-1, 0, isGravity=True)
                    # handle logger
                    player.logger.tickLog()

            # proceed to next state if necessary
            if self.cur_state == const.STATE_MENU:
                pass
            elif (
                self.cur_state == const.STATE_COUNTDOWN
                and pg.time.get_ticks() - self.game_timer
                >= const.COUNTDOWN_LENGTH * 1000
            ):
                self.evManager.Post(StateChangeEvent(const.STATE_PLAY))
                pass
            elif (
                self.cur_state == const.STATE_PLAY
                and pg.time.get_ticks() - self.game_timer >= const.GAME_LENGTH * 1000
            ):
                self.evManager.Post(StateChangeEvent(const.STATE_TIMEUP))
                pass
            elif (
                self.cur_state == const.STATE_TIMEUP
                and pg.time.get_ticks() - self.game_timer >= const.TIMEUP_LENGTH * 1000
            ):
                self.evManager.Post(StateChangeEvent(const.STATE_ENDGAME))
                pass
            elif (
                self.cur_state == const.STATE_ENDGAME
                and pg.time.get_ticks() - self.game_timer >= const.ENDGAME_LENGTH * 1000
            ):
                self.evManager.Post(StateChangeEvent(const.STATE_MENU))
                pass

        elif isinstance(event, KOEvent):
            KOed_id, KOget_id = event.KOed_id, event.KOget_id
            self.players[KOget_id].KO += 1
            if self.players[KOget_id].KO == const.MAX_KO:
                self.evManager.Post(StateChangeEvent(const.STATE_ENDGAME, winner=KOget_id))
            elif KOed_id == 0:
                self.evManager.Post(PlaySoundEvent("KOed"))
            elif KOget_id == 0:
                self.evManager.Post(PlaySoundEvent("KOget"))

        elif isinstance(event, SendGarbageEvent):
            self.players[event.to_id].receiveGarbage(event.n_garbage)

    def _handleStateChangeEvent(self, cur_state):
        if cur_state == const.STATE_MENU:
            for button in self.buttons:
                button.isHover = False

        elif cur_state == const.STATE_COUNTDOWN:
            self.evManager.Post(PlaySoundEvent("countDown"))
            modes = [0, 0]
            for button in self.buttons:
                if isinstance(button, PlayerNumButton):
                    self.n_players = button.textIndex + 1
                elif isinstance(button, PlayerModeButton):
                    modes[button.id] = button.textIndex

            self.players = [
                Player(i, self.evManager, self.n_players, mode=modes[i])
                for i in range(self.n_players)
            ]

        elif cur_state == const.STATE_PLAY:
            self.evManager.Post(PlaySoundEvent("main"))
            for player in self.players:
                player.game_timer = pg.time.get_ticks()

        elif cur_state == const.STATE_TIMEUP:
            self.evManager.Post(PlaySoundEvent("timeup"))

        elif cur_state == const.STATE_ENDGAME:
            # decide the winner and loser
            if self.winner_id != None:
                self.loser_id = (
                    None if self.n_players != 2 else self.n_players - 1 - self.winner_id
                )
            elif self.loser_id != None:
                self.winner_id = (
                    None if self.n_players != 2 else self.n_players - 1 - self.loser_id
                )
            elif self.n_players == 2:
                pt = [self.players[0].KO, self.players[1].KO]
                if pt[0] == pt[1]:
                    pt = [self.players[0].score, self.players[1].score]
                    if pt[0] == pt[1]:
                        for id, player in enumerate(self.players):
                            pt[id] = -len(player.garbages)
                            board = player.board
                            for y in range(const.BOARD_HEIGHT):
                                if sum(board[y]) > 0:
                                    pt[id] = -y - 1 - len(player.garbages)
                if pt[0] >= pt[1]:
                    self.winner_id, self.loser_id = 0, 1
                else:
                    self.winner_id, self.loser_id = 1, 0
            else:
                self.winner_id = 0
            self.evManager.Post(
                PlaySoundEvent("win" if self.winner_id == 0 else "lose")
            )

        self.game_timer = pg.time.get_ticks()
        self.cur_state = cur_state

    def run(self):
        """
        Starts the game engine loop.

        This pumps a Tick event into the message queue for each loop.
        The loop ends when this object hears a QuitEvent in notify().
        """
        self.running = True
        self.evManager.Post(InitializeEvent())
        self.buttons = [
            StartButton(self.evManager),
            PlayerNumButton(self.evManager),
            PlayerModeButton(self.evManager, 0, True),
            PlayerModeButton(self.evManager, 1, False),
        ]
        # self.clock = pg.time.Clock()
        while self.running:
            # self.clock.tick(const.FPS)
            newTick = TickEvent()
            self.evManager.Post(newTick)
