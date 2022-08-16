# Tetris-Battle-pygame
 ![Python](https://img.shields.io/badge/python-3670A0?style=flat-square&logo=python&logoColor=ffdd54) ![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg?style=flat-square) ![MIT badge](https://img.shields.io/github/license/EdgedSquirrels/Tetris-Battle-pygame?style=flat-square) ![CodeFactor](https://www.codefactor.io/repository/github/edgedsquirrels/tetris-battle-pygame/badge?style=flat-square)
```bash
pip3 install pygame
git clone ...
cd ...
python3 main.py
```
Enjoy and have fun :slightly_smiling_face:
## Introduction
[Tetris Battle](https://tetris.fandom.com/wiki/Tetris_Battle) was a well-known online tetris game on Facebook. But due to its closure in 2019, the game is no longer available in Facebook. As an aficionado of this game, I re-made the [game](https://github.com/EdgedSquirrels/Tetris-Battle) in C++ with [SFML](https://www.sfml-dev.org/). However, setting up the SFML environment may be hard for beginners. Therefore, I made a pygame version based on [mvc-game-design](https://github.com/wesleywerner/mvc-game-design), and it will make installation easier. Also, the framework is redesigned for better readibility and maintenance.

## Game Mechanism
Most of the implementation follows [Tetris Guideline](https://tetris.fandom.com/wiki/Tetris_Guideline). And this implementation supports Super Rotation System/Standard Rotation System ([SRS](https://tetris.fandom.com/wiki/SRS)) as well.

### Scoring
The scoring mechanism is based on [Tetris Battle](https://tetris.fandom.com/wiki/Tetris_Battle#Garbage) settings.

| Combo | Sent | Line Clear    | Sent |
| ----- | ---- | ------------- | ---- |
| 1     | 1    | Single        | 0    |
| 2     | 1    | Double        | 1    |
| 3     | 2    | Triple        | 2    |
| 4     | 2    | Tetris        | 4     |
| 5     | 3    | T-spin Mini   | 1    |
| 6     | 3    | T-spin Single | 2    |
| 7     | 4    | T-spin Double | 4    |
| 8     | 4    | T-spin Triple | 6    |
| 9     | 4    | B2B Tetris    | 6    |
| 10    | 4    | B2B T-Mini    | 2    |
| 11    | 4    | B2B T-Single  | 3    |
| 12    | 4    | B2B T-Double  | 6    |
| 13    | 4    | B2B T-Triple  | 9    |
| 14+   | 4    | Perfect Clear | 10   |

### Players
It currently supports 1 player or 2 players. It can be set from the menu. For the two players, 2 modes can be chosen from: `KEYBOARD`, `LOG`.

The description of playing modes are shown below:
- `KEYBOARD`:
  In this mode, the player is manipulated with keyboard. The  mappings are shown below:
  **1P**:
  - `Up` and `X` are to rotate 90° clockwise.
  - `Space` is to hard drop.
  - `Shift` and `C` are to hold.
  - `Ctrl` and `Z` are to rotate 90° counterclockwise.
  - `Left`, `Right`, and `Down` arrows are to move. 

  **2P**:
  - `0` is to hold.
  - `8`, `4`, `6`, and `2` are hard drop, left shift, right shift, and soft drop respectively.
  - `1`, `5`, and `9` are to rotate 90° clockwise.
  - `3` and `7` are to rotate 90° counterclockwise.
- `LOG`:
  In this mode, the player is from the log file `log/log.txt` or `log/log2.txt`. The game will automatically log the movements you make in each game. The name of the log file is in this format: `log-[1P|2P] [year]-[month]-[date] [hour]-[minute]-[second].txt`. If you want to play against your log, simply rename the log file to `log/log.txt`(for **1P**)or `log/log2.txt`(for **2P**). If you want to learn more about how the log is stored, you are recommended to visit [log structure](##Log-structure).


## File structure
- `log`
The directory that stores player logs.
- `src`
The directory that stores images and sound effects.
- `eventmanager.py`
The file that defines events.
- `const.py`
The file that defines the game, such as width, height, etc.
- `model.py`
The file that defines the main structure.
- `view.py`
The file that handles graphics and sound effects.
- `controll.py`
The file that announce player movements to model.
- `main.py`
The main file that integrates the above functionalities.

## Log structure
The player log records how the player moves. And it is stored in pure text file. It records some characteristices of the player:
- `TETRIMINO_CODE`: It declares the types of tetriminos.
  - `Z`: 1, `L`: 2, `O`: 3, `S`: 4, `I`: 5, `J`: 6, `T`: 7
  -  It is the same as rainbow color order.
- `TIME_CLK`: It records the time stamp of the movement in milliseconds, and the time being KOed is excluded.
- `CENTER_X`, `CENTER_Y`: It records the center of the current falling tetrimino, and the *height of garbage lines* is ignored.

For each line of the log file, it represents one of the following action:
- **Initialization**:
  ```
  TETRIMINO_CODE TETRIMINO_CODE ... TETRIMINO_CODE (6 TETRIMINO_CODEs)
  ```
  It declares the up comming 6 tetriminos. (Current tetrimino + 5 next tetriminos.)

- **Set Initial Position**  
  ```
  TIME_CLK i CENTER_X CENTER_Y
  ```
  Set the initial position of current tetrimino. This is called when the current tetrimino is changed.
- **Move**:
  ```
  TIME_CLK m dx dy
  ```
  Move the current tetrimino by the desired position.
  


- **Hold**:
  ```
  TIME_CLK h [TETRIMINO_CODE]
  ```
  Hold the current tetrimino. `TETRIMINO_CODE` is required for the first hold. It represents the tetrimino appended to the bag instead of the upcoming falling piece.
- **Rotate**:
  ```
  TIME_CLK r isCounterClockwise
  ```
  Rotate the current tetrimino. `m CENTER_X CENTER_Y` is required if the center is changed.
  - `isCounterClockwise`: Decide the rotate direction.
- **Drop**:
  ```
  TIME_CLOCK d TETRIMINO_CODE
  ```
  Hard drop the current tetrimino to the desired position. `TETRIMINO_CODE` will be added to player bag.

## Future work:
- Enable to play against online opponents with sockets.
- Fexible UI that fits different board width and height.
- Available to apply robot agent.
