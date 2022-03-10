import tkinter as tk
from tkinter import *
import time
import sys
import random
import os

root = tk.Tk()

time1 = ''
clock = Label(root)

canvas_width = 700
canvas_height = 700
canvas = tk.Canvas(root, width = canvas_width+1, height = canvas_height+1, highlightthickness=10, highlightbackground="black")
bg = PhotoImage(file = "map4.png")
canvas.pack(fill = "both", expand = True)
canvas.create_image( 0, 0, image = bg, anchor = "nw")

class Snake:
    snake = []
    snakeblockscoordX = []
    snakeblockscoordY = []
    x = 0
    y = 0
    move = [0,10]
    moveblocks = []
    speed = 1
    
    def __init__(self, xcord, ycord):
        self.x = xcord
        self.y = ycord
        for i in range(10):
            self.addblock(i)

    def addblock(self,i):
        self.moveblocks.append([0,0])
        block=tk.Canvas(canvas,width=10, height=10, bd=0, highlightthickness=0.5, highlightbackground="white", relief='ridge', bg="yellow")
        block.place(x=self.x, y=self.y-i*10)
        self.snake.append(block)
        self.snakeblockscoordX.append(self.x)
        self.snakeblockscoordY.append(self.y-i*10)
        # print("initialise: ", i, "----",self.snakeblockscoordX[i], "- ",self.snakeblockscoordY[i])

    def changedir(self,dir1):
        if dir1=='Up':
            self.move = [0,-10]
        elif dir1=='Left':
            self.move = [-10,0]
        elif dir1=='Down':
            self.move = [0,10]
        elif dir1=='Right':
            self.move = [10,0]
        
    def movesnake(self):
        #special teleportation of a block
        def abs_move(new_x, new_y,j):
            self.snake[j].place(x=new_x, y=new_y)
            self.snakeblockscoordX[j] = new_x
            self.snakeblockscoordY[j] = new_y
            self.x = self.snakeblockscoordX[0]
            self.y = self.snakeblockscoordY[0]

        #passing on block movement orientation.step to the following block
        for j in range(len(self.moveblocks)-1,0,-1):
            self.moveblocks[j]=self.moveblocks[j-1]
            self.snakeblockscoordX[j]=self.snakeblockscoordX[j-1]
            self.snakeblockscoordY[j]=self.snakeblockscoordY[j-1]
            # print("pos:", j, "----",self.snakeblockscoordX[j], ": ",self.snakeblockscoordY[j])
        self.moveblocks[0]=self.move
        #blocking moving backwards and overlapping snake
        if self.moveblocks[0][0] == -self.moveblocks[1][0]:
            self.moveblocks[0][0] = self.moveblocks[1][0]
        if self.moveblocks[0][1] == -self.moveblocks[1][1]:
            self.moveblocks[0][1] = self.moveblocks[1][1]

        # print(self.x, " ", self.y)
        #for every block in the snake, move or teleport
        for j in range(len(self.snake)):
            if self.snakeblockscoordX[j] < 10 or self.snakeblockscoordX[j] > canvas_width or self.snakeblockscoordY[j] < 10 or self.snakeblockscoordY[j] > canvas_height:
                if  self.snakeblockscoordX[j] < 10:             abs_move(canvas_width, self.snakeblockscoordY[j],j)
                if  self.snakeblockscoordX[j] > canvas_width:   abs_move(10,            self.snakeblockscoordY[j],j)
                if  self.snakeblockscoordY[j] < 10:             abs_move(self.snakeblockscoordX[j],canvas_height,j)
                if  self.snakeblockscoordY[j] > canvas_height:  abs_move(self.snakeblockscoordX[j],            10,j)
            else:
                # print(j, "----",self.moveblocks[j][0], "- ",self.moveblocks[j][1])
                self.snake[j].place(x=self.snakeblockscoordX[j], y=self.snakeblockscoordY[j])

        self.x = self.x + int(self.moveblocks[0][0])
        self.y = self.y + int(self.moveblocks[0][1])
        self.snakeblockscoordX[0]=self.x
        self.snakeblockscoordY[0]=self.y

    def teleport(self,new_x,new_y,j):
        self.snake[j].place(x=new_x, y=new_y)
        self.snakeblockscoordX[j] = new_x
        self.snakeblockscoordY[j] = new_y
        self.x = self.snakeblockscoordX[0]
        self.y = self.snakeblockscoordY[0]
        self.movesnake()
        

    def adjustspeed(self,speed):
        self.speed = speed

    def getspeed(self):
        return self.speed


