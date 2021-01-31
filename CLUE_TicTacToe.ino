#include <Adafruit_Arcada.h> // include for screen


/***
 * 
 * X always goes first!
 * 
 * Button A - Commit Move
 * Button B - Move Spots
 * Reset    - Resets the game
*/

// https://learn.adafruit.com/adafruit-clue/pinouts
// https://learn.adafruit.com/assets/88322 - large pinout diagram
// https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.h - display library

#define DEBUG

#define WHITE_LED 43
#define BUTTON_A 5
#define BUTTON_B 11

#define BOARD_X 20
#define BOARD_Y 40
#define BOARD_WIDTH 200
#define BOARD_HEIGHT 200
#define BOARD_LINE_WIDTH 6
#define CELL_PADDING 2

#define PLAYER_X 1
#define PLAYER_O 2

#define WINNER_NONE 0
#define WINNER_X 1
#define WINNER_O 2
#define WINNER_TIE 3

Adafruit_Arcada arcada;

const int CELL_SIZE = (BOARD_WIDTH - (BOARD_LINE_WIDTH * 2)) / 3;
const int CELL_PADDING2 = CELL_PADDING * 2;

byte board[] = {0,0,0,0,0,0,0,0,0};
byte carrotPos = 0;
bool xTurn = true;
bool redraw = true;
bool winner = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // init LED
  pinMode(WHITE_LED, OUTPUT);
  digitalWrite(WHITE_LED, LOW);
  // init buttons
  pinMode(BUTTON_A, INPUT);
  pinMode(BUTTON_B, INPUT);

   Serial.println("CLUE TIC-TAC-TOE Starting");
   if (!arcada.arcadaBegin()) {
    Serial.print("Failed to begin");
    while (1);
  }
  arcada.displayBegin();

  arcada.display->setCursor(0, 0);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(3);

  // Turn on backlight
  arcada.setBacklight(255);
}

void drawBoard() {
  arcada.display->fillScreen(ARCADA_BLACK);
  int del2 = CELL_SIZE * 2 + BOARD_LINE_WIDTH;
  arcada.display->fillRect(BOARD_X + CELL_SIZE, BOARD_Y, BOARD_LINE_WIDTH, BOARD_HEIGHT, ARCADA_WHITE);
  arcada.display->fillRect(BOARD_X + del2, BOARD_Y, BOARD_LINE_WIDTH, BOARD_HEIGHT, ARCADA_WHITE);
  arcada.display->fillRect(BOARD_X, BOARD_Y + CELL_SIZE, BOARD_WIDTH, BOARD_LINE_WIDTH, ARCADA_WHITE);
  arcada.display->fillRect(BOARD_X, BOARD_Y + del2, BOARD_WIDTH, BOARD_LINE_WIDTH, ARCADA_WHITE);
}

void drawHeading() {
  arcada.display->setCursor(BOARD_X, 0);
  arcada.display->setTextColor(ARCADA_WHITE);
  if(winner != WINNER_NONE) {
    if(winner == WINNER_X)
      arcada.display->print("X Wins!");
    else if(winner == WINNER_O)
      arcada.display->print("O Wins!");
    else
      arcada.display->print("Tie Game!");
    return;
  }
  
  if(xTurn)
    arcada.display->print("X Turn");
  else 
    arcada.display->print("O Turn");
}

void drawPlayerPieces() {
  for(int i = 0; i < 9; i++) {
    int x = i % 3;
    int y = i / 3;
    switch(board[i]) {
      case PLAYER_X: 
        drawX(x, y, ARCADA_CYAN);
        break;
      case PLAYER_O:
        drawO(x, y, ARCADA_PINK);
        break;
    }
  }
}

void drawX(int x, int y, int color) {
  int xPos = BOARD_X + (CELL_SIZE * x) + (BOARD_LINE_WIDTH * x);
  int yPos = BOARD_Y + (CELL_SIZE * y) + (BOARD_LINE_WIDTH * y);
  arcada.display->setTextColor(color);
  arcada.display->setTextSize(9);
  arcada.display->setCursor(xPos + CELL_PADDING2, yPos - CELL_PADDING2);
  arcada.display->print("x");
  // Reset
  arcada.display->setTextColor(ARCADA_WHITE);
  arcada.display->setTextSize(3);
}

void drawO(int x, int y, int color) {
  int xPos = BOARD_X + (CELL_SIZE * x) + (BOARD_LINE_WIDTH * x);
  int yPos = BOARD_Y + (CELL_SIZE * y) + (BOARD_LINE_WIDTH * y);
  int radius1 = (CELL_SIZE - CELL_PADDING2 - (BOARD_LINE_WIDTH << 1)) / 2;
  int radius2 = (CELL_SIZE - CELL_PADDING2 - (BOARD_LINE_WIDTH << 2)) / 2;
  arcada.display->fillCircle(xPos + radius1 + CELL_PADDING, yPos + radius1 + CELL_PADDING, radius1, color);
  arcada.display->fillCircle(xPos + radius1 + CELL_PADDING, yPos + radius1 + CELL_PADDING, radius2, ARCADA_BLACK);
}

void drawCarrot() {
  if(winner != WINNER_NONE)
    return;

  int x = carrotPos % 3;
  int y = carrotPos / 3;
  if(xTurn)
    drawX(x, y, ARCADA_YELLOW);
  else
    drawO(x, y, ARCADA_YELLOW);
}

void updateCarrotPos() {
  if(winner != WINNER_NONE)
    return;

  int newPos = (carrotPos + 1) % 9;
  while(board[newPos] != 0) {
    newPos = (newPos + 1) % 9;
  }
  carrotPos = newPos;
}

void commitPiece() {
  board[carrotPos] = xTurn ? PLAYER_X : PLAYER_O;
  xTurn = !xTurn;
}

void checkWinner() {
  if(isWinner(PLAYER_X))
    winner = WINNER_X;
  else if(isWinner(PLAYER_O))
    winner = WINNER_O;
  
  if(winner == WINNER_NONE) {
    for(int i = 0; i < 9; i++) {
      if(board[i] == 0)
        return; // the whole board isn't filled up so return
    }
    winner = WINNER_TIE;
  }
}

bool isWinner(int player) {
    bool row =     
    (board[0] == player && board[1] == player && board[2] == player) ||
    (board[3] == player && board[4] == player && board[5] == player) ||
    (board[6] == player && board[7] == player && board[8] == player);

    bool col = 
    (board[0] == player && board[3] == player && board[6] == player) ||
    (board[1] == player && board[4] == player && board[7] == player) ||
    (board[2] == player && board[5] == player && board[8] == player);
    
    bool diag = 
    (board[0] == player && board[4] == player && board[8] == player) ||
    (board[2] == player && board[4] == player && board[6] == player);

    return row || col || diag;
}

void loop() {
  if(redraw) {
    drawBoard();
    drawHeading();
    drawPlayerPieces();
    drawCarrot();
    redraw = false;
  }

  if(winner != WINNER_NONE) {
    // use reset to clear
    return;
  }

  // player interaction
  //uint8_t pressed_buttons = arcada.readButtons();
  //if (pressed_buttons & ARCADA_BUTTONMASK_A) {
  if(digitalRead(BUTTON_A) == 0) {
    updateCarrotPos();
    redraw = true;
  } else if (digitalRead(BUTTON_B) == 0) {
    commitPiece();
    checkWinner();
    if(winner == WINNER_NONE) updateCarrotPos();
    redraw = true;
  }
  delay(500);
}
