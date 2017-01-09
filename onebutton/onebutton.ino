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
#define NEWGAME ' '

#define NUM_LEVELS 2

#define BUTTON_PIN 2


#define INBOUNDS(x, y, min, max) \
  ((x >= min) && (y >= min) && (x < max) && (y < max))

struct game {
  unsigned char walls[8];
  unsigned char boxes[8];
  unsigned char goals[8];
  char x;
  char y;
};

struct game game;

struct game games[NUM_LEVELS] = {
  {
    {0xF8, 0x88, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    1,
    1
  },
  {
    {0xfe, 0x8a, 0x82, 0x82, 0x82, 0x82, 0xfe, 0x0},
    {0x0, 0x0, 0x68, 0x8, 0x0, 0x10, 0x0, 0x0},
    {0x0, 0x40, 0x20, 0x0, 0x30, 0x10, 0x0, 0x0},
    2,
    1
  }
};

int gamei = 0;
int moves = 0;

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
  if (bittest(game.walls[ty], 7 - tx)) {
    return;
  }

  // Pushing up against a box?
  if (bittest(game.boxes[ty], 7 - tx)) {
    // possibly push the box.
    bx = tx + dx;
    by = ty + dy;
    if (!INBOUNDS(bx, by, 0, 8)) {
      return;
    }

    // Wall prohibits box from moving there.
    if (bittest(game.walls[by], 7 - bx)) {
      return;
    }
    // Box prohibits other box from moving there.
    if (bittest(game.boxes[by], 7 - bx)) {
      return;
    }

    // unset box at tx, ty
    // set box at bx, by
    bitclr(&(game.boxes[ty]), 7 - tx);
    bitset(&(game.boxes[by]), 7 - bx);

  }

  game.x = tx;
  game.y = ty;
}

static void
draw() 
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
        Serial.write(WALL);
        continue;
      }

      player = (game.x == x && game.y == y) ? 1: 0;
      box = bittest(game.boxes[y], 7-x);
      goal = bittest(game.goals[y], 7-x);

      if (player && goal) {
        Serial.write(PLAYER_ON_GOAL);
      } else if (player) {
        Serial.write(PLAYER);
      } else if (box && goal) {
        Serial.write(BOX_ON_GOAL);
      } else if (box) {
        Serial.write(BOX);
      } else if (goal) {
        Serial.write(GOAL);
      } else {
        Serial.write(FLOOR);
      }
    }
    Serial.write('\n');
  }
  Serial.write('\n');
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

static void
newgame(int t)
{
  int i;
  for (i = 0; i < 8; i++) {
    game.walls[i] = games[t].walls[i];
    game.boxes[i] = games[t].boxes[i];
    game.goals[i] = games[t].goals[i];
  }
  game.x = games[t].x;
  game.y = games[t].y;
}

static int
next() {
  if (Serial.available() > 0) {
    return Serial.read();
  }

  // Read the button, and return ' ' to *switch* board.
  if (digitalRead(BUTTON_PIN) == HIGH) {
    return ' ';
  }
  
  return 0;
}

void
setup() {
  pinMode(BUTTON_PIN, INPUT);
 
  Serial.begin(9600);
  while (!Serial);
  
  gamei = 0;
  newgame(gamei);
  draw();
}

void
loop() {
  switch (next()) {
    case NEWGAME:
      gamei = (gamei + 1) % NUM_LEVELS;
      moves = 0;
      newgame(gamei);
      break;
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
      return;
  }

  moves++;
  if (wins()) {
    Serial.print("YOU WIN!\n");
  } else {
    draw();
  }
}

