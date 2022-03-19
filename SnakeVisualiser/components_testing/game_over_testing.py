from tkinter import *
from tkinter import ttk
from SnakeVisualiser.components.game_over import GameOver
import json
from time import sleep

root = Tk()
root.title("Game over component testing")
mainframe = ttk.Frame(root, padding="3 3 12 12")
mainframe.grid(column=0, row=0, sticky=(N, W, E, S))
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

leaderboard = GameOver(mainframe, "Client 0", 10)

root.mainloop()
