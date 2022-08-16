import const
import pygame as pg
from random import shuffle, randint
from eventmanager import *
from logger import *


class Player:
    """
    Player info and methods.
    """

    def __init__(self, id, evManager, n_players=1, mode=const.MODE_KEYBOARD):
        self.evManager = evManager
        self.id = id
        self.n_players = n_players
        self.opp_id = (id + 1) % n_players
        self.mode = mode

        # tetrimono properties
        self.board = [
            [0 for _ in range(const.BOARD_WIDTH)] for row in range(const.BOARD_HEIGHT)
        ]

        # garbage line properties
        self.garbages = []  # from top to bottom
        # separate n_garbage into 'from' and 'to' for rendering
        self.n_garbages_from = 0
        self.n_garbages_to = 0

        # hold properties
        self.hold_piece = 0
        self.has_hold = 0

        self.logger = Logger(self)
        if self.mode == const.MODE_KEYBOARD:
            self.initBag()  # init the tetrimino bag

        # scoring properites
        self.is_last_move_rotate = False
        self.combo = -1
        self.score = 0
        self.B2B = 0
        self.KO = 0
        self.KOed = 0

        self.tactic = ""
        self.tactic_Tspin = False

        # timer properties
        self.game_timer = None
        self.gravity_timer = pg.time.get_ticks()
        self.combo_timer = None
        self.garbage_timer = None
        self.KOed_timer = None
        self.tactic_timer = None

    @property
    def TIME_CLK(self):
        return pg.time.get_ticks() - self.game_timer - 1000 * self.KOed

    def initBag(self, tetriminos=None):
        if tetriminos == None:
            tetriminos = [x for x in range(1, len(const.TETRIMINO_CODES))]
            shuffle(tetriminos)
            if self.mode == const.MODE_KEYBOARD:
                self.logger.writeLog(" ".join(map(str, tetriminos[:6])))
        self.current_position = [const.START_LOCATION[0] - 1, const.START_LOCATION[1]]
        self.current_rotation = 0
        self.current_piece = tetriminos[0]
        self.next_pieces = tetriminos[1:6]
        self.bag = tetriminos[6:]
        self._updateGhostPiece()

    def isCollision(self, ypos, xpos, rotation=None):
        """
        Check whether the current piece collide with the board.
        """
        if rotation == None:
            rotation = self.current_rotation
        for y, x in const.TETRIMINOS[self.current_piece][rotation]:
            y += ypos
            x += xpos
            if x < 0 or y < 0 or x >= const.BOARD_WIDTH or self.board[y][x] != 0:
                return True
        return False

    def _updateGhostPiece(self):
        """
        Update the ghost piece position.
        """
        ypos, xpos = self.current_position
        while not self.isCollision(ypos, xpos):
            ypos -= 1
        ypos += 1
        self.ghost_position = [ypos, xpos]

    def initCurrentPiece(self, center_x=None, center_y=None):
        y, x = const.START_LOCATION
        self.current_rotation = 0
        garbage_height = len(self.garbages)
        if not self.isCollision(y - 1 - garbage_height, x):
            y = y - 1 - garbage_height
        elif not self.isCollision(y - garbage_height, x):
            y = y - garbage_height
        elif garbage_height > 0:
            self.KOed += 1
            self.evManager.Post(KOEvent(self.id, self.opp_id))
            return
        else:
            self.evManager.Post(StateChangeEvent(const.STATE_ENDGAME, loser=self.id))

        if center_x != None and center_y != None:
            x, y = center_x, center_y

        self.logger.writeLog("%d i %d %d" % (self.TIME_CLK, x, y))
        self.current_position = [y, x]
        self._updateGhostPiece()
        self.is_last_move_rotate = False
        self.gravity_timer = pg.time.get_ticks()

    def _nextPiece(self):
        """
        Handle the situation of getting new piece.
        """
        self.current_piece = self.next_pieces.pop(0)
        self.next_pieces.append(self.bag.pop(0))
        self.is_last_move_rotate = False
        if self.mode == const.MODE_KEYBOARD and len(self.bag) == 0:
            self.bag = [x for x in range(1, len(const.TETRIMINO_CODES))]
            shuffle(self.bag)
        self.initCurrentPiece()

    def _clearLines(self):
        """
        Clear lines on board.

        @return:
        num: Number of lines cleared.
        isClear: Whether the board is totally cleared.
        """
        num, board, isClear = 0, [], True
        for row in self.board:
            for x in row:
                if x == 0:
                    board.append(row)
                    if isClear and sum(row) > 0:
                        isClear = False
                    break
            else:
                num += 1
        for _ in range(num):
            board.append([0 for _ in range(const.BOARD_WIDTH)])
        self.board = board
        return num, isClear

    def _addGarbage(self, n_garbage):
        for _ in range(n_garbage):
            if self.garbages == []:
                x = randint(0, const.BOARD_WIDTH - 1)
            else:
                x = randint(0, const.BOARD_WIDTH - 2)
                if x >= self.garbages[-1]:
                    x += 1
            self.garbages.append(x)
        self.n_garbages_from = self.n_garbages_to
        self.n_garbages_to = 0
        self.garbage_timer = pg.time.get_ticks()

    def receiveGarbage(self, n_garbage):
        self.garbage_timer = pg.time.get_ticks()
        # print("receive!!", self.garbage_timer, pg.time.get_ticks())
        self.n_garbages_from = self.n_garbages_to
        self.n_garbages_to += n_garbage

    def _getTspin_Type(self):
        ypos, xpos = self.ghost_position
        n_occupy, Tspin_type = 0, 0
        if (
            self.is_last_move_rotate
            and const.TETRIMINO_CODES[self.current_piece] == "T"
        ):
            not_fill = (0, 0)  # record which block is not filled
            for dx, dy in ((-1, -1), (-1, 1), (1, -1), (1, 1)):
                x, y = xpos + dx, ypos + dy
                if x < 0 or x >= const.BOARD_WIDTH or y < 0 or self.board[y][x]:
                    n_occupy += 1
                else:
                    not_fill = (dx, dy)
            if n_occupy >= 3:
                Tspin_type = 2
                if (
                    not_fill == (-1, -1)
                    and self.current_rotation in [2, 3]
                    or not_fill == (-1, 1)
                    and self.current_rotation in [3, 0]
                    or not_fill == (1, 1)
                    and self.current_rotation in [0, 1]
                    or not_fill == (1, -1)
                    and self.current_rotation in [1, 2]
                ):
                    Tspin_type = 1
        return Tspin_type

    def drop(self, addToBag=[]):
        """
        Hard-Drop the current falling tetrimino.
        @ addToBag: For player log, append the tetrimino to self.bag
        """
        if self.id == 0:
            self.evManager.Post(PlaySoundEvent("drop"))

        if self.ghost_position != self.current_position:
            self.is_last_move_rotate = False

        ypos, xpos = self.ghost_position

        self.bag.extend(addToBag)
        self.logger.writeLog("%d d %d" % (self.TIME_CLK, self.bag[0]))

        has_clear_garbage = False

        for y, x in const.TETRIMINOS[self.current_piece][self.current_rotation]:
            y += ypos
            x += xpos
            self.board[y][x] = self.current_piece
            if not has_clear_garbage and y == 0:
                if len(self.garbages) > 0 and x == self.garbages[0]:
                    has_clear_garbage = True
                    self.garbages.pop(0)

        # handle T-spin
        Tspin_type = self._getTspin_Type()
        lines, isClear = self._clearLines()

        if self.id == 0 and lines:
            self.evManager.Post(PlaySoundEvent("clear"))

        # Update combo
        if lines or has_clear_garbage:
            self.combo += 1
        else:
            self.combo = -1
        self.combo_timer = pg.time.get_ticks() if self.combo > 0 else None

        if self.id == 0 and self.combo > 0:
            self.evManager.Post(PlaySoundEvent("combo%d" % min(7, self.combo)))

        # Update score
        score = 0
        for t in [1, 3, 5, 7]:
            if self.combo >= t:
                score += 1  # Combo
        if isClear and len(self.garbages) == 0:
            score += 10  # Perfect Clear
        if lines == 4:
            score += 6 if self.B2B else 4
        elif Tspin_type:
            score += lines * (3 if self.B2B else 2)
            if Tspin_type == 1 and lines == 1:
                score -= 1  # Tspin-mini
        elif lines > 0:
            score += lines - 1

        # Update tactic
        if lines or Tspin_type:
            self.tactic_timer = pg.time.get_ticks()
            if lines:
                self.tactic = ["", "SINGLE", "DOUBLE", "TRIPLE", "TETRIS"][lines]
            if isClear:
                self.tactic = "Perfect Clear"
            self.tactic_Tspin = Tspin_type > 0
            if Tspin_type == 1 and lines == 1:
                self.tactic = "MINI"

        # Update scoring properties
        self.B2B = True if lines == 4 or Tspin_type else False
        self.score += score

        # handle garbage lines
        if self.n_players > 1:
            score = score - self.n_garbages_to
            self._addGarbage(-score)
            if score > 0:
                self.evManager.Post(SendGarbageEvent(self.id, self.opp_id, score))

        self._nextPiece()
        self.has_hold = 0

    def hold(self, addToBag=[]):
        """
        Hold the current tetrimino
        @ addToBag: For player log, append the tetrimino to self.bag
        """
        if self.id == 0:
            self.evManager.Post(PlaySoundEvent("hold"))
        if self.has_hold:
            return

        self.bag.extend(addToBag)
        if self.hold_piece == 0:
            self.hold_piece = self.current_piece
            self.logger.writeLog("%d h %d" % (self.TIME_CLK, self.bag[0]))
            self._nextPiece()
        else:
            self.hold_piece, self.current_piece = self.current_piece, self.hold_piece
            self.logger.writeLog("%d h" % (self.TIME_CLK))
            self.initCurrentPiece()
        self.has_hold = 1

    def move(
        self,
        dy,
        dx,
        isGravity=False,
    ):
        y, x = self.current_position
        if dy != 0:
            self.gravity_timer = pg.time.get_ticks()
        if not self.isCollision(y + dy, x + dx):
            self.current_position = [y + dy, x + dx]
            self.is_last_move_rotate = False
            self.logger.writeLog("%d m %d %d" % (self.TIME_CLK, dx, dy))
            if dx != 0:
                self._updateGhostPiece()
        elif isGravity:
            self.drop()

    def rotate(self, isClockwise=True):
        rotate_dir = 0 if isClockwise else 1
        rotation = (
            (self.current_rotation + 1) % 4
            if isClockwise
            else (self.current_rotation + 3) % 4
        )

        y0, x0 = self.current_position
        for y1, x1 in (
            const.WALL_KICK_TABLE_I[rotate_dir][rotation]
            if self.current_piece == const.TETRIMINO_I
            else const.WALL_KICK_TABLE_OTHERS[rotate_dir][rotation]
        ):
            y = y0 + y1
            x = x0 + x1
            if not self.isCollision(y, x, rotation):
                self.logger.writeLog("%d r %d" % (self.TIME_CLK, isClockwise))
                self.current_position = [y, x]
                self.current_rotation = rotation
                self.is_last_move_rotate = True
                self._updateGhostPiece()
                return
