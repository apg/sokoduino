/**
 * soklevel.c: dirt simple sokoban ASCII > sokobon.c compatible
 *  level editor.
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
 *
 *
 * Usage: soklevel < level.txt
 *
 * Level's should be no more than 8x8.
 * Each line should end with a newline, on column 9 or less.
 * The defines below give you the format of the rest.
 */

#include <stdio.h>
#include <string.h>

#define WALL '#'
#define PLAYER '@'
#define PLAYER_ON_GOAL '+'
#define BOX '$'
#define BOX_ON_GOAL '*'
#define GOAL '.'

unsigned char walls[8] = {
  0, 0, 0, 0, 0, 0, 0, 0
};
unsigned char boxes[8] = {
  0, 0, 0, 0, 0, 0, 0, 0
};
unsigned char goals[8] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

int playerx = 0;
int playery = 0;

static void
bitset(unsigned char *x, int b)
{
  *x |= (1 << b);
}

int
main(int argc, char **argv)
{
  char buf[10];
  char *out;
  int i, y = 0, x = 0;

  while ((out = fgets(buf, 10, stdin)) && y < 8) {
    for (x = 0; buf[x] != '\n' && buf[x] != '\0'; x++) {
      switch (buf[x]) {
      case WALL:
        bitset(&(walls[y]), 7-x);
        break;
      case BOX:
        bitset(&(boxes[y]), 7-x);
        break;
      case BOX_ON_GOAL:
        bitset(&(boxes[y]), 7-x);
        bitset(&(goals[y]), 7-x);
        break;
      case GOAL:
        bitset(&(goals[y]), 7-x);
        break;
      case PLAYER:
        playerx = x;
        playery = y;
        break;
      case PLAYER_ON_GOAL:
        playerx = x;
        playery = y;
        bitset(&(goals[y]), 7-x);
        break;
      }
    }
    y++;
  }

  printf(".walls = {");
  for (i = 0; i < 7; i++) {
    printf("0x%x, ", walls[i]);
  }
  printf("0x%x", walls[7]);
  printf("},\n");

  printf(".goals = {");
  for (i = 0; i < 7; i++) {
    printf("0x%x, ", goals[i]);
  }
  printf("0x%x", goals[7]);
  printf("},\n");

  printf(".boxes = {");
  for (i = 0; i < 7; i++) {
    printf("0x%x, ",boxes[i]);
  }
  printf("0x%x", boxes[7]);
  printf("},\n");

  printf(".x = %d,\n", playerx);
  printf(".y = %d\n", playery);

  return 0;
}
