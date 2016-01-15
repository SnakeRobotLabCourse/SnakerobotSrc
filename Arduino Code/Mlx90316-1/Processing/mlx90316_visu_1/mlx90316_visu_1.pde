/*
 
 *
 * visualize mlx90316 data
 *
 
 */

// import serial library (need to read date from arduino board)
import processing.serial.*;
//import processing.opengl.*;


PFont font;
// The font must be located in the sketch's 
// "data" directory to load successfully


// serial port
Serial port;

// Serial variables
String serialString;
String rxString = null; // Anwort vom Ser Port
int cr = 13; // carriage return
int lf = 10; // line feed


int v1,v2,v3,v1a,wv;

int f_receive;


int cnt,cnt2;
int td,tm,tma,ratems;
int xax;
int s_width=1000;
int s_height=500;
float tfm;
float f1;

color cv1= color(255,0,0);
color cv2= color(0,255,0);
color cv3= color(200,200,255);




// this block is executed one time when programm starts
void setup(){
  font = loadFont("CourierNewPS-BoldMT-48.vlw"); 
  textFont(font); 

  // size of window
//  size(s_width,s_height,OPENGL);
  size(s_width,s_height);

  // print a list of the serial ports, to get port of arduino
  println("Serial ports");
  println(Serial.list());

  // init serial port, on my computer the arduino port is the first port
  port = new Serial(this, Serial.list()[1], 115200);

  frameRate(100);
  background(0);


  
  

  tma=millis(); 


}

// this block is executed in a loop after setup is called one time
void draw(){

    // clear background  
    noStroke();
    fill(0);
    rect(0,0,width,height-350);

    cnt2++;

   f1=v1*0.1;




    fill(255);
    text(cnt, s_width-200, 40);
    
    text(ratems+" ms", s_width-200, 80);
    fill(cv1);
    text("mlx  : "+v1, 20, 40);
    fill(cv2);
    text("angle: "+f1, 20, 80);
    fill(cv3);
    text("speed: "+wv, 20, 120);


    mlx_meter(v1);

    



}

//****************************************************************************
// show mlx90316 position
void mlx_meter(int wi) {

  int cx=450;
  int cy=300;
  
  float ww;
  
  ww=wi* 0.1;
  
  
 
 // noFill();
   fill(0);
  strokeWeight(1);
  stroke(255,10,0);
  ellipseMode(CENTER);
  ellipse(cx, cy, 200, 200);
  ww= 360.0-ww;  // reverse rotation
  ww=ww * PI / 180.0;
  int sx= int(100* sin(ww));
  int sy= int(100* cos(ww));
  strokeWeight(3);
  stroke(255,10,0);
  line(cx,cy,cx+sx,cy+sy);
  strokeWeight(1);
 // stroke(0);
 // 


}

// ***************************************************************
// serial event
void serialEvent(Serial p){

  // get string till line break (ASCII > 13)
  serialString = port.readStringUntil(13);
  // just if there is data
  if (serialString != null) {
   // print(serialString);
    
    tm=millis();  
    if (cnt > 0) {
     
    td= tm-tma;
     tfm=tfm+ td;
    }
    ratems=int(tfm/cnt);
        tma=tm;
    
   
    // try catch function because of possible garbage in received data
    try {
      String[] message = splitTokens(serialString);
      // get current angle
      v1 = Integer.parseInt(message[0]);
 //     v2 = Integer.parseInt(message[1]);
 //     v3 = Integer.parseInt(message[2]);
       wv= abs(v1-v1a);
       v1a=v1;

   


    }
    catch (Exception e){
    }
    cnt=cnt+1;
  }
}





