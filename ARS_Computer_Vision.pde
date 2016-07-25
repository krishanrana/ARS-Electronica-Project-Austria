import gab.opencv.*;
import processing.video.*;
import blobDetection.*;
import processing.serial.*;


Serial myPort;
Movie video;
BlobDetection theBlobDetection;
OpenCV opencv;

PImage threshold;
int capturewidth = 1920/3;
int captureheight = 1080/3;
float centroidx, centroidy;

void settings() {

  size(capturewidth*2, captureheight);
}


void setup() {

  String portName = Serial.list()[4];
  println(portName);
  myPort = new Serial(this, portName, 9600);
  video = new Movie(this, "blobtest.mov");
  opencv = new OpenCV(this, 1920, 1080);
  threshold = createImage(1920/3, 1080/3, ARGB);
  video.play();
  video.loop();

  delay(100);
  text("Threshold Image", capturewidth+50, 10);
  
}

void draw() {

  image(video, 0, 0, capturewidth, captureheight); //resizes output video
  opencv.loadImage(video);
  opencv.threshold(90);
  opencv.erode();
  opencv.dilate();
  threshold = opencv.getOutput();
  image(threshold, capturewidth, 0, capturewidth, captureheight);

  noFill();
  theBlobDetection = new BlobDetection(threshold.width, threshold.height);
  theBlobDetection.setPosDiscrimination(false);
  theBlobDetection.setThreshold(0.2f);
  theBlobDetection.computeBlobs(threshold.pixels);
  drawBlobs(true);
}


void drawBlobs(boolean drawBlobs)
{

  noFill();
  Blob b;

  for (int n=0; n<theBlobDetection.getBlobNb(); n++)
  {
    b=theBlobDetection.getBlob(n);
    if (b!=null)
    {

      // Blobs
      if (drawBlobs)
      {
        if ((b.w*capturewidth * b.h*captureheight)>80)
        {

          centroidx = ((b.xMin*capturewidth)+(b.w*capturewidth*0.5));
          centroidy = ((b.yMin*captureheight)+(b.h*captureheight*0.5));

          strokeWeight(1);
          stroke(255, 0, 0);
          rect(
            b.xMin*capturewidth + capturewidth, b.yMin*captureheight, 
            b.w*capturewidth, b.h*captureheight
            );

          stroke(0, 255, 0);
          ellipse(centroidx + capturewidth, centroidy, 10, 10);
        }
      }
    }
  }
}

void movieEvent(Movie m) {
  m.read();
}