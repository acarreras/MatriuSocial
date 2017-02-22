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

char estat[64][32];
char newEstat[64][32];

// SCHELLING
const int quantsAgents = 450; // de cada color (total = x2)
const int quantsCanviem = 20;

int quantsHanCanviat = 0;
const int llindarCanvi = 2;
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
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
      if(estat[i][j] == 0){
        matrix.drawPixel(i, j, matrix.Color333(0, 0, 0));
      }
      if(estat[i][j] == 1){
        matrix.drawPixel(i, j, matrix.Color444(15, 7, 1));
      }
      if(estat[i][j] == 2){
        //matrix.drawPixel(i, j, matrix.Color444(7, 15, 15)); // cian
        matrix.drawPixel(i, j, matrix.Color333(7, 7, 7)); // blanc
      }
    }
  }
  
  // wait
  //delay(250);
  //matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
  //delay(150);
  
  // GAME
  quantsHanCanviat = 0;
  schellingStep();
  if((quantsHanCanviat <= llindarCanvi)&&(framecount > 2400)){
    if(contAbansRestart > 100){
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
  framecount++;
}

void resetRandomStates(){
  randomSeed(millis());
  
  for(int i=0; i<numCols; i++){
    for(int j=0; j<numFiles; j++){
      estat[i][j] = 0;
      newEstat[i][j] = 0;
    }
  }

  for(int c=0; c<quantsAgents; c++){
    boolean done = false;
    while(!done){
      int rc = (int)random(0, numCols);
      int rf = (int)random(0, numFiles);
      if(estat[rc][rf] == 0){
        newEstat[rc][rf] = 2;
        estat[rc][rf] = 2; // evitem que els següents ens trepitgin
        done = true;
      }
    }
  }
  for(int c=0; c<quantsAgents; c++){
    boolean done = false;
    while(!done){
      int rc = (int)random(0, numCols);
      int rf = (int)random(0, numFiles);
      if(estat[rc][rf] == 0){
        newEstat[rc][rf] = 1;
        done = true;
      }
    }
  }
}

/* SCHELLING MODEL******************/
// http://nifty.stanford.edu/2014/mccown-schelling-model-segregation/
/***********************************/

void schellingStep(){
  for(int c=0; c<quantsCanviem; c++){
    int cr = random(0, numCols);
    int fr = random(0, numFiles);
    
    if((cr == 0)&&(fr == 0)){
      schellingCornerSupEsq();
    }
    else if((cr == numCols-1)&&(fr == 0)){
      schellingCornerSupDret();
    }
    else if((cr == 0)&&(fr == numFiles-1)){
      schellingCornerInfEsq();
    }
    else if((cr == numCols-1)&&(fr == numFiles-1)){
      schellingCornerInfDret();
    }
    else if(cr == 0){
      schellingColumnaEsq(fr);
    }
    else if(cr == numCols-1){
      schellingColumnaDreta(fr);
    }
    else if(fr == 0){
      schellingFilaSup(cr);
    }
    else if(fr == numFiles-1){
      schellingFilaInf(cr);
    }
    else{
      schellingCenter(cr, fr);
    }
  }
}

int checkNord(int ii, int jj){
  if(estat[ii][jj-1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkSud(int ii, int jj){
  if(estat[ii][jj+1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkOest(int ii, int jj){
  if(estat[ii-1][jj] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkEst(int ii, int jj){
  if(estat[ii+1][jj] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkSudEst(int ii, int jj){
  if(estat[ii+1][jj+1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkNordEst(int ii, int jj){
  if(estat[ii+1][jj-1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkSudOest(int ii, int jj){
  if(estat[ii-1][jj+1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

int checkNordOest(int ii, int jj){
  if(estat[ii-1][jj-1] == estat[ii][jj]){ return 1; }
  else{ return 0; }
}

void schellingNewState(int q, int v, int ii, int jj){
  if(q >= v*0.3){ // happy
    // no ens movem
  }
  else{ // unhappy
    quantsHanCanviat++;
    newEstat[ii][jj] = 0;
    boolean done = false;
    while(!done){
      int rc = (int)random(0, numCols);
      int rf = (int)random(0, numFiles);
      if(estat[rc][rf] == 0){
        newEstat[rc][rf] = estat[ii][jj];
        estat[rc][rf] = estat[ii][jj];
        done = true;
      }
    }
  }
}
      
void schellingCenter(int i, int j){
  int quantscomjoCenter = 0;
  quantscomjoCenter = quantscomjoCenter + checkNord(i,j);
  quantscomjoCenter = quantscomjoCenter + checkSud(i,j);
  quantscomjoCenter = quantscomjoCenter + checkOest(i,j);
  quantscomjoCenter = quantscomjoCenter + checkEst(i,j);
  quantscomjoCenter = quantscomjoCenter + checkNordEst(i,j);
  quantscomjoCenter = quantscomjoCenter + checkSudEst(i,j);
  quantscomjoCenter = quantscomjoCenter + checkSudOest(i,j);
  quantscomjoCenter = quantscomjoCenter + checkNordOest(i,j);
  schellingNewState(quantscomjoCenter, 8, i, j);
}

void schellingFilaSup(int i){
  int quantscomjoFilaSup = 0;
  quantscomjoFilaSup = quantscomjoFilaSup + checkSud(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkOest(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkEst(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkSudOest(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkSudEst(i,0);
  schellingNewState(quantscomjoFilaSup, 5, i,0);    
}

void schellingFilaInf(int i){
  int quantscomjoFilaInf = 0;
  quantscomjoFilaInf = quantscomjoFilaInf + checkNord(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkOest(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkEst(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkNordEst(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkNordOest(i,numFiles-1);
  schellingNewState(quantscomjoFilaInf, 5, i,numFiles-1);
}

void schellingColumnaEsq(int j){
  int quantscomjoColumnaEsq = 0;
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkNord(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkSud(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkEst(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkNordEst(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkSudEst(0,j);
  schellingNewState(quantscomjoColumnaEsq, 5, 0,j);
}

void schellingColumnaDreta(int j){
  int quantscomjoColumnaDreta = 0;
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkNord(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkSud(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkOest(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkSudOest(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkNordOest(numCols-1,j);
  schellingNewState(quantscomjoColumnaDreta, 5, numCols-1,j);  
}

void schellingCornerSupEsq(){
  int quantscomjoCornerSupEsq = 0; // corner sup esq
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + checkSud(0,0);
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + checkEst(0,0);
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + checkSudEst(0,0);
  schellingNewState(quantscomjoCornerSupEsq, 3, 0,0);
}

void schellingCornerSupDret(){
  int quantscomjoCornerSupDret = 0; // corner sup dreta
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + checkSud(numCols-1,0);
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + checkOest(numCols-1,0);
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + checkSudOest(numCols-1,0);
  schellingNewState(quantscomjoCornerSupDret, 3, numCols-1,0);
}

void schellingCornerInfEsq(){
  int quantscomjoCornerInfEsq = 0; // corner inferior esq
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + checkNord(0,numFiles-1);
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + checkEst(0,numFiles-1);
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + checkNordEst(0,numFiles-1);
  schellingNewState(quantscomjoCornerInfEsq, 3, 0,numFiles-1);
}

void schellingCornerInfDret(){
  int quantscomjoCornerInfDret = 0; // corner inferior dreta
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + checkNord(numCols-1,numFiles-1);
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + checkOest(numCols-1,numFiles-1);
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + checkNordOest(numCols-1,numFiles-1);
  schellingNewState(quantscomjoCornerInfDret, 3, numCols-1,numFiles-1);
}
