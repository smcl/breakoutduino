// system includes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// project includes
#include "breakoutduino.h"
#include "BallVector.h"
#include "BallPath.h"
#include "Block.h"

BallVector *bumperVectors[8];
Block blocks[BLOCK_COUNT];

const int bumperLeftPin = 3;
const int bumperRightPin = 2;

struct BallVector *newVector() {
  return (BallVector *)malloc(sizeof(BallVector));
}

#include "Point.h"

Point ball;

void setupBumperVectors() {
  // bumper split into seven segments, when you hit each you're reflected in a different direction

  // furthest left (segment 0)
  BallVector *seg0_2 = newVector();
  seg0_2->a = AXIS_X;
  seg0_2->scalar = -1;
  seg0_2->next = NULL;

  BallVector *seg0_1 = newVector();
  seg0_1->a = AXIS_Y;
  seg0_1->scalar = -1;
  seg0_1->next = seg0_2;

  BallVector *seg0 = newVector();
  seg0->a = AXIS_X;
  seg0->scalar = -1;
  seg0->next = seg0_1;

  // segment 1
  BallVector *seg1_1 = newVector();
  seg1_1->a = AXIS_X;
  seg1_1->scalar = -1;
  seg1_1->next = NULL;

  BallVector *seg1 = newVector();
  seg1->a = AXIS_Y;
  seg1->scalar = -1;
  seg1->next = seg1_1;

  // segment 2
  BallVector *seg2_2 = newVector();
  seg2_2->a = AXIS_Y;
  seg2_2->scalar = -1;
  seg2_2->next = NULL;

  BallVector *seg2_1 = newVector();
  seg2_1->a = AXIS_X;
  seg2_1->scalar = -1;
  seg2_1->next = seg2_2;

  BallVector *seg2 = newVector();
  seg2->a = AXIS_Y;
  seg2->scalar = -1;
  seg2->next = seg2_1;

  // segment 3
  BallVector *seg3_1 = newVector();
  seg3_1->a = AXIS_Y;
  seg3_1->scalar = -1;
  seg3_1->next = NULL;

  BallVector *seg3 = newVector();
  seg3->a = AXIS_Y;
  seg3->scalar = -1;
  seg3->next = seg3_1;

  // segment 4
  BallVector *seg4_1 = newVector();
  seg4_1->a = AXIS_Y;
  seg4_1->scalar = -1;
  seg4_1->next = NULL;

  BallVector *seg4 = newVector();
  seg4->a = AXIS_Y;
  seg4->scalar = -1;
  seg4->next = seg4_1;

  // segment 5
  BallVector *seg5_2 = newVector();
  seg5_2->a = AXIS_Y;
  seg5_2->scalar = -1;
  seg5_2->next = NULL;

  BallVector *seg5_1 = newVector();
  seg5_1->a = AXIS_X;
  seg5_1->scalar = 1;
  seg5_1->next = seg5_2;

  BallVector *seg5 = newVector();
  seg5->a = AXIS_Y;
  seg5->scalar = -1;
  seg5->next = seg5_1;

  // segment 6
  BallVector *seg6_1 = newVector();
  seg6_1->a = AXIS_Y;
  seg6_1->scalar = -1;
  seg6_1->next = NULL;

  BallVector *seg6 = newVector();
  seg6->a = AXIS_X;
  seg6->scalar = 1;
  seg6->next = seg6_1;

  // segment 7
  BallVector *seg7_2 = newVector();
  seg7_2->a = AXIS_X;
  seg7_2->scalar = 1;
  seg7_2->next = NULL;

  BallVector *seg7_1 = newVector();
  seg7_1->a = AXIS_Y;
  seg7_1->scalar = -1;
  seg7_1->next = seg7_2;

  BallVector *seg7 = newVector();
  seg7->a = AXIS_X;
  seg7->scalar = 1;
  seg7->next = seg7_1;

  // populate the bumper vectors
  bumperVectors[0] = seg0;
  bumperVectors[1] = seg1;
  bumperVectors[2] = seg2;
  bumperVectors[3] = seg3;
  bumperVectors[4] = seg4;
  bumperVectors[5] = seg5;
  bumperVectors[6] = seg6;
  bumperVectors[7] = seg7;
}

void setupBlocks() {
  for (int i = 0; i < BLOCK_COUNT; i++) {
    blocks[i].x = i * BLOCK_WIDTH;
    blocks[i].y = 10;
    blocks[i].w = BLOCK_WIDTH;
    blocks[i].h = BLOCK_HEIGHT;
    blocks[i].active = true;
    blocks[i].fade = 0;
  }
}

void setupButtons() {
  pinMode(bumperLeftPin, INPUT);
  pinMode(bumperRightPin, INPUT);
}

void renderBlocks() {
  for (int i = 0; i < BLOCK_COUNT; i++) {
    if (blocks[i].active) {
      display.drawRect(blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h, WHITE);
    }

    if (blocks[i].fade > 0) {
      int f = blocks[i].fade;

      while (f > 0 && blocks[i].w >= 0 && blocks[i].h >= 0) {
        blocks[i].x++;
        blocks[i].y++;
        blocks[i].w -= 2;
        blocks[i].h -= 2;
        f--;
      }

      if (blocks[i].w < 1 || blocks[i].h < 1) {
        blocks[i].fade = 0;
      } else {
        blocks[i].fade++;
      }
      display.drawRect(blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h, WHITE);
    }
  }
}

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();

  ball = Point();

  bumper_mul = 4;
  bumper_x = (MAX_X - (8 * bumper_mul)) / 2;

  display.setTextSize(2);
  display.setTextColor(WHITE);

  setupBumperVectors();
  setupBlocks();
  setupButtons();
}

void renderBall() {
  display.drawPixel(ball.x, ball.y, WHITE);
}

int bumper_v = 1;

void maybeResetBlocks() {
  bool allInactive = true;

  for (int i = 0; i < BLOCK_COUNT; i++) {
    allInactive = allInactive && !blocks[i].active;
  }

  if (allInactive) {
    setupBlocks();
  }
}

void maybeResetBall() {
  if (ball.y >= 64) {
    ball.x = 64;
    ball.y = 32; 
  }
}

void renderBumper() {

  int bumperLeftState = digitalRead(bumperLeftPin);
  int bumperRightState = digitalRead(bumperRightPin);


  if (bumperLeftState == HIGH && (3 + bumper_x + (bumper_mul * 8)) < 128) {
    bumper_x += 3;
  }

  if (bumperRightState == HIGH && (bumper_x - 3) > 0) {
    bumper_x -= 3;
  }

  display.drawFastHLine(bumper_x, bumper_y, bumper_mul * 8, WHITE);
}

void loop() {
  display.clearDisplay();

  ball.tick();
  //  delay(100);

  renderBall();
  renderBumper();
  renderBlocks();

  delay(15);
  
  maybeResetBlocks();

  display.display();
}
