void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

////function to begin calibrating the angle sensor
////while calibrating no other commands can be processed
//  void startCalibrating(){
//    calibrating = true;
//    minAngle = 9000;
//    maxAngle = 9000;
//  }

//void calibrate(){
//  ii = mlx_1.readAngle();
//  Serial.println(ii);
//  if(calibratingMax){//calibrate on of the sides
//    if(maxAngle > ii){
//      calibratingDiff = maxAngle -ii;
//    } else{
//       calibratingDiff = ii -maxAngle;
//    }
//    if(ii>0){
//      maxAngle = ii;
//    }
//    
//    if(calibratingDiff < 1 || ii ==-1){//if difference to last value is smalle then 1 degree, we assume we found the value
//      calibratingMax = false;//continue with the other
//      myservo.writeMicroseconds(MAXMS1);//switch direction servo
//      ii= -1;
//    } else{
//      myservo.writeMicroseconds(MINMS1);//keep turning
//    }
//  } else{//calibrate the other of the sides
//    if(minAngle > ii){
//      calibratingDiff = minAngle -ii;
//    } else{
//      calibratingDiff = ii -minAngle;
//    }
//    if(ii>0){
//      minAngle = ii;
//    }
//      
//    
//    //remember last read value
//    if(calibratingDiff < 1 || ii ==-1){//if difference to last value is smalle then 1 degree, we assume we found the value
//      calibratingMax = true;
//      calibratingFinished = true;//we are done calibrating
//      myservo.writeMicroseconds(MIDMS);//stop servo
//      Serial.print("minangle =");
//      Serial.println(minAngle);
//      Serial.print("maxangle =");
//      Serial.println(maxAngle);
//    } else{
//      myservo.writeMicroseconds(MAXMS1);//keep turning
//    }
//  }
//}


// old version
//void updateAngle(){
//  
//    Serial.println("updateAngle func");
//    ii = mlx_1.readAngle();// read the given angle from sensor
//  
//   Serial.println(ii);
//   if(ii<0){
//    Serial.println("stopping servo cause angle sensor not working");
//     myservo.writeMicroseconds(MIDMS);
//     return;
//   }
// if(targetAngle > CENTERANGLE+850 || targetAngle < CENTERANGLE-850){
//    Serial.println("stopping servo");
//     myservo.writeMicroseconds(MIDMS);
//     return;
//  }
//
//
//  
//  if(ii < targetAngle){ //if the angle is between 0 and the target angle
//    iiX = ii+3600;
//
//    curDiffLeft = iiX -targetAngle; //messure distance when turning away from targetangle going over 0
//    curDiffRight = targetAngle -ii; //messure distance when turning towardds targetangle
//    
//  } else {//(ii > targetAngle)  //if the angle is between 360 and the target angle
//    iiX = ii-3600;
//
//    curDiffLeft = ii -targetAngle;  //messure distance when turning towardds targetangle
//    curDiffRight = targetAngle -iiX;   //messure distance when turning away from targetangle going over 0
//  } 
//  if(curDiffRight > curDiffLeft){
//    if(curDiffLeft > ACCURRACY3){
//      myservo.writeMicroseconds(MAXMS1);
//    } else if(curDiffLeft > ACCURRACY2){
//      myservo.writeMicroseconds(MAXMS1);
//    } else if(curDiffLeft > ACCURRACY1){
//      myservo.writeMicroseconds(MAXMS1);
//    } else{
//      
//       myservo.writeMicroseconds(MIDMS);
//    }
//    
//  } else {//if(curDiffLeft >= curDiffRight){
//    if(curDiffRight > ACCURRACY3){
//      myservo.writeMicroseconds(MINMS1);
//    } else if(curDiffRight > ACCURRACY2){
//      myservo.writeMicroseconds(MINMS1);
//    } else if(curDiffRight > ACCURRACY1){
//      myservo.writeMicroseconds(MINMS1);
//    } else{
//       myservo.writeMicroseconds(MIDMS);
//    }
//  }
//}
