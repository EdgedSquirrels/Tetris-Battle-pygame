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