import pygame as pg

# Warning: All the postions are represented as (y, x) if not specified

BOARD_WIDTH = 10
BOARD_HEIGHT = 40
BOARD_HEIGHT_VISIBLE = 21  # a few pixels of row 21 will be visible
START_LOCATION = [
    20,
    4,
]  # Immediately drop one space if no existing block is in its path

TETRIMINO_I = 5
TETRIMINO_CODES = " ZLOSIJT"  # 0:None
TETRIMINO_SIZES = [
    (0, 0),
    (3, 2),
    (3, 2),
    (2, 2),
    (3, 2),
    (4, 1),
    (3, 2),
    (3, 2),
]  # width, height
TILE_CODES = TETRIMINO_CODES + "DWB"  # "DWB" can't change

TETRIMINOS = {
    1: [  # Z
        [(1, -1), (1, 0), (0, 0), (0, 1)],
        [(1, 1), (0, 1), (0, 0), (-1, 0)],
        [(-1, 1), (-1, 0), (0, 0), (0, -1)],
        [(-1, -1), (0, -1), (0, 0), (1, 0)],
    ],
    2: [  # L
        [(0, -1), (0, 0), (0, 1), (1, 1)],
        [(1, 0), (0, 0), (-1, 0), (-1, 1)],
        [(0, 1), (0, 0), (0, -1), (-1, -1)],
        [(-1, 0), (0, 0), (1, 0), (1, -1)],
    ],
    3: [  # O
        [(1, 0), (0, 0), (0, 1), (1, 1)],
        [(1, 0), (0, 0), (0, 1), (1, 1)],
        [(1, 0), (0, 0), (0, 1), (1, 1)],
        [(1, 0), (0, 0), (0, 1), (1, 1)],
    ],
    4: [  # S
        [(0, -1), (0, 0), (1, 0), (1, 1)],
        [(1, 0), (0, 0), (0, 1), (-1, 1)],
        [(0, 1), (0, 0), (-1, 0), (-1, -1)],
        [(-1, 0), (0, 0), (0, -1), (1, -1)],
    ],
    5: [  # I
        [(0, -1), (0, 0), (0, 1), (0, 2)],
        [(1, 1), (0, 1), (-1, 1), (-2, 1)],
        [(-1, 2), (-1, 1), (-1, 0), (-1, -1)],
        [(-2, 0), (-1, 0), (0, 0), (1, 0)],
    ],
    6: [  # J
        [(0, -1), (0, 0), (0, 1), (1, -1)],
        [(1, 0), (0, 0), (-1, 0), (1, 1)],
        [(0, 1), (0, 0), (0, -1), (-1, 1)],
        [(-1, 0), (0, 0), (1, 0), (-1, -1)],
    ],
    7: [  # T
        [(0, -1), (0, 0), (0, 1), (1, 0)],
        [(1, 0), (0, 0), (-1, 0), (0, 1)],
        [(0, 1), (0, 0), (0, -1), (-1, 0)],
        [(-1, 0), (0, 0), (1, 0), (0, -1)],
    ],
}

# Wall-kick table for Tetrimino I
WALL_KICK_TABLE_I = [
    [  # Clockwise
        [(0, 0), (0, 1), (0, -2), (-2, 1), (1, -2)],
        [(0, 0), (0, -2), (0, 1), (-1, -2), (2, 1)],
        [(0, 0), (0, -1), (0, 2), (2, -1), (-1, 2)],
        [(0, 0), (0, 2), (0, -1), (1, 2), (-2, -1)],
    ],
    [  # Counter-clockwise
        [(0, 0), (0, 2), (0, -1), (1, 2), (-2, -1)],
        [(0, 0), (0, 1), (0, -2), (-2, 1), (1, -2)],
        [(0, 0), (0, -2), (0, 1), (-1, -2), (2, 1)],
        [(0, 0), (0, -1), (0, 2), (2, -1), (-1, 2)],
    ],
]

# Wall-kick table for other Tetriminos
WALL_KICK_TABLE_OTHERS = [
    [  # Clockwise
        [(0, 0), (0, -1), (-1, -1), (2, 0), (2, -1)],
        [(0, 0), (0, -1), (1, -1), (-2, 0), (-2, -1)],
        [(0, 0), (0, 1), (-1, 1), (2, 0), (2, 1)],
        [(0, 0), (0, 1), (1, 1), (-2, 0), (-2, 1)],
    ],
    [  # Counter-clockwise
        [(0, 0), (0, 1), (-1, 1), (2, 0), (2, 1)],
        [(0, 0), (0, -1), (1, -1), (-2, 0), (-2, -1)],
        [(0, 0), (0, -1), (-1, -1), (2, 0), (2, -1)],
        [(0, 0), (0, 1), (1, 1), (-2, 0), (-2, 1)],
    ],
]

FPS = 50
COUNTDOWN_LENGTH = 6.5
GAME_LENGTH = 120
TIMEUP_LENGTH = 3
ENDGAME_LENGTH = 5

MOVEMENTS = {
    pg.K_c: (0, "Hold"),
    pg.K_LSHIFT: (0, "Hold"),
    pg.K_SPACE: (0, "Drop"),
    pg.K_LEFT: (0, "Left"),
    pg.K_RIGHT: (0, "Right"),
    pg.K_DOWN: (0, "Down"),
    pg.K_UP: (0, "Rotate_Clockwise"),
    pg.K_x: (0, "Rotate_Clockwise"),
    pg.K_z: (0, "Rotate_CounterClockwise"),
    pg.K_LCTRL: (0, "Rotate_CounterClockwise"),
}

MOVEMENTS_2PLAYERS = MOVEMENTS.copy()
MOVEMENTS_2PLAYERS.update(
    {
        pg.K_KP0: (1, "Hold"),
        pg.K_KP8: (1, "Drop"),
        pg.K_KP4: (1, "Left"),
        pg.K_KP6: (1, "Right"),
        pg.K_KP2: (1, "Down"),
        pg.K_KP1: (1, "Rotate_Clockwise"),
        pg.K_KP5: (1, "Rotate_Clockwise"),
        pg.K_KP9: (1, "Rotate_Clockwise"),
        pg.K_KP3: (1, "Rotate_CounterClockwise"),
        pg.K_KP7: (1, "Rotate_CounterClockwise"),
    }
)

MAX_COMBO = 18
MAX_KO = 10

# State machine constants
STATE_MENU = 0
STATE_COUNTDOWN = 1
STATE_PLAY = 2
STATE_TIMEUP = 3
STATE_ENDGAME = 4

# PLayer mode
MODE_KEYBOARD = 0
MODE_LOG = 1
