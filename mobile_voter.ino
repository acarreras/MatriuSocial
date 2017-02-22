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
const int totalPunts = numCols*numFiles;

boolean estat[64][32];
boolean newEstat[64][32];

// ON/OFF
int filaToChange = 0;

// VOTER
const int alpha = 31; // han d'estar hardcoded per a les comparacions
const int betha = 10; // han d'estar hardcoded per a les comparacions

const int quantsCanviem = 20;

int quantsHanCanviat = 0;
const int llindarCanvi = 1;
int contAbansRestart = 0;

int framecount = 0;

void setup(){
  matrix.begin();
  
  // draw a pixel in solid black
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
      if(estat[i][j] == true){
        //matrix.drawPixel(i, j, matrix.Color444(15, 15, 7)); // groguet
        matrix.drawPixel(i, j, matrix.Color333(7, 7, 7)); // blanc
      }
      else{
        matrix.drawPixel(i, j, matrix.Color333(0, 0, 0));
      }
    }
  }
    
  // wait
  //delay(100);
  //matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
  //delay(150);
  
  // GAME
  quantsHanCanviat = 0;
  voterStep();
  if((quantsHanCanviat <= llindarCanvi)&&(framecount > 5500)){
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
      int r = random(0,10);
      boolean b = false;
      if(r < 4){
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

/* VOTER MODEL *********************/
// https://en.wikipedia.org/wiki/Contact_process_%28mathematics%29#Voter_model
// https://www.math.utah.edu/~rbutler/votermodel/index.html
// https://services.math.duke.edu/~rtd/survey/survc5.html
/***********************************/

void voterStep(){
  for(int c=0; c<quantsCanviem; c++){
    int cr = random(0, numCols);
    int fr = random(0, numFiles);
    
    if((cr == 0)&&(fr == 0)){
      voterCornerSupEsq();
    }
    else if((cr == numCols-1)&&(fr == 0)){
      voterCornerSupDret();
    }
    else if((cr == 0)&&(fr == numFiles-1)){
      voterCornerInfEsq();
    }
    else if((cr == numCols-1)&&(fr == numFiles-1)){
      voterCornerInfDret();
    }
    else if(cr == 0){
      voterColumnaEsq(fr);
    }
    else if(cr == numCols-1){
      voterColumnaDreta(fr);
    }
    else if(fr == 0){
      voterFilaSup(cr);
    }
    else if(fr == numFiles-1){
      voterFilaInf(cr);
    }
    else{
      voterCenter(cr, fr);
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

void voterNewState(int q, int ii, int jj){
  if(q == 0){ // canviem d'opinio
    newEstat[ii][jj] = !estat[ii][jj];
    quantsHanCanviat++;
  }
  if(q == 5){ // mantenim opinio
    newEstat[ii][jj] = estat[ii][jj];
  }
  if(q == 1){
    int ra = random(0,100);
    if(ra >= 31){ // segurament canviem d'opinio
      newEstat[ii][jj] = !estat[ii][jj];
      quantsHanCanviat++;
    }
  }
  if(q == 4){
    int raa = random(0,100);
    if(raa < 31){ // poc probable que canviem d'opinio
      newEstat[ii][jj] = !estat[ii][jj];
      quantsHanCanviat++;
    }
  }
  if(q == 2){
    int rb = random(0,100);
    if(rb >= 10){ // segurament canviem d'opinio
      newEstat[ii][jj] = !estat[ii][jj];
      quantsHanCanviat++;
    }
  }
  if(q == 3){
    int rbb = random(0,100);
    if(rbb < 10){ // poc probable que canviem d'opinio
      newEstat[ii][jj] = !estat[ii][jj];
      quantsHanCanviat++;
    }
  }
}
      
void voterCenter(int i, int j){
  int quantscomjoCenter = 0;
  quantscomjoCenter = quantscomjoCenter + checkNord(i,j);
  quantscomjoCenter = quantscomjoCenter + checkSud(i,j);
  quantscomjoCenter = quantscomjoCenter + checkOest(i,j);
  quantscomjoCenter = quantscomjoCenter + checkEst(i,j);
  quantscomjoCenter = quantscomjoCenter + 1; // jo
  voterNewState(quantscomjoCenter, i, j);
}

void voterFilaSup(int i){
  int quantscomjoFilaSup = 0;
  quantscomjoFilaSup = quantscomjoFilaSup + checkSud(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkOest(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + checkEst(i,0);
  quantscomjoFilaSup = quantscomjoFilaSup + 1;
  voterNewState(quantscomjoFilaSup, i,0);    
}

void voterFilaInf(int i){
  int quantscomjoFilaInf = 0;
  quantscomjoFilaInf = quantscomjoFilaInf + checkNord(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkOest(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + checkEst(i,numFiles-1);
  quantscomjoFilaInf = quantscomjoFilaInf + 1;
  voterNewState(quantscomjoFilaInf, i,numFiles-1);
}

void voterColumnaEsq(int j){
  int quantscomjoColumnaEsq = 0;
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkNord(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkSud(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + checkEst(0,j);
  quantscomjoColumnaEsq = quantscomjoColumnaEsq + 1;
  voterNewState(quantscomjoColumnaEsq, 0,j);
}

void voterColumnaDreta(int j){
  int quantscomjoColumnaDreta = 0;
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkNord(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkSud(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + checkOest(numCols-1,j);
  quantscomjoColumnaDreta = quantscomjoColumnaDreta + 1;
  voterNewState(quantscomjoColumnaDreta, numCols-1,j);  
}

void voterCornerSupEsq(){
  int quantscomjoCornerSupEsq = 0; // corner sup esq
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + checkSud(0,0);
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + checkEst(0,0);
  quantscomjoCornerSupEsq = quantscomjoCornerSupEsq + 1;
  voterNewState(quantscomjoCornerSupEsq, 0,0);
}

void voterCornerSupDret(){
  int quantscomjoCornerSupDret = 0; // corner sup dreta
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + checkSud(numCols-1,0);
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + checkOest(numCols-1,0);
  quantscomjoCornerSupDret = quantscomjoCornerSupDret + 1;
  voterNewState(quantscomjoCornerSupDret, numCols-1,0);
}

void voterCornerInfEsq(){
  int quantscomjoCornerInfEsq = 0; // corner inferior esq
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + checkNord(0,numFiles-1);
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + checkEst(0,numFiles-1);
  quantscomjoCornerInfEsq = quantscomjoCornerInfEsq + 1;
  voterNewState(quantscomjoCornerInfEsq, 0,numFiles-1);
}

void voterCornerInfDret(){
  int quantscomjoCornerInfDret = 0; // corner inferior dreta
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + checkNord(numCols-1,numFiles-1);
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + checkOest(numCols-1,numFiles-1);
  quantscomjoCornerInfDret = quantscomjoCornerInfDret + 1;
  voterNewState(quantscomjoCornerInfDret, numCols-1,numFiles-1);
}
