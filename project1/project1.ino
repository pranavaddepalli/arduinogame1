/*
 * Arduino Aimlab
 * Pranav Addepalli
 * for CMU's 05-333 course
 */


/*------- Hardware configuration -------*/
const int row1 = 10;
const int row2 = 11;
const int row3 = 12;
const int row4 = 9;
const int row5 = 2;
const int row6 = 3;
const int row7 = 4;
const int row8 = 5;


const int col1 = A5;
const int col2 = A4;
const int col3 = A3;
const int col4 = A2;
const int col5 = 6;
const int col6 = 7;
const int col7 = 13;
const int col8 = A1;

const int joyy = A7;
const int joyx = A6;

// this is how long we wait before continuing the scan
const int framerate = 2;

// these are the starting x and y values
int basex;
int basey;

// this is the level we require for a direction to be activated
const int joybuffer = 50;

/* 
 *  Rows: ON - HIGH, OFF - LOW
 *  Columns: ON - LOW, OFF - HIGH
 *      
 *         Column
 *          1 0
 *  ROW 1 | 0 1
 *      0 | 0 0
 *    
 */

// store rows and cols as a 8x8 matrix that we can modify with HIGH/LOW values
const int rows[8] = {row1, row2, row3, row4, row5, row6, row7, row8};
const int cols[8] = {col1, col2, col3, col4, col5, col6, col7, col8};

int matrix[8][8];


//===============================================================================
// CORE MATRIX LEVEL CODE
//===============================================================================

// turns off all LEDs on the matrix
void blank(){
  for(int r = 0; r < 8; r++){
    for(int c = 0; c < 8; c++){
      matrix[r][c] = LOW;   
    }
  }
}

// shows the image described by `matrix`
void render(){

  // turns on each LED by scanning matrix column by column
  for (int c = 0; c < 8; c++) {
    // turn column on
    digitalWrite(cols[c], LOW);

    // turn the right rows on
    for (int r = 0; r < 8; r++){
      int val = matrix[r][c];
      digitalWrite(rows[r], val);
    }
    
    delay(framerate);
    
    // turn the column back off
    digitalWrite(cols[c], HIGH);
    for (int r = 0; r < 8; r++){
      digitalWrite(rows[r], LOW);
    }
    
  }
}

// turns on col based on a given 8 bit bitpattern
// col is 0 - 7
void oncol(int col, int bitpattern) {
    
  for(int i = 0; i < 8; i++){
    // if the ith bit of `bitpattern` is 0, LED off, otherwise, LED on
    if((bitpattern >> (7 - i)) & 1){
      // on
      matrix[i][col] = HIGH;
    }
    else {
      matrix[i][col] = LOW;
    }
  }
}

// sets the matrix based on a given array of col bitpatterns
void show(int image[]){
  for(int i = 0; i < 8; i++){
    oncol(i, image[i]);
  }
}

// constant bit patterns for rendering digits on the matrix
int firstDigits[10][4] = {
   {0b01111110, 0b01000010, 0b01111110, 0},
   {0, 0b01111110, 0, 0},
   {0b01011110, 0b01010010, 0b01110010, 0},
   {0b01010010, 0b01010010, 0b01111110, 0},
   {0b01110000, 0b00010000, 0b01111110, 0},
   {0b01110010, 0b01010010, 0b01011110, 0},
   {0b01111110, 0b01001010, 0b01001110, 0},
   {0b01000000, 0b01000000, 0b01111110, 0},
   {0b01111110, 0b01001010, 0b01111110, 0},
   {0b01110000, 0b01010000, 0b01111110, 0}
   };

int secondDigits[10][4] = {
  {0, 0b01111110, 0b01000010, 0b01111110},
  {0, 0, 0, 0b01111110},
  {0, 0b01011110, 0b01010010, 0b01110010},
  {0, 0b01010010, 0b01010010, 0b01111110},
  {0, 0b01110000, 0b00010000, 0b01111110},
  {0, 0b01110010, 0b01010010, 0b01011110},
  {0, 0b01111110, 0b01001010, 0b01001110},
  {0, 0b01000000, 0b01000000, 0b01111110},
  {0, 0b01111110, 0b01001010, 0b01111110},
  {0, 0b01110000, 0b01010000, 0b01111110}
};

