/**
 * sokoban.c: dirt simple sokoban game supporting 8x8 boards
 *
 * Copyright 2017: Andrew Gwozdziewycz <me@apgwoz.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#define WALL '#'
#define PLAYER '@'
#define PLAYER_ON_GOAL '+'
#define BOX '$'
#define BOX_ON_GOAL '*'
#define GOAL '.'
#define FLOOR ' '

#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

#define INBOUNDS(x, y, min, max) \
  ((x >= min) && (y >= min) && (x < max) && (y < max))

struct game {
  unsigned char walls[8];
  unsigned char boxes[8];
  unsigned char goals[8];
  char x;
  char y;
} games[2] = {
  {
    .walls = {0xF8, 0x88, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0},
    .goals = {0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    .boxes = {0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    .x = 1,
    .y = 1
  },
  {
    .walls = {0xfe, 0x8a, 0x82, 0x82, 0x82, 0x82, 0xfe, 0x0},
    .goals = {0x0, 0x40, 0x20, 0x0, 0x30, 0x10, 0x0, 0x0},
    .boxes = {0x0, 0x0, 0x68, 0x8, 0x0, 0x10, 0x0, 0x0},
    .x = 2,
    .y = 1
  }
};

struct game game;

static int
bittest(unsigned char x, int b)
{
  return x & (1 << b);
}

static void
bitset(unsigned char *x, int b)
{
  *x |= (1 << b);
}

static void
bitclr(unsigned char *x, int b)
{
  *x ^= (1 << b);
}

static void
move(int dx, int dy)
{
  int tx = game.x + dx;
  int ty = game.y + dy;

  // potential next box position.
  int bx = 0;
  int by = 0;

  // general bounds checking
  if (!INBOUNDS(tx, ty, 0, 8)) {
    return;
  }

  // in a wall?
  if (bittest(game.walls[ty], 7-tx)) {
    return;
  }

  // Pushing up against a box?
  if (bittest(game.boxes[ty], 7-tx)) {
    // possibly push the box.
    bx = tx + dx;
    by = ty + dy;
    if (!INBOUNDS(bx, by, 0, 8)) {
      return;
    }

    // Wall prohibits box from moving there.
    if (bittest(game.walls[by], 7-bx)) {
      return;
    }
    // Box prohibits other box from moving there.
    if (bittest(game.boxes[by], 7-bx)) {
      return;
    }

    // unset box at tx, ty
    // set box at bx, by
    bitclr(&(game.boxes[ty]), 7-tx);
    bitset(&(game.boxes[by]), 7-bx);

  }

  game.x = tx;
  game.y = ty;
}

static int
wins()
{
  int y;
  for (y = 0; y < 8; y++) {
    if (game.boxes[y] != game.goals[y]) {
      return 0;
    }
  }
  return 1;
}

static int draw()
{
  int x, y;
  int player = 0;
  int goal = 0;
  int box = 0;

  for (y = 0; y < 8; y++) {
    if (game.walls[y] == 0 && y > 3) {
      break;
    }

    for (x = 0; x < 8; x++) {
      if (bittest(game.walls[y], 7-x)) {
        putchar(WALL);
        continue;
      }

      player = (game.x == x && game.y == y) ? 1: 0;
      box = bittest(game.boxes[y], 7-x);
      goal = bittest(game.goals[y], 7-x);

      if (player && goal) {
        putchar(PLAYER_ON_GOAL);
      } else if (player) {
        putchar(PLAYER);
      } else if (box && goal) {
        putchar(BOX_ON_GOAL);
      } else if (box) {
        putchar(BOX);
      } else if (goal) {
        putchar(GOAL);
      } else {
        putchar(FLOOR);
      }
    }
    putchar('\n');
  }
  putchar('\n');
}

int
main(int argc, char **argv)
{
  int c, moves = 0, gamei;

  game = games[0];

  if (argc == 2) {
    gamei = atoi(argv[1]);
    if (gamei >= 0 && gamei < 2) {
      game = games[gamei];
    }
  }

  draw();
  while (c = getchar()) {
    if (c == '\n') {
      continue;
    }

    switch (c) {
    case UP:
      move(0, -1);
      break;
    case DOWN:
      move(0, 1);
      break;
    case LEFT:
      move(-1, 0);
      break;
    case RIGHT:
      move(1, 0);
      break;
    default:
      continue;
    }
    moves++;
    if (wins()) {
      draw();
      printf("Congratulations! You won in %d moves.\n", moves);
      return 0;
    }
    draw();
  }

  return 0;
}
