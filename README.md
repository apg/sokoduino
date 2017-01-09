# sokoduino: Sokoban for Arduino

Sokoduino is a project to bring Sokoban to my TV in 1-bit graphics glory.

## Overview

[Sokoban](https://en.wikipedia.org/wiki/Sokoban) is a conceptually
simple game in which a player moves boxes to specific places on the
gameboard. A level is considered solved when all boxes are on their
destinations.

The puzzles can get quite complicated, involving moving the player
back and forth across the screen to provide enough clearance to move
a box.

## Implementations

The end goal of this project is to construct a game in which I can
play on my TV over an RCA video cable. However, I'm taking multiple
steps to get there, and documenting my progress, in hopes that this
will be useful to someone (like me) starting out in electronics
tinkering. As such, each implementation (after the serial port only
one) will have an accompanying [Fritzing](http://fritzing.org/home/)
file which provides a schematic for the circuitry needed to play that
version.

### `console/` - Sokoban in a terminal.

The first implementation is an extremely simple version which shows
the game board on the console and reads movement commands from STDIN.

#### Building

```bash
$ make sokoban
$ ./sokoban
```

#### Playing

The controls are simply, but you'll have to hit the key + `RET` since
no attempt at non-buffered I/O is being made.

- `w` - move up
- `s` - move down
- `a` - move left
- `d` - move right
- ` ` - switch levels

### `serial/` - Sokoban, running on an Arduino Uno controllable over Serial

The second implementation runs on an Arduino Uno is is controllable over
the Serial console, utilizing the same keys as in `console/`. Load the
`sokoban.ino` into the Arduino app, connect your Arduino over USB, and
upload the sketch.

From the menu, open up the `Serial Monitor` to see and interact with
the game.


## Notes on Gameplay

The text based versions of this game (basically up until the TV version)
use a notation for the game board that is commonly used for sharing Sokoban
levels on the Internet. 

- `#` - This is a wall.
- `@` - The player's position
- `.` - A boxes final destination; the goal.
- `$` - A box
- ` ` - Floor tile
- `+` - The player is on a goal tile.
- `*` - A box is on a goal tile.

## Notes on Implementation

Sokoduino supports boards no larger than 8x8. This restriction is present
due to the way the implementation stores the board state. For each type
of board entity--walls, boxes, goals--8 `unsigned char` are used. The most
significant bit is the left most tile on the board. 

As an example, the simplest board:

```
#####
#@$.#
#####
```

is represented in the following way:

```
.walls = {0xF8, 0x88, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0},
.goals = {0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
.boxes = {0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
```

That is to say:

```
##### : walls=0b11111000, goals=0b00000000, boxes=0b00000000
#@$.# : walls=0b10001000, goals=0b00010000, boxes=0b00100000
##### : walls=0b11111000, goals=0b00000000, boxes=0b00000000
```

The player's position is simply stored as `x=1, y=1` to start with.

To help with this encoding, the program `soklevel.c` will read an 8x8 level
formatted as specified in "Notes on Gameplay" above, and spit out the values
for the game struct. `soklevel.c` does not do error checking, or level
feasibility--you'll have to do that by yourself.

Win testing is done simply by comparing the current state of goals against
the current state of boxes, making 8 comparisons at most. If they are all
equal, the board is in a solved state and the game finishes.

## Contributions

As this is a personal, for learning project, I am unlikely to merge
PRs.  However, if you have feedback to give on the schematics, wiring,
or even code, please do!

## Copyright

Copyright 2017, Andrew Gwozdziewycz, web@apgwoz.com

Licensed under the GNU GPLv3. See LICENSE for more details.
