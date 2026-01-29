#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// НАСТРОЙКИ КАЛИБРОВКИ (Вставьте свои числа)
#define TS_MINX ...   // Сюда пишем Raw X из левого верхнего угла
#define TS_MAXX ...  // Сюда пишем Raw X из правого нижнего угла
#define TS_MINY ...   // Сюда пишем Raw Y из левого верхнего угла
#define TS_MAXY ...  // Сюда пишем Raw Y из правого нижнего угла

// Пины подключения
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4
#define TOUCH_CS  27
#define TOUCH_SCK 14
#define TOUCH_MISO 12
#define TOUCH_MOSI 13

SPIClass hspi(HSPI); 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS);

#define GRID_W 10
#define GRID_H 11      
#define CELL_SIZE 24
#define TOP_OFFSET 42  
#define MINES_COUNT 15

struct Cell {
  bool isMine;
  bool isOpen;
  bool isFlagged;
  int neighbors;
};

Cell grid[GRID_W][GRID_H];
bool gameOver = false;
bool winState = false;
bool digMode = true;

uint16_t rainbow[] = {ILI9341_RED, ILI9341_ORANGE, ILI9341_YELLOW, ILI9341_GREEN, ILI9341_CYAN, ILI9341_MAGENTA};

void drawCell(int x, int y) {
  int px = x * CELL_SIZE;
  int py = y * CELL_SIZE + TOP_OFFSET;
  if (!grid[x][y].isOpen) {
    tft.fillRect(px, py, CELL_SIZE, CELL_SIZE, 0x7BEF); 
    tft.drawRect(px, py, CELL_SIZE, CELL_SIZE, ILI9341_BLACK);
    if (grid[x][y].isFlagged) {
      tft.setTextColor(ILI9341_RED);
      tft.setTextSize(2);
      tft.setCursor(px + 6, py + 5); tft.print("F");
    }
  } else {
    tft.fillRect(px, py, CELL_SIZE, CELL_SIZE, ILI9341_LIGHTGREY);
    tft.drawRect(px, py, CELL_SIZE, CELL_SIZE, 0x94B2);
    if (grid[x][y].isMine) {
      tft.fillCircle(px + 12, py + 12, 7, ILI9341_BLACK);
    } else if (grid[x][y].neighbors > 0) {
      tft.setTextColor(ILI9341_BLUE);
      tft.setTextSize(2);
      tft.setCursor(px + 7, py + 5); tft.print(grid[x][y].neighbors);
    }
  }
}

void playWinAnimation() {
  static unsigned long lastAnim = 0;
  static int colorIdx = 0;
  if (millis() - lastAnim > 200) {
    lastAnim = millis();
    colorIdx = (colorIdx + 1) % 6;
    tft.setCursor(15, 140);
    tft.setTextColor(rainbow[colorIdx], ILI9341_BLACK);
    tft.setTextSize(3);
    tft.print("VY POBEDILI!");
    tft.drawRect(5, 130, 230, 50, rainbow[(colorIdx + 3) % 6]);
  }
}

void checkWin() {
  int closedCells = 0;
  for (int x = 0; x < GRID_W; x++) {
    for (int y = 0; y < GRID_H; y++) {
      if (!grid[x][y].isOpen) closedCells++;
    }
  }
  if (closedCells == MINES_COUNT && !gameOver) {
    gameOver = true;
    winState = true;
    tft.fillScreen(ILI9341_BLACK);
  }
}

void revealMines() {
  for (int x = 0; x < GRID_W; x++) {
    for (int y = 0; y < GRID_H; y++) {
      if (grid[x][y].isMine) { grid[x][y].isOpen = true; drawCell(x, y); }
    }
  }
  tft.setCursor(50, 150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(4);
  tft.print("BOOM!");
}

void drawUI() {
  tft.fillRect(5, 5, 100, 32, digMode ? ILI9341_GREEN : ILI9341_ORANGE);
  tft.drawRect(5, 5, 100, 32, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(20, 13);
  tft.setTextSize(2);
  tft.print(digMode ? "DIG" : "FLAG");

  tft.fillRect(135, 5, 100, 32, ILI9341_BLUE);
  tft.drawRect(135, 5, 100, 32, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(150, 13);
  tft.print("RESET");
}

void newGame() {
  gameOver = false;
  winState = false;
  tft.fillScreen(ILI9341_BLACK);
  for (int x = 0; x < GRID_W; x++) {
    for (int y = 0; y < GRID_H; y++) {
      grid[x][y] = {false, false, false, 0};
    }
  }
  int m = 0;
  randomSeed(millis());
  while (m < MINES_COUNT) {
    int rx = random(GRID_W), ry = random(GRID_H);
    if (!grid[rx][ry].isMine) { grid[rx][ry].isMine = true; m++; }
  }
  for (int x = 0; x < GRID_W; x++) {
    for (int y = 0; y < GRID_H; y++) {
      if (grid[x][y].isMine) continue;
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          int nx = x + dx, ny = y + dy;
          if (nx >= 0 && nx < GRID_W && ny >= 0 && ny < GRID_H && grid[nx][ny].isMine) 
            grid[x][y].neighbors++;
        }
      }
    }
  }
  drawUI();
  for (int x = 0; x < GRID_W; x++)
    for (int y = 0; y < GRID_H; y++) drawCell(x, y);
}
void open(int x, int y) {
  if (x < 0  x >= GRID_W  y < 0  y >= GRID_H  grid[x][y].isOpen || grid[x][y].isFlagged) return;
  grid[x][y].isOpen = true;
  drawCell(x, y);
  if (grid[x][y].isMine) {
    gameOver = true;
    revealMines();
    return;
  }
  if (grid[x][y].neighbors == 0) {
    for (int dx = -1; dx <= 1; dx++)
      for (int dy = -1; dy <= 1; dy++) open(x + dx, y + dy);
  }
  checkWin();
}

void setup() {
  tft.begin();
  tft.setRotation(2); 
  hspi.begin(TOUCH_SCK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(hspi);
  ts.setRotation(2);
  newGame();
}

void loop() {
  if (winState) playWinAnimation();

  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int tx, ty;
    if (INVERT_X) tx = map(p.x, TS_MAXX, TS_MINX, 0, 240);
    else tx = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    if (INVERT_Y) ty = map(p.y, TS_MAXY, TS_MINY, 0, 320);
    else ty = map(p.y, TS_MINY, TS_MAXY, 0, 320);

    if (ty < TOP_OFFSET || winState) {
      if (tx > 130 && tx < 240) { newGame(); delay(300); }
      if (ty < TOP_OFFSET && tx < 110 && !winState) { digMode = !digMode; drawUI(); delay(300); }
    } else if (!gameOver) {
      int gx = tx / CELL_SIZE;
      int gy = (ty - TOP_OFFSET) / CELL_SIZE;
      if (gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H) {
        if (digMode) open(gx, gy);
        else if (!grid[gx][gy].isOpen) {
          grid[gx][gy].isFlagged = !grid[gx][gy].isFlagged;
          drawCell(gx, gy);
        }
      }
      delay(200);
    }
  }
}