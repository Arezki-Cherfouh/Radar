import processing.serial.*; 
import java.awt.event.KeyEvent; 
import java.io.IOException;

Serial myPort; 
String angle="";
String distance="";
String data="";
String noObject;
float pixhsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;

void setup() {
 size (1200, 700); // Screen Resolution
 smooth();
 // CHANGE "COM3" to your Arduino Port!
 myPort = new Serial(this,"COM3", 9600); 
 myPort.bufferUntil('.'); 
}

void draw() {
  fill(98,245,31);
  noStroke();
  fill(0,4); 
  rect(0, 0, width, height-height*0.065); 
  
  fill(98,245,31); 
  // Draw the radar lines, circles, and texts
  drawRadar(); 
  drawLine();
  drawObject();
  drawText();
}

void serialEvent (Serial myPort) { 
  data = myPort.readStringUntil('.');
  data = data.substring(0,data.length()-1);
  
  index1 = data.indexOf(","); 
  angle= data.substring(0, index1); 
  distance= data.substring(index1+1, data.length()); 
  
  iAngle = int(angle);
  iDistance = int(distance);
}

// Function to draw the radar grid
void drawRadar() {
  pushMatrix();
  translate(width/2,height-height*0.074); 
  noFill();
  strokeWeight(2);
  stroke(98,245,31);
  // Draws the arc lines
  arc(0,0,(width-width*0.0625),(width-width*0.0625),PI,TWO_PI);
  arc(0,0,(width-width*0.218),(width-width*0.218),PI,TWO_PI);
  arc(0,0,(width-width*0.375),(width-width*0.375),PI,TWO_PI);
  arc(0,0,(width-width*0.531),(width-width*0.531),PI,TWO_PI);
  // Draws the angle lines
  line(-width/2,0,width/2,0);
  line(0,0,(-width/2)*cos(radians(30)),(-width/2)*sin(radians(30)));
  line(0,0,(-width/2)*cos(radians(60)),(-width/2)*sin(radians(60)));
  line(0,0,(-width/2)*cos(radians(90)),(-width/2)*sin(radians(90)));
  line(0,0,(-width/2)*cos(radians(120)),(-width/2)*sin(radians(120)));
  line(0,0,(-width/2)*cos(radians(150)),(-width/2)*sin(radians(150)));
  line(-width/2*cos(radians(30)),0,width/2,0);
  popMatrix();
}

void drawObject() {
  pushMatrix();
  translate(width/2,height-height*0.074);
  strokeWeight(9);
  stroke(255,10,10); // Red color for objects
  pixhsDistance = iDistance*((height-height*0.1666)*0.025); 
  if(iDistance<40){
    line(pixhsDistance*cos(radians(iAngle)),-pixhsDistance*sin(radians(iAngle)),(width-width*0.505)*cos(radians(iAngle)),-(width-width*0.505)*sin(radians(iAngle)));
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(30,250,60);
  translate(width/2,height-height*0.074);
  line(0,0,(height-height*0.12)*cos(radians(iAngle)),-(height-height*0.12)*sin(radians(iAngle)));
  popMatrix();
}

void drawText() {
  // Simple labels for distance and angle
  pushMatrix();
  fill(0,0,0);
  noStroke();
  rect(0, height-height*0.0648, width, height);
  fill(98,245,31);
  textSize(25);
  text("Angle: " + iAngle +" °", width*0.1, height-height*0.027);
  text("Distance: " + iDistance +" cm", width*0.5, height-height*0.027);
  popMatrix();
}