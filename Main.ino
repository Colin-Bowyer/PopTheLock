#include <Adafruit_CircuitPlayground.h>

bool buttonFlag, switchFlag;
bool lost;
bool dir = true;

int playerX, targetX;
int score, highScore;
int pixels[10];
int lastButtonTime;

void setup() {
  CircuitPlayground.begin();

  Serial.begin(9600);
  while (!Serial);

  attachInterrupt(digitalPinToInterrupt(4), buttonInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(5), buttonInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(7), switchInterrupt, CHANGE);

  for (int i = 0; i < 10; i++) {
    pixels[i] = 0x000000;
  }
}

void loop() {
  if (buttonFlag) {
    buttonFlag = false;
    displayHighScore();
  }

  if (switchFlag) {
    delay(5);
    switchFlag = false;
    game();
  }
}

void game() {
  playerX = 0;
  targetX = 9;
  draw();
  lost = false;
  while (!lost) {
    Serial.println(score);
    if (buttonFlag) {
      dir = !dir;
      buttonFlag = false;
      if(targetX == playerX){
        score++;
        newTarget();
      } else{
        lose();
      }
    }
    advance();
    draw();
    delay(300);
  }
}

void displayHighScore() {
  //TODO: Use speaker, store high score? Get code from somewhere?
  Serial.print("High Score: ");
  Serial.println(highScore);
}

void buttonInterrupt() {
  unsigned long currentTime = millis();
  if(currentTime - lastButtonTime > 200) {
    buttonFlag = true;
    lastButtonTime = currentTime;
  }
}

void switchInterrupt() {
  switchFlag = true;
}

void advance() {
  if (playerX == targetX){
    lose();
    return;
  }
  if (dir) playerX++;
  else playerX--;

  if (playerX < 0) {
    playerX = 9;
  } else if (playerX >= 10) {
    playerX = 0;
  }
}

void draw() {
  if(lost) return;
  clearArray();

  pixels[targetX] = 0xFF0000;
  pixels[playerX] = 0xFFFFFF;

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, pixels[i]);
  }
}

void clearArray() {
  for (int i = 0; i < 10; i++) {
    pixels[i] = 0;
  }
}

void newTarget(){
  int newTargetLoc = random(playerX + 4, playerX + 8);

  if (newTargetLoc < 0) {
    newTargetLoc += 10;
  } else if (newTargetLoc >= 10) {
    newTargetLoc -= 10;
  }

  targetX = newTargetLoc;
}

void lose(){
  lost = true;
  if (score > highScore) highScore = score;
  Serial.println("YOU LOSE");
  for(int i = 0; i < 10; i ++){
    CircuitPlayground.setPixelColor(i, 0xFF0000);
  }
  delay(2000);

  CircuitPlayground.clearPixels();
}