class powerup:
    power_ups = []
    power_upsX = []
    power_upsY = []
    # foodTypes = ["grow", "portal", "ultra_speed","slow_down"]
    # colours = ["red", "blue", "orange", "green"]
    powerTypes = [["grow","red"] , ["portal", "blue"] , ["ultra_speed", "orange"] , ["slow_down", "green"]]
    radius = 10
    j = 1
    
    tagval = ["test0"]

    def __init__(self):
        x = 200
        y = 200 #using random num_gen for food.
        powerRandom = ["grow","red"]
        id = canvas.create_oval(x,y,x+self.radius,y+self.radius,fill=powerRandom[1],tag="test0")
        self.power_ups.append([id,powerRandom[0]])
        self.power_upsX.append(x)
        self.power_upsY.append(y)

    def generate(self,_x,_y,_r):
        print("Generated")
        for s in range (random.choice([1,2])):
            x = _x
            y = _y
            powerRandom = self.powerTypes[_r]
            id = canvas.create_oval(x,y,x+self.radius,y+self.radius,fill=powerRandom[1],tag="test"+str(self.j))
            self.tagval.append("test"+str(self.j))
            self.power_ups.append([id,powerRandom[0]])
            self.power_upsX.append(x)
            self.power_upsY.append(y)
            self.j = self.j+1
    
    def delete(self,j):
        for i in range(len(self.power_ups)):
            self.power_ups.pop(j)
            self.power_upsX.pop(j)
            self.power_upsY.pop(j)
            canvas.delete(self.tagval.pop(j))
            print("Deleted")

    def powerupType(self,p,type):
        if type == "portal":
            new_x = random.randrange(30,500,10)
            new_y = random.randrange(20,500,10)
            for j in range(len(p.snake)):
                p.teleport(new_x,new_y,j)
                new_x = new_x-10
        if type == "grow":
            p.addblock(len(p.snake)+1)
            snakeAnnimation(p,"blue-red")
            clock.after(200,lambda: snakeAnnimation(player,"return"))
        if type == "ultra_speed":
            p.adjustspeed(2)
            snakeAnnimation(p,"yellow")
            clock.after(200,lambda: snakeAnnimation(player,"return"))
        if type == "slow_down":
            p.adjustspeed(0.5)
            snakeAnnimation(p,"green")
            clock.after(200,lambda: snakeAnnimation(player,"return"))
                
def snakeAnnimation(p,animation):
    if animation == "blue-red":
        for j in range(len(p.snake)):
            if j%2==0: p.snake[j].configure(bg="blue")
            else: p.snake[j].configure(bg="red")
    elif animation == "yellow":
        for j in range(len(p.snake)):
            p.snake[j].configure(bg="yellow")
    elif animation == "green":
        for j in range(len(p.snake)):
            p.snake[j].configure(bg="lime green")
    elif animation == "return":
        for j in range(len(p.snake)):
            p.snake[j].configure(bg="yellow")


allplayers = []
player = Snake(random.randint(100,900), random.randint(100,900))
allplayers.append(player)
food = powerup()



def kpress(event):
      if event.keysym == 'Up':
            player.changedir('Up')
            
      if event.keysym == 'Left':
            player.changedir('Left')
            
      if event.keysym == 'Down':
            player.changedir('Down')
            
      if event.keysym == 'Right':
            player.changedir('Right')


def increasespeed():
    if player.getspeed()!=100:
        player.adjustspeed(player.getspeed()+1)

def decreasespeed():
    if player.getspeed()!=1:
        player.adjustspeed(player.getspeed()-1)
def restart():
    for f in range(len(food.power_ups)):
        food.delete(f)

button1 = tk.Button(root, text='Increase Speed',command=increasespeed)
canvas.create_window(270,18,window=button1)
button2 = tk.Button(root, text='Decrease Speed',command=decreasespeed)
canvas.create_window(100,18,window=button2)
button3 = tk.Button(root, text='Restart',command=restart)
canvas.create_window(380,18,window=button3)