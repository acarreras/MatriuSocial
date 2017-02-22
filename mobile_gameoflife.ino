// For 32x64 RGB LED matrix.
// NOTE THIS CAN ONLY BE USED ON A MEGA! NOT ENOUGH RAM ON UNO!

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

const int numCols = 64;
const int numFiles = 32;
const int totalPunts = 2048;

boolean estat[64][32];
boolean newEstat[64][32];

// ON/OFF
int filaToChange = 0;

// GAME OF LIFE
int quantsHanCanviat = 0;
const int llindarCanvi = 32;
int contAbansRestart = 0;

int framecount = 0;

void setup(){
  matrix.begin();
  
  // draw a pixel in solid blac
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
  resetRandomStates();
}

void loop(){
  // actualitza estat
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
      estat[i][j] = newEstat[i][j];
    }
  }
  
  // dibuixa
  for(int c=0; c<totalPunts; c++){
    int i = c/numFiles;
    int j = c%numFiles;
    if(estat[i][j] == true){
      //matrix.drawPixel(i, j, matrix.Color444(14, 10, 11)); // blanquet lilos
      matrix.drawPixel(i, j, matrix.Color333(7, 7, 7)); // blanc
    }
    else{
      matrix.drawPixel(i, j, matrix.Color333(0, 0, 0)); 
    }
  }
  /*
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
      if(estat[i][j] == true){
        //matrix.drawPixel(i, j, matrix.Color444(14, 10, 11)); // blanquet lilos
        matrix.drawPixel(i, j, matrix.Color333(7, 7, 7)); // blanc
      }
      else{
        matrix.drawPixel(i, j, matrix.Color333(0, 0, 0)); 
      }
    }
  }
  */
  /*
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
        matrix.drawPixel(random(0,64), random(0,32), matrix.Color333(7, 7, 7)); 
    }
  }
  */
  
  // wait
  delay(250);
  
  //matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
  //delay(150);
  
  // GAME
  quantsHanCanviat = 0;
  gameoflifeStep();
  if(quantsHanCanviat <= llindarCanvi){
    if(contAbansRestart > 40){
      contAbansRestart = 0;
      framecount = 0;
      resetRandomStates();
      matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
      delay(150);
    }
    else{
      contAbansRestart++;
    }
  }
  if(framecount > 600){ // portem massa en aquest loop
    contAbansRestart = 0;
    framecount = 0;
    resetRandomStates();
    matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
    delay(150);
  }
  framecount++;
}

void resetRandomStates(){
  randomSeed(millis());
  
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
      int r = random(0,2);
      boolean b = false;
      if(r < 1){
        b = true;
      }
      newEstat[i][j] = b;
    }
  }
}

void filaOnOff(int numFila, boolean on){
  for(int i=0; i<numCols; i++){
    newEstat[i][numFila] = on;
  }  
}

/* GAME OF LIFE ********************/
/***********************************/

void gameoflifeStep(){
  gameoflifeCornerSupEsq();
  gameoflifeCornerSupDret();
  gameoflifeCornerInfEsq();
  gameoflifeCornerInfDret();
  
  for(int fr=1; fr<numFiles-1; fr++){
    gameoflifeColumnaEsq(fr);
    gameoflifeColumnaDreta(fr);  
  }
  for(int cr=1; cr<numCols-1; cr++){
    gameoflifeFilaSup(cr);
    gameoflifeFilaInf(cr);
  }
      
  for(int cr=1; cr<numCols-1; cr++){
    for(int fr=1; fr<numFiles-1; fr++){
      gameoflifeCenter(cr, fr);
    }
  }
}

int checkNordAlife(int ii, int jj){
  if(estat[ii][jj-1] == true){ return 1; }
  else{ return 0; }
}

int checkSudAlife(int ii, int jj){
  if(estat[ii][jj+1] == true){ return 1; }
  else{ return 0; }
}

int checkOestAlife(int ii, int jj){
  if(estat[ii-1][jj] == true){ return 1; }
  else{ return 0; }
}

int checkEstAlife(int ii, int jj){
  if(estat[ii+1][jj] == true){ return 1; }
  else{ return 0; }
}

int checkSudEstAlife(int ii, int jj){
  if(estat[ii+1][jj+1] == true){ return 1; }
  else{ return 0; }
}

int checkNordEstAlife(int ii, int jj){
  if(estat[ii+1][jj-1] == true){ return 1; }
  else{ return 0; }
}

int checkSudOestAlife(int ii, int jj){
  if(estat[ii-1][jj+1] == true){ return 1; }
  else{ return 0; }
}

int checkNordOestAlife(int ii, int jj){
  if(estat[ii-1][jj-1] == true){ return 1; }
  else{ return 0; }
}

void gameoflifeNewState(int q, int ii, int jj){
  if(estat[ii][jj] == true){ // viva
    if(q == 2 || q == 3){
      newEstat[ii][jj] = true; // seguim viva
    }
    else{
      newEstat[ii][jj] = false; // morim
      quantsHanCanviat++;
    }
  }
  else{ // morta
    if(q == 3){
      newEstat[ii][jj] = true; // revivim
      quantsHanCanviat++;
    }
    else{
      newEstat[ii][jj] = false; // seguim morts
    }
  }
}
      
void gameoflifeCenter(int i, int j){
  int quantsalifeCenter = 0;
  quantsalifeCenter = quantsalifeCenter + checkNordAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkSudAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkOestAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkEstAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkNordEstAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkSudEstAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkSudOestAlife(i,j);
  quantsalifeCenter = quantsalifeCenter + checkNordOestAlife(i,j);
  gameoflifeNewState(quantsalifeCenter, i, j);
}

void gameoflifeFilaSup(int i){
  int quantsalifeFilaSup = 0;
  quantsalifeFilaSup = quantsalifeFilaSup + checkSudAlife(i,0);
  quantsalifeFilaSup = quantsalifeFilaSup + checkOestAlife(i,0);
  quantsalifeFilaSup = quantsalifeFilaSup + checkEstAlife(i,0);
  quantsalifeFilaSup = quantsalifeFilaSup + checkSudOestAlife(i,0);
  quantsalifeFilaSup = quantsalifeFilaSup + checkSudEstAlife(i,0);
  gameoflifeNewState(quantsalifeFilaSup, i,0);    
}

void gameoflifeFilaInf(int i){
  int quantsalifeFilaInf = 0;
  quantsalifeFilaInf = quantsalifeFilaInf + checkNordAlife(i,numFiles-1);
  quantsalifeFilaInf = quantsalifeFilaInf + checkOestAlife(i,numFiles-1);
  quantsalifeFilaInf = quantsalifeFilaInf + checkEstAlife(i,numFiles-1);
  quantsalifeFilaInf = quantsalifeFilaInf + checkNordEstAlife(i,numFiles-1);
  quantsalifeFilaInf = quantsalifeFilaInf + checkNordOestAlife(i,numFiles-1);
  gameoflifeNewState(quantsalifeFilaInf, i,numFiles-1);
}

void gameoflifeColumnaEsq(int j){
  int quantsalifeColumnaEsq = 0;
  quantsalifeColumnaEsq = quantsalifeColumnaEsq + checkNordAlife(0,j);
  quantsalifeColumnaEsq = quantsalifeColumnaEsq + checkSudAlife(0,j);
  quantsalifeColumnaEsq = quantsalifeColumnaEsq + checkEstAlife(0,j);
  quantsalifeColumnaEsq = quantsalifeColumnaEsq + checkNordEstAlife(0,j);
  quantsalifeColumnaEsq = quantsalifeColumnaEsq + checkSudEstAlife(0,j);
  gameoflifeNewState(quantsalifeColumnaEsq, 0,j);
}

void gameoflifeColumnaDreta(int j){
  int quantsalifeColumnaDreta = 0;
  quantsalifeColumnaDreta = quantsalifeColumnaDreta + checkNordAlife(numCols-1,j);
  quantsalifeColumnaDreta = quantsalifeColumnaDreta + checkSudAlife(numCols-1,j);
  quantsalifeColumnaDreta = quantsalifeColumnaDreta + checkOestAlife(numCols-1,j);
  quantsalifeColumnaDreta = quantsalifeColumnaDreta + checkSudOestAlife(numCols-1,j);
  quantsalifeColumnaDreta = quantsalifeColumnaDreta + checkNordOestAlife(numCols-1,j);
  gameoflifeNewState(quantsalifeColumnaDreta, numCols-1,j);  
}

void gameoflifeCornerSupEsq(){
  int quantsalifeCornerSupEsq = 0; // corner sup esq
  quantsalifeCornerSupEsq = quantsalifeCornerSupEsq + checkSudAlife(0,0);
  quantsalifeCornerSupEsq = quantsalifeCornerSupEsq + checkEstAlife(0,0);
  quantsalifeCornerSupEsq = quantsalifeCornerSupEsq + checkSudEstAlife(0,0);
  gameoflifeNewState(quantsalifeCornerSupEsq, 0,0);
}

void gameoflifeCornerSupDret(){
  int quantsalifeCornerSupDret = 0; // corner sup dreta
  quantsalifeCornerSupDret = quantsalifeCornerSupDret + checkSudAlife(numCols-1,0);
  quantsalifeCornerSupDret = quantsalifeCornerSupDret + checkOestAlife(numCols-1,0);
  quantsalifeCornerSupDret = quantsalifeCornerSupDret + checkSudOestAlife(numCols-1,0);
  gameoflifeNewState(quantsalifeCornerSupDret, numCols-1,0);
}

void gameoflifeCornerInfEsq(){
  int quantsalifeCornerInfEsq = 0; // corner inferior esq
  quantsalifeCornerInfEsq = quantsalifeCornerInfEsq + checkNordAlife(0,numFiles-1);
  quantsalifeCornerInfEsq = quantsalifeCornerInfEsq + checkEstAlife(0,numFiles-1);
  quantsalifeCornerInfEsq = quantsalifeCornerInfEsq + checkNordEstAlife(0,numFiles-1);
  gameoflifeNewState(quantsalifeCornerInfEsq, 0,numFiles-1);
}

void gameoflifeCornerInfDret(){
  int quantsalifeCornerInfDret = 0; // corner inferior dreta
  quantsalifeCornerInfDret = quantsalifeCornerInfDret + checkNordAlife(numCols-1,numFiles-1);
  quantsalifeCornerInfDret = quantsalifeCornerInfDret + checkOestAlife(numCols-1,numFiles-1);
  quantsalifeCornerInfDret = quantsalifeCornerInfDret + checkNordOestAlife(numCols-1,numFiles-1);
  gameoflifeNewState(quantsalifeCornerInfDret, numCols-1,numFiles-1);
}