// this puts two digits together into one image and shows it
void showTwoDigits(int a, int b) {
  int img[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for(int i = 0; i < 4; i++){
    img[i] = firstDigits[a][i];
    img[i + 4] = secondDigits[b][i];
  }
  show(img);
}

// this displays a two digit integer on the matrix
void showNum(int num) {
  int tens = num / 10;
  int ones = num % 10;
  showTwoDigits(tens, ones);
}

// this returns a string representing what direction the joystick is in
// return values are 'none', 'right', 'left', 'up', or 'down'
String joyDir() {
  String dir = "none";
  
  int currx = analogRead(joyx);
  int curry = analogRead(joyy);
  
  if (currx == 1023) {
    dir = "left";
  }
  if (currx == 0) {
    dir = "right";
  }
  if (curry == 1023) {    
    dir = "down";
  }
  if (curry == 0) {
    dir = "up";
  }
  return dir;
}

//===============================================================================
// CORE GAME LEVEL CODE
//===============================================================================

// this is how long the game takes (in seconds)
const int gameTime = 10;

// this will track the user's score
int score = 0;

// these will track where the pointer is
int r = 5;
int c = 5;

// these will track the current target (2 by 2 specified by the top left row, col)
int targetR;
int targetC;

// this will track the start time of the game
int startt = millis() / 1000; 

// checks if the time is up and updates game timer
bool checkTimer() {
  int timeleft = gameTime + startt - (millis() / 1000);
  // show the timer
  if (timeleft <= 10 && timeleft > 8){
    oncol(7, 0xFF);
  }
  if (timeleft <= 8){
    int remaining = ~((0xFF << timeleft) & 0xFF);
    oncol(7, remaining);
  }
  
  return timeleft > 0;
}

// turns the target at targetR, targetC to ON
void targetOn() {
  matrix[targetR][targetC] = HIGH;
  matrix[targetR + 1][targetC] = HIGH;
  matrix[targetR + 1][targetC + 1] = HIGH;
  matrix[targetR][targetC + 1] = HIGH;
}

// turns the target at targetR, targetC to OFF
void targetOff() {
  matrix[targetR][targetC] = LOW;
  matrix[targetR + 1][targetC] = LOW;
  matrix[targetR + 1][targetC + 1] = LOW;
  matrix[targetR][targetC + 1] = LOW;
}

// checks if the pointer is in the current target
bool inTarget() {
  return ( (r == targetR && c == targetC)
    || (r == targetR + 1 && c == targetC)
    || (r == targetR && c == targetC + 1)
    || (r == targetR + 1 && c == targetC + 1));
}

// this will spawn the next target on the game randomly
void spawnTarget() {
  // turn off the old target
  targetOff();
  
  targetR = random(0, 7);
  targetC = random(0, 6);

  // make sure it is not the cursor position
  while(inTarget()) {
    targetR = random(0, 7);
    targetC = random(0, 6);
  }

  // turn it on
  targetOn();
}


// shows the game over screen (flashes all)
void gameOver() {
  blank();
  showNum(score);
}

// runs the game
int runGame() {
  
  // update timer and check if we still have time left
  if(!checkTimer()) {
    return -1;
  }
  
  // turn off the last position:
  matrix[r][c] = LOW;
  // read the sensors for X and Y values:
  r = map(analogRead(joyy), 0, 1023, 0, 7);
  c = 6 - map(analogRead(joyx), 0, 1023, 0, 6);

  // if we made it to the target, update score!
  if(inTarget()){
    score++;
    spawnTarget();
  }
  
  // set new position
  matrix[r][c] = HIGH;

  return 0;
}


//===============================================================================
// ARDUINO RUNTIME
//===============================================================================

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  
  // establish direction of pins we are using to drive LEDs 
  pinMode(col1, OUTPUT); 
  pinMode(col2, OUTPUT); 
  pinMode(col3, OUTPUT); 
  pinMode(col4, OUTPUT); 
  pinMode(col5, OUTPUT);
  pinMode(col6, OUTPUT);
  pinMode(col7, OUTPUT);
  pinMode(col8, OUTPUT);
  
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(row3, OUTPUT);
  pinMode(row4, OUTPUT);
  pinMode(row5, OUTPUT);
  pinMode(row6, OUTPUT);
  pinMode(row7, OUTPUT);
  pinMode(row8, OUTPUT);

  pinMode(joyx, INPUT);
  pinMode(joyy, INPUT);

  // these are the starting x and y values of the joystick
  basex = analogRead(joyx);
  basey = analogRead(joyy);

  // blank out the matrix
  blank();

  // start the game timer
//  timerStart();

  // create the first target
  spawnTarget();
  
  // render the matrix
  render();
}


void loop() {
  // put your main code here, to run repeatedly:
  int smileface[8] = {0b0, 0b00000100, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000100, 0x00};
  int sadface[8] = {0b0, 0b00000001, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000001, 0x00};
  
  
  if (runGame() < 0){
    gameOver();
  }
  render();
}
