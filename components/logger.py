import const
import time
import pygame as pg


def is_number(s):
    """Returns True is string is a number."""
    try:
        int(s)
        return True
    except ValueError:
        return False


class Logger:
    """
    Handles log I/O for files

    keyboard:
        keyboard -> game -> writeLog -> logger -> file
    logfile:
        logger -> game
    """

    def __init__(self, player):
        self.player = player

        # open outFile
        time_str = time.strftime("20%y-%m-%d %H-%M-%S", time.localtime())
        self.outFile = open("log/log %s %dP.txt" % (time_str, player.id + 1), "w")

        # open log file
        if player.mode == const.MODE_LOG:
            self.buffer = (
                []
            )  # To store the information that has not applied to the game
            self.inFile = open("log/log.txt" if player.id == 0 else "log/log2.txt", "r")
            line = self.inFile.readline()
            self.writeLog(line[:-1])
            line = list(map(int, line.split()))
            # print("handling:", line)
            player.initBag(line)

    def writeLog(self, data: str):
        """
        Send the data to log file.
        """
        print(data, file=self.outFile)

    def tickLog(self):
        """
        Given the timeClock, check whether the logger should take any action
        """
        time_clk = (
            pg.time.get_ticks() - self.player.game_timer - self.player.KOed * 1000
        )
        if self.player.mode == const.MODE_LOG:
            while True:
                if self.buffer == []:
                    line = self.inFile.readline()
                    if line == "":
                        break
                    else:
                        self.buffer.append(
                            [int(x) if is_number(x) else x for x in line.split()]
                        )

                line = self.buffer[0]
                if line[0] > time_clk - self.player.KOed * 1000:
                    break
                self.buffer.pop(0)
                print("handling:", line)

                if line[1] == "h":
                    self.player.hold(addToBag=line[2:])
                elif line[1] == "d":
                    self.player.drop(addToBag=line[2:])
                elif line[1] == "i":
                    self.player.initCurrentPiece(line[2], line[3])
                elif line[1] == "m":
                    self.player.move(line[3], line[2])
                elif line[1] == "r":
                    self.player.rotate(line[2])

    def __del__(self):
        if self.player.mode == const.MODE_LOG:
            self.inFile.close()
        self.outFile.close()
