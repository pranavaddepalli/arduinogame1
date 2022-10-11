/*
 * 2040-shape
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

// these are mutex locks for the directions so we only send one signal
int joyxlock = false;
int joyylock = false;

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

// this returns a string representing what direction the joystick is in
// return values are 'none', 'right', 'left', 'up', or 'down'
String joydir() {
  String dir = "none";
  
  int currx = analogRead(joyx);
  int curry = analogRead(joyy);
  
  if (currx == 1023 && !joyxlock) {
    joyxlock = true;
    dir = "left";
  }
  if (currx == 0 && !joyxlock) {
    joyxlock = true;
    dir = "right";
  }
  if (curry == 1023 && !joyylock) {
    joyylock = true;
    dir = "down";
  }
  if (curry == 0 && !joyylock) {
    joyylock = true;
    dir = "up";
  }

  // reset lock if it gets back
  Serial.print("currs");
  Serial.println(currx);
  Serial.println(basex + joybuffer);
  
  if (currx == basex + joybuffer || currx == basex - joybuffer){
    joyxlock = false;
  }
  if (curry == basey + joybuffer || curry == basey - joybuffer){
    joyylock = false;
  }
  Serial.println(joyxlock);
  Serial.println(joyylock);
  delay(1000);
  return dir;
}


//===============================================================================
// CORE GAME LEVEL CODE
//===============================================================================

/* a box is a 2-by-2 square on the matrix specified by its top left row and column
 *  boxes have five types: 0 (empty), 1, 2, 3, and 4, for the number of pixels, filled from the top left
 */
struct box {
  int row;
  int col;
  int type;
};

// this is the grid for the game
box gameGrid[4][4];

// this keeps track of what turn the user is on
int turn = 0;

// transforms the game grid to the LED matrix
void showGame(){
  for(int boxr = 0; boxr < 4; boxr++) {
    for(int boxc = 0; boxc < 4; boxc++) {
      box a = gameGrid[boxr][boxc];
      
      int aMatrixRow = a.row * 2;
      int aMatrixCol = a.col * 2;
      
      if(a.type == 0){
        // all four LEDs need to be OFF
        matrix[aMatrixRow][aMatrixCol] = LOW;
        matrix[aMatrixRow + 1][aMatrixCol] = LOW;
        matrix[aMatrixRow][aMatrixCol + 1] = LOW;
        matrix[aMatrixRow + 1][aMatrixCol + 1] = LOW;
      }
      else if(a.type == 1){
        // top left is ON
        matrix[aMatrixRow][aMatrixCol] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol] = LOW;
        matrix[aMatrixRow][aMatrixCol + 1] = LOW;
        matrix[aMatrixRow + 1][aMatrixCol + 1] = LOW;
      }
      else if(a.type == 2){
        // top is ON
        matrix[aMatrixRow][aMatrixCol] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol] = LOW;
        matrix[aMatrixRow][aMatrixCol + 1] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol + 1] = LOW;
      }
      else if(a.type == 3){
        // top is ON, bottom left is ON
        matrix[aMatrixRow][aMatrixCol] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol] = HIGH;
        matrix[aMatrixRow][aMatrixCol + 1] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol + 1] = LOW;
      }
      else if(a.type == 4){
        // all ON
        matrix[aMatrixRow][aMatrixCol] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol] = HIGH;
        matrix[aMatrixRow][aMatrixCol + 1] = HIGH;
        matrix[aMatrixRow + 1][aMatrixCol + 1] = HIGH;
      }
    }
  }
}

// resets the game to an empty screen
void resetGame(){
  for(int boxr = 0; boxr < 4; boxr++) {
    for(int boxc = 0; boxc < 4; boxc++) {
      // set all boxes to empty
      gameGrid[boxr][boxc].type = 0;
    }
  }

  showGame();
}

// starts the game with an empty screen
void startGame(){
  for(int boxr = 0; boxr < 4; boxr++) {
    for(int boxc = 0; boxc < 4; boxc++) {
      // initialize box rows, cols, and type as empty
      box a = {boxr, boxc, 0};
      gameGrid[boxr][boxc] = a;
    }
  }

  showGame();
}

// checks if the game is full (no more boxes left)
bool gameFull(){
  for(int boxr = 0; boxr < 4; boxr++) {
    for(int boxc = 0; boxc < 4; boxc++) {
      if(gameGrid[boxr][boxc].type == 0){
        return false;
      }
    }
  }
  return true;
}

// spawns a box at a randomly chosen spot of randomly chosen size in the gamegrid
void spawnBox(){

  box next = {(int)random(0, 5), (int)random(0, 5), (int)random(1, 5)};
  int i = 0;
  // repeat until we find a free spot
  while(gameGrid[next.row][next.col].type != 0){
    // taken, so recalculate row and col
    next.row = random(0, 5);
    next.col = random(0, 5);
    next.type = random(1, 5);
  }
  // we found an empty box so we can place it there
  gameGrid[next.row][next.col] = next;

}

// advances the game by a turn based on what direction the user moved the joystick
void advanceGame(String dir){
  // first check if the game is full
  if(gameFull()){
    Serial.println("Game over");
    return;
  }
  turn++;
  Serial.print("turn"); Serial.println(turn);

  // spawn a box
  
  
  showGame(); 
}


// monitors user input and runs the game based on inputs
void runGame(){
  advanceGame(joydir());
  Serial.println(joydir());
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

  // setup the game
  startGame();
    
  // render the matrix
  render();
}


void loop() {
  // put your main code here, to run repeatedly:
  int smileface[8] = {0b0, 0b00000100, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000100, 0x00};
  int sadface[8] = {0b0, 0b00000001, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000001, 0x00};

//  Serial.println(turn);
  runGame();
  
  render();
}
