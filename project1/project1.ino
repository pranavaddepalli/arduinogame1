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
//  startGame();
    
  // render the matrix
  render();
}


void loop() {
  // put your main code here, to run repeatedly:
  int smileface[8] = {0b0, 0b00000100, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000100, 0x00};
  int sadface[8] = {0b0, 0b00000001, 0b01110010, 0b00000010, 0b00000010, 0b01110010, 0b00000001, 0x00};
  
  
  render();
}
