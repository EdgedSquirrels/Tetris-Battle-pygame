from turtle import clear
import pygame as pg
import model
import const
from eventmanager import *

WINDOW_SIZE = (955, 675)  # (width, height)

# images
tilesImg = [0 for _ in range(len(const.TILE_CODES))]
tetriminoImg = [0 for _ in range(len(const.TETRIMINO_CODES))]
numImg = [0 for _ in range(10)]
comboImg = [0 for _ in range(const.MAX_COMBO + 1)]
KOImg = [0 for _ in range(const.MAX_KO + 1)]
KOedImg = None
mainImg = None
winImg = None
loseImg = None
timeupImg = None
startImg = None
countDownImg = [0 for _ in range(int(const.COUNTDOWN_LENGTH * 10 + 1))]

# sounds
mainMusic = None
gameSounds = {}


class GraphicalView(object):
    """
    Draws the model state onto the screen and handle music and sound.
    """

    def __init__(self, evManager, model: model.GameEngine):
        """
        evManager (EventManager): Allows posting messages to the event queue.
        model (GameEngine): a strong reference to the game Model.
        """

        self.evManager = evManager
        evManager.RegisterListener(self)
        self.model = model
        self.isinitialized = False
        self.screen = None
        self.clock = None
        self.smallfont = None

    def notify(self, event):
        """
        Receive events posted to the message queue.
        """

        if isinstance(event, InitializeEvent):
            self.initialize()

        elif isinstance(event, QuitEvent):
            # shut down the pygame graphics
            self.isinitialized = False
            pg.quit()

        elif isinstance(event, PlaySoundEvent):
            if event.name == "main":
                pg.mixer.music.play()
            elif event.name in gameSounds.keys():
                gameSounds[event.name].play()

        elif isinstance(event, StateChangeEvent):
            if event.cur_event == const.STATE_ENDGAME:
                pg.mixer.music.stop()

        elif isinstance(event, TickEvent):
            self.renderall()
            # limit the redraw speed to FPS
            self.clock.tick(const.FPS)
            pg.display.set_caption("demo game FPS: %.2f" % self.clock.get_fps())

    def renderall(self):
        """
        Draw the current game state on screen.
        Does nothing if isinitialized == False (pg.init failed)
        """

        if not self.isinitialized:
            return
        # clear display
        self.screen.fill((0, 0, 0))
        self.screen.blit(mainImg, (0, 0))

        cur_state = self.model.cur_state

        if cur_state in [const.STATE_TIMEUP, const.STATE_ENDGAME, const.STATE_PLAY]:
            self._renderBoard()
            self._renderOtherPieces()
            self._renderScore()
            self._renderKO()
            self._renderGarbageBar()
            self._renderTactic()

        if cur_state == const.STATE_MENU:
            # draw buttons
            self._renderButtons()

        if cur_state == const.STATE_COUNTDOWN:
            # draw countdown
            idx = min(
                int(const.COUNTDOWN_LENGTH * 10 - 1),
                (pg.time.get_ticks() - self.model.game_timer) // 100,
            )
            self.screen.blit(countDownImg[idx], (0, 113))

        # render time
        self._renderTime()

        # flip the display to show whatever we drew
        pg.display.flip()

    def _renderBoard(self):
        for id, player in enumerate(self.model.players):
            board = player.board

            surface = pg.Surface((23 * const.BOARD_WIDTH, 23 * 20 + 3), pg.SRCALPHA)
            surface.fill((255, 255, 255, 0))

            # render Garbage Lines
            for i, _x in enumerate(player.garbages):
                y = len(player.garbages) - i - 1
                if y >= const.BOARD_HEIGHT_VISIBLE:
                    continue
                for x in range(const.BOARD_WIDTH):
                    surface.blit(
                        tilesImg[len(const.TETRIMINO_CODES) + 1 + (x == _x)],
                        (23 * x, (19 - y) * 23 + 3),
                    )

            # render Ghost Piece
            piece, rotation = player.current_piece, player.current_rotation
            ypos, xpos = player.ghost_position
            for y, x in const.TETRIMINOS[piece][rotation]:
                y += ypos
                x += xpos
                surface.blit(
                    tilesImg[len(const.TETRIMINO_CODES)],
                    (23 * x, (19 - (y + len(player.garbages))) * 23 + 3),
                )

            # render Current Piece
            ypos, xpos = player.current_position
            for y, x in const.TETRIMINOS[piece][rotation]:
                y += ypos
                x += xpos
                surface.blit(
                    tilesImg[piece],
                    (23 * x, (19 - (y + len(player.garbages))) * 23 + 3),
                )
            
            # render Board
            for y in range(const.BOARD_HEIGHT_VISIBLE):
                for x in range(const.BOARD_WIDTH):
                    if board[y][x]:
                        surface.blit(
                            tilesImg[board[y][x]],
                            (23 * x, (19 - (y + len(player.garbages))) * 23 + 3),
                        )

            # render Combo
            if player.combo_timer != None:
                prog = min(1, (pg.time.get_ticks() - player.combo_timer) / 1000)
                if prog == 1:
                    player.combo_timer = None
                else:
                    img = comboImg[player.combo].copy()
                    img.set_alpha(int(255 * (1 - prog)))
                    surface.blit(img, (46, 161))

            # render board with dark mask effect, e.g. KOed, timeup, endgame
            cur_state = self.model.cur_state
            if player.KOed_timer != None or cur_state in [
                const.STATE_TIMEUP,
                const.STATE_ENDGAME,
            ]:
                surface2 = pg.Surface((23 * const.BOARD_WIDTH, 463), pg.SRCALPHA)
                surface2.fill((0, 0, 0, 128))
                if cur_state == const.STATE_TIMEUP:
                    surface2.blit(timeupImg, (0, 210))
                elif cur_state == const.STATE_ENDGAME:
                    if id == self.model.winner_id:
                        surface2.blit(winImg, (0, 210))
                    elif id == self.model.loser_id:
                        surface2.blit(loseImg, (0, 210))
                elif player.KOed_timer != None:
                    surface2.blit(KOedImg, (46, 207))
                surface.blit(surface2, (0, 0))

            self.screen.blit(surface, (114 + 484 * id, 140))

    def _renderOtherPieces(self):
        """
        Render hold piece and next pieces
        """
        for id, player in enumerate(self.model.players):
            # render hold
            tetrimino_code = player.hold_piece
            if tetrimino_code:
                width, height = const.TETRIMINO_SIZES[tetrimino_code]
                img = pg.transform.scale(
                    tetriminoImg[tetrimino_code],
                    (int(width * 23 * 0.8), int(height * 23 * 0.8)),
                )
                width, height = img.get_size()
                if player.has_hold:
                    img.set_alpha(70)  # Make it darker
                self.screen.blit(img, (62 - width // 2 + 484 * id, 203 - height // 2))

            # render next
            NEXT_POSITION = (
                [406, 203],
                [406, 288],
                [406, 357],
                [406, 427],
                [406, 487],
            )  # align: center
            NEXT_RESIZE = [0.8, 0.75, 0.6, 0.6, 0.6]
            for i, tetrimino_code in enumerate(player.next_pieces):
                width, height = const.TETRIMINO_SIZES[tetrimino_code]
                img = pg.transform.scale(
                    tetriminoImg[tetrimino_code],
                    (
                        int(width * 23 * NEXT_RESIZE[i]),
                        int(height * 23 * NEXT_RESIZE[i]),
                    ),
                )
                width, height = img.get_size()
                self.screen.blit(
                    img,
                    (
                        NEXT_POSITION[i][0] - width // 2 + 484 * id,
                        NEXT_POSITION[i][1] - height // 2,
                    ),
                )

    def _renderScore(self):
        """
        Render linesents.
        """
        for id, player in enumerate(self.model.players):
            score = str(player.score)
            surface = pg.Surface((34 * len(score), 38), pg.SRCALPHA)
            surface.fill((255, 255, 255, 0))
            for i, d in enumerate(score):
                surface.blit(numImg[int(d)], (34 * i, 0))
            width, height = surface.get_size()
            width, height = int(0.7 * width), int(0.7 * height)
            surface = pg.transform.scale(surface, (width, height))
            self.screen.blit(surface, (65 + id * 484 - width // 2, 413 - height // 2))

    def _renderKO(self):
        """
        Render KO gets.
        """
        for id, player in enumerate(self.model.players):
            if player.KO > 0:
                self.screen.blit(KOImg[player.KO], (32 + 484 * id, 262))

    def _renderTactic(self):
        """
        Render tactic used.
        """
        for id, player in enumerate(self.model.players):
            if player.tactic_timer != None:
                if pg.time.get_ticks() - player.tactic_timer >= 1000:
                    player.tactic_timer = None
                else:
                    color = (128, 0, 255) if player.tactic_Tspin else (0, 128, 255)
                    self.screen.blit(
                        self.smallfont.render(player.tactic, True, color),
                        (378 + 484 * id, 585),
                    )
                    if player.tactic_Tspin:
                        self.screen.blit(
                            self.smallfont.render("T-spin", True, color),
                            (378 + 484 * id, 567),
                        )

    def _renderTime(self):
        time = const.GAME_LENGTH
        cur_state = self.model.cur_state
        if cur_state in [const.STATE_TIMEUP, const.STATE_ENDGAME]:
            time = 0
        if cur_state == const.STATE_PLAY:
            time = max(
                0,
                const.GAME_LENGTH
                - (pg.time.get_ticks() - self.model.game_timer) // 1000,
            )

        minute, second = time // 60, time % 60
        surface = pg.Surface((398, 57), pg.SRCALPHA)
        surface.fill((0, 0, 0, 0))

        # Render time bar
        pg.draw.rect(surface, (86, 87, 82), pg.Rect(17, 0, 126, 5))  # gray
        pg.draw.rect(
            surface,
            (229, 150, 26),
            pg.Rect(17, 0, int(126 * time / const.GAME_LENGTH), 5),
        )  # yellow

        # Render digits
        surface.blit(numImg[minute // 10], (0, 12))
        surface.blit(numImg[minute % 10], (34, 12))
        surface.blit(numImg[second // 10], (86, 12))
        surface.blit(numImg[second % 10], (120, 12))

        self.screen.blit(surface, (398, 20))

    def _renderGarbageBar(self):
        for id, player in enumerate(self.model.players):
            if player.garbage_timer == None:
                continue
            prog = min(1.0, (pg.time.get_ticks() - player.garbage_timer) / 1000)
            # print('prog', prog)
            if prog == 1 and player.n_garbages_to == 0:
                player.garbage_timer = None
            else:
                from_height, to_height = (
                    min(player.n_garbages_from * 23, 463),
                    player.n_garbages_to * 23,
                )
                orange_height = min(to_height, 463)
                red_height = min(from_height * (1 - prog) + to_height * prog, 463)
                # print('heights:', orange_height, red_height)
                if orange_height > red_height:
                    pg.draw.rect(
                        self.screen,
                        (214, 135, 30),
                        pg.Rect(
                            349 + 484 * id,
                            140 + 463 - orange_height,
                            8,
                            orange_height - red_height,
                        ),
                    )  # orange
                pg.draw.rect(
                    self.screen,
                    (197, 32, 21),
                    pg.Rect(349 + 484 * id, 140 + 463 - red_height, 8, red_height),
                )  # red

    def _renderButtons(self):
        for button in self.model.buttons:
            if button.isVisible:
                button.draw(self.screen)

    def _loadImages(self):
        global mainImg, winImg, loseImg, timeupImg, tilesImg, comboImg, KOedImg

        # load base images
        mainImg = pg.image.load("src/Base/img/main.png")
        winImg = pg.image.load("src/Base/img/win.png")
        loseImg = pg.image.load("src/Base/img/lose.png")
        timeupImg = pg.image.load("src/Base/img/timeup.png")

        # load numbers
        for i in range(10):
            numImg[i] = pg.image.load("src/Number/%d.png" % i)

        # load combos
        for i in range(1, const.MAX_COMBO + 1):
            comboImg[i] = pg.image.load("src/Combo/img/%d.png" % i)

        # load KOs
        for i in range(1, const.MAX_KO + 1):
            KOImg[i] = pg.image.load("src/KO/img/%d.png" % i)
        KOedImg = pg.image.load("src/KO/img/KOed.png")

        # load countDown
        for i in range(int(const.COUNTDOWN_LENGTH * 10)):
            img = pg.image.load("src/CountDown/%d.png" % i)
            countDownImg[i] = pg.transform.scale(img, (955, 431))

        # load tiles and draw tetriminos
        for id in range(1, len(const.TILE_CODES)):
            tilesImg[id] = pg.image.load("src/Tiles/%s.png" % const.TILE_CODES[id])
            if id < len(const.TETRIMINO_CODES):
                tile, size = tilesImg[id], const.TETRIMINO_SIZES[id]
                surface = pg.Surface((size[0] * 23, size[1] * 23), pg.SRCALPHA)
                surface.fill((0, 0, 0, 0))

                for y, x in const.TETRIMINOS[id][0]:
                    # adjust rendering of I and O
                    if size[0] >= 3:
                        x += 1
                    if size[0] <= 3:
                        y -= 1
                    surface.blit(tile, (x * 23, -y * 23))
                tetriminoImg[id] = surface

    def _loadSounds(self):

        global mainMusic, gameSounds

        # load base sounds
        pg.mixer.music.load("src/Base/snd/main.ogg")
        pg.mixer.music.set_volume(0.7)
        gameSounds["clear"] = pg.mixer.Sound(file="src/Base/snd/clear.ogg")
        gameSounds["countDown"] = pg.mixer.Sound(file="src/Base/snd/countDown.ogg")
        gameSounds["drop"] = pg.mixer.Sound(file="src/Base/snd/drop.ogg")
        gameSounds["hold"] = pg.mixer.Sound(file="src/Base/snd/hold.ogg")
        gameSounds["lose"] = pg.mixer.Sound(file="src/Base/snd/lose.ogg")
        gameSounds["timeup"] = pg.mixer.Sound(file="src/Base/snd/timeup.ogg")
        gameSounds["win"] = pg.mixer.Sound(file="src/Base/snd/win.ogg")

        # load combo sounds
        for i in range(1, 8):
            gameSounds["combo%d" % i] = pg.mixer.Sound(file="src/Combo/snd/%d.ogg" % i)

        # load KO sounds
        gameSounds["KOed"] = pg.mixer.Sound(file="src/KO/snd/KOed.ogg")
        gameSounds["KOget"] = pg.mixer.Sound(file="src/KO/snd/KOget.ogg")

    def initialize(self):
        """
        Set up the pygame graphical display and loads graphical resources.
        """
        result = pg.init()
        pg.font.init()
        pg.display.set_caption("demo game ")
        self.screen = pg.display.set_mode(WINDOW_SIZE)
        self.clock = pg.time.Clock()
        self.bigfont = pg.font.Font("src/KRKZ.ttf", 40)
        self.screen.blit(
            self.bigfont.render("loading...", True, (255, 255, 255)), (378, 300)
        )
        pg.display.flip()
        self.smallfont = pg.font.Font("src/KRKZ.ttf", 20)
        self.font = pg.font.Font("src/KRKZ.ttf", 30)
        self.isinitialized = True

        self._loadImages()
        self._loadSounds()
