# PACMAN Game using C and RAYLIB

## Table of Contents
- [Pac-Man Game using C and Raylib](#pac-man-game-using-c-and-raylib)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Features](#features)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Installing Raylib](#installing-raylib)
    - [Building the Game](#building-the-game)
  - [Usage](#usage)
  - [Controls](#controls)
  - [Game Design](#game-design)
    - [Maze](#maze)
    - [Score](#score)
    - [Levels](#levels)
  - [Credits](#credits)
  - [License](#license)

## Introduction
Welcome to the Pac-Man game, developed using C and the Raylib library. This project is a simple recreation of the classic arcade game Pac-Man. The goal is to navigate Pac-Man through a maze, eating all the dots while avoiding ghosts.

## Features
- Classic Pac-Man gameplay
- Simple, intuitive controls
- Retro-style graphics
- Sound effects
- Score tracking

## Installation

### Prerequisites
- C Compiler (e.g., GCC)
- Raylib library installed

### Installing Raylib
If you don't have Raylib installed, follow the instructions on the [Raylib installation page](https://github.com/raysan5/raylib#installation).

### Building the Game
1. Clone the repository:
   ```sh
   git clone https://github.com/thirumuruganra/PACMAN-using-C-and-RAYLIB.git
   cd pacman-raylib
   ```

2. Compile the game:
   ```sh
   gcc -o pacman PACMAN_Final.c -lraylib -lm -lpthread -ldl -lrt
   ```

3. Run the game:
   ```sh
   ./pacman
   ```

## Usage
After building the game, run the executable to start playing. Use the controls listed below to navigate Pac-Man through the maze.

## Controls
- **Arrow Keys**: Move Pac-Man
- **Y**: To Replay the Game After Winning or Losing
- **N**: Quit the game After Winning or Losing
- **Esc**: Quit the game while playing

## Game Design
### Maze
The maze consists of walls, pellets, power pellets, and ghosts. The objective is to eat all the pellets and super pellets while avoiding the ghosts. Eating a power pellet allows Pac-Man to eat the ghosts for a short duration (that is 9 seconds).

### Score
Points are awarded for eating pellets, super pellets and ghosts. The game tracks your score, which is displayed at the bottom of the screen.

### Levels
The game includes EASY, MEDIUM and HARD levels with increasing difficulty. Each level increases the speed of the ghosts, thereby making the game more challenging and complex.

### 

## Credits
- **Thirumurugan RA** - [thirumuruganra](https://github.com/thirumuruganra/)
- **Vishal Muralidharan** - [vizz1006](https://github.com/vizz1006)
- **Varun Prakash** - [hauseboi](https://github.com/hauseboi)
- **Raylib**: An easy-to-use library to enjoy videogames programming by [Ray San](https://www.raylib.com/)
- **Original Pac-Man Game**: Created by Toru Iwatani and published by Namco
  

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
