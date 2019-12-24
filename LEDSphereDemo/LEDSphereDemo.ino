#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>

#include "defines.h"

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_NEOPS, NEOP, NEO_GRB+NEO_KHZ800);
unsigned long frameBuf[BUFSIZE];
unsigned long demo_timeout = 0;
#define DEMO_TIME 8000L

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(IRRCV, INPUT);
  pixel.begin();
  Blynk.begin(auth, ssid, pass); //connect Blynk
  Serial.println(WiFi.localIP());
  ArduinoOTA.begin();
  demo_timeout = millis() + DEMO_TIME;
}

#define NUM_DEMOS 7
byte demoindex = 0;

void loop() {
	ArduinoOTA.handle();
  Blynk.run();

	switch(demoindex) {
	case 0:
		pixel.setBrightness(16);
		demo0();
		break;
		
	case 1:
		pixel.setBrightness(255);
		demo1();
		break;
		
	case 2:
		pixel.setBrightness(32);
		demo2();
		break;
		
	case 3:
		pixel.setBrightness(32);
		demo3();
		break;
		
	case 4:
		pixel.setBrightness(32);
		prepare_demo4();
		displayFrameBuf();
		break;
	
	case 5:
		pixel.setBrightness(32);
		prepare_demo5();
		displayFrameBuf();
		break;
		
	case 6:
		pixel.setBrightness(64);
		prepare_demo6();
		displayFrameBuf();
		break;
	}

	if(millis() > demo_timeout) {
		demo_timeout = millis() + DEMO_TIME;
		demoindex = (demoindex+1)%NUM_DEMOS;
	}
}

void displayFrameBuf() {
  while(digitalRead(IRRCV)==HIGH) {yield();}
  for(int i = 0; i < BUFSIZE; i+=NUM_NEOPS){
  	for(int j=i;j<i+NUM_NEOPS;j++) {
  		pixel.setPixelColor(j-i, frameBuf[j]);
  	}
  	pixel.show();
  }		
}

unsigned long wipecolors[] = {0xFF0000, 0x00FF00, 0x0000FF,
0xFFFF00, 0xFF00FF, 0x00FFFF};
unsigned long d6timeout = 0;
int d6row = 0;
byte d6colindex = 0;
void prepare_demo6() {
	memset(frameBuf, 0, BUFSIZE*sizeof(unsigned long));
	for(byte c=0;c<NUMCOLS;c++) {
		frameBuf[c*NUM_NEOPS+d6row] = wipecolors[d6colindex];
	}

	if(millis() > d6timeout) {
		d6row = (d6row+1)%NUM_NEOPS;
		d6timeout = millis()+10;
		if(d6row==0) {
			d6colindex = (d6colindex+1)%6;
		}
	}	
}


unsigned long d5timeout = 0;
int8_t d5incr = 1;
int d5row = 0, d5col = 0;
byte d5colindex = 0;
void prepare_demo5() {
	memset(frameBuf, 0, BUFSIZE*sizeof(unsigned long));
	int ss, tt;
	for(ss=d5col-3+NUM_NEOPS;ss<=d5col+NUM_NEOPS;ss++) {
		frameBuf[(ss%NUMCOLS)*NUM_NEOPS+d5row] = wipecolors[d5colindex];
		if(d5row>0)
			frameBuf[(ss%NUMCOLS)*NUM_NEOPS+d5row-1] = wipecolors[d5colindex];
	}

	if(millis() > d5timeout) {
		d5row += d5incr;
		if(d5row==NUM_NEOPS) {
			d5row = NUM_NEOPS-1;
			d5incr = -d5incr;
			d5col++;
		} else if(d5row==-1) {
			d5row = 0;
			d5incr = -d5incr;
			d5col++;
		}
		d5timeout = millis()+10;
	}	
}


unsigned long startc = 0;
unsigned long d4timeout = 0;
void prepare_demo4() {
	// prepare buffer
	for(int i=0;i<BUFSIZE;i++) {
		frameBuf[i] = wipecolors[((i/NUM_NEOPS+startc)/11)%6];
	}
	if(millis() > d4timeout) {
		startc++;
		d4timeout = millis()+10;
	}
}

byte wipestop = 0;
byte wipeindex = 0;
unsigned long wipetimeout = 0;
void demo3() {
	while(digitalRead(IRRCV)==HIGH) {yield();}
	for(int i=0;i<=wipestop;i++) {
		pixel.setPixelColor(i, wipecolors[wipeindex]);
	}
	pixel.show();
	if(millis() > wipetimeout) {
		wipestop = (wipestop+1)%NUM_NEOPS;
		if(wipestop==0) wipeindex=(wipeindex+1)%(sizeof(wipecolors)/sizeof(unsigned long));
		wipetimeout = millis() + 50;
	}
}

unsigned long starthue = 0;
void demo2() {
	while(digitalRead(IRRCV)==HIGH) {yield();}
	for(int k=0;k<NUMCOLS;k++) {
		for(int i=0;i<NUM_NEOPS;i++) {
			int hue = starthue + (i*65536L/NUM_NEOPS);
			pixel.setPixelColor(i, pixel.gamma32(pixel.ColorHSV(hue)));
		}
		pixel.show();
	} 
	starthue = (starthue+1024) % 65536L;
}

unsigned long frame_repeat = 3;
void demo1() {
  while(digitalRead(IRRCV)==HIGH) {yield();}
  for(int k=0;k<frame_repeat;k++) {
    for(int i=0;i<NUM_NEOPS-1;i++) {
      pixel.clear();
      pixel.setPixelColor(i, 0xFF0000);
      pixel.show();
    }
    for(int i=NUM_NEOPS-1;i>0;i--) {
      pixel.clear();
      pixel.setPixelColor(i, 0xFF0000);
      pixel.show();
    }
  }
  pixel.clear();
  pixel.show();
}

void demo0() {
	for(int i=0;i<NUM_NEOPS;i++) {
		pixel.setPixelColor(i, 0x0000FF);
		pixel.show();
	}
}
