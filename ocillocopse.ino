//<App !Start!>
// FILE: [ocillocopse.ino]
// Created by GUIslice Builder version: [0.17.b27]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "ocillocopse_GSLC.h"
#include "math.h"
#include <TFT_eSPI.h>
#include "Arduino.h"
#include "avdweb_Switch.h"
TFT_eSPI tft = TFT_eSPI();
#include <TFT_eWidget.h>               // Widget library
#include <TrueRMS.h>
#include <digitalWriteFast.h> // It uses digitalWriteFast only for the purpose of debugging!
                              // https://code.google.com/archive/p/digitalwritefast/downloads

#include <MCP3XXX.h>

MCP3008 adc;

GraphWidget gr = GraphWidget(&tft);    // Graph widget gr instance with pointer to tft
TraceWidget tr = TraceWidget(&gr);     // Graph trace tr with pointer to gr

const float gxLow  = 0.0;
const float gxHigh = 0.5;
const float gyLow  = -1000;
const float gyHigh = 1000;
int i = 0; 
int trang = 1;
int cnt = 0;
int value0;
int value1[100];
int value2[100];
unsigned long nextLoop;
int adcVal;
int adcVal1;
int ap;
int dong;
float VoltRange = 3.3; // The full scale value is set to 5.00 Volts but can be changed when using an


Rms readRms; // create an instance of Rms.
float Uef;
float Ief;
float PVA;
float PW;
float PowerFactor;
float kV = 2.6022;
float kI = 0.0002;
float Wh = 0;
unsigned long previousComputeMillis;
float Hz = 50;        // sample rate
float dt = 1000000/Hz; // sample time in microsecond
long loop_time = 0;
// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_BTN1:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG4);
          trang = 4;
        break;
      case E_ELEM_BTN2:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG2);
          trang = 2;      
        break;
      case E_ELEM_BTN3:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG_MAIN);
          trang = 1;
        break;
      case E_ELEM_BTN4:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG3);
          trang = 3;
        break;
      case E_ELEM_BTN5:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG2);
          trang = 2;
        break;
      case E_ELEM_BTN6:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG4);
          trang = 4;
        break;
      case E_ELEM_BTN7:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG3);
          trang = 3; 
        break;
      case E_ELEM_BTN8:
          tft.fillScreen(TFT_GREEN );
          gslc_SetPageCur(&m_gui,E_PG_MAIN);
          trang = 1; 
        break;

//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  previousComputeMillis = millis();
  adc.begin(5, 13, 12, 14);
  Serial.begin(115200);
  pinMode(25, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  pinMode(33, INPUT);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_GREEN );

  // Graph area is 200 pixels wide, 150 pixels high, dark grey background
  gr.createGraph(200, 150, tft.color565(5, 5, 5));

  // x scale units is from 0 to 100, y scale units is -512 to 512
  gr.setGraphScale(gxLow, gxHigh, gyLow, gyHigh);

  // X grid starts at 0 with lines every 20 x-scale units
  // Y grid starts at -512 with lines every 64 y-scale units
  // blue grid
  gr.setGraphGrid(gxLow, 20.0, gyLow, 64.0, TFT_BLUE);

  // Draw empty graph, top left corner at pixel coordinate 40,10 on TFT

  gr.drawGraph(20, 150);

  // Start a trace with using red, trace points are in x and y scale units
  // In this example a horizontal line is drawn
  tr.startTrace(TFT_RED);
  // Add a trace point at 0.0,0.0 on graph
  tr.addPoint(0.0, 0.0);
  // Add another point at 100.0, 0.0 this will be joined via line to the last point added
  tr.addPoint(100.0, 0.0);

  // Start a new trace with using white
  tr.startTrace(TFT_WHITE);
  gslc_InitDebug(&DebugOut);

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();
 

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{
  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  if (millis() - previousComputeMillis >= 2000) {
    previousComputeMillis = millis();
    MeasurePower();
    ComputePower();
  }
    dong = adc.analogRead(1)-adc.analogRead(0); 
    ap = adc.analogRead(3)-adc.analogRead(0);
    Serial.println(ap);
    gslc_Update(&m_gui); 
    if (trang == 1){
      tft.setCursor(135, 121, 2);
      tft.println(Ief);   
      static uint32_t plotTime = millis();
      static float gx = 0.0, gy = 0.0;
      static float delta = 10.0;
       // Create a new plot point every 100ms
      if (millis() - plotTime >= 0.0001) {
        plotTime = millis();

       // Add a plot, first point in a trace will be a single pixel (if within graph area)
         tr.addPoint(gx, gy);
          gx += 0.01;
          gy = dong;

         // If the end of the graph x ais is reached start a new trace at 0.0,0.0
          if (gx > gxHigh) {
          gx = 0.0;
          gy = dong;

        gr.drawGraph(20, 150);
  
        tr.startTrace(TFT_GREEN);


}}}
    if (trang == 2){

      tft.setCursor(135, 121, 2);
      tft.println(Uef);
      static uint32_t plotTime = millis();
      static float gx = 0.0, gy = 0.0;
      static float delta = 10.0;
       // Create a new plot point every 100ms
      if (millis() - plotTime >= 0.0001) {
        plotTime = millis();

       // Add a plot, first point in a trace will be a single pixel (if within graph area)
         tr.addPoint(gx, gy);
          gx += 0.01;
          gy = ap;

         // If the end of the graph x ais is reached start a new trace at 0.0,0.0
          if (gx > gxHigh) {
          gx = 0.0;
          gy = ap;

        gr.drawGraph(20, 150);
  
        tr.startTrace(TFT_GREEN);

}}}
}
void MeasurePower() {
  int i;
  value0 = adc.analogRead(0);
  unsigned long MeasureMillis = millis();
  unsigned long T= micros();
  while (millis() - MeasureMillis < 21) { //Read values during 20ms
    i = (micros() % 20000) / 200;
    value1[i] = adc.analogRead(3) - adc.analogRead(0);
    value2[i] = adc.analogRead(1) - adc.analogRead(0);
}}
void ComputePower() {
  float V;
  float I;
  Uef = 0;
  Ief = 0;
  PW = 0;
  for (int i = 0; i < 100; i++) {
    V = kV * float(value1[i]);
    Uef += sq(V);
    I = kI * float(value2[i]);
    Ief += sq(I );
    PW += V * I;
  }
  Uef = sqrt(Uef / 100) ;
  Ief = sqrt(Ief / 100) ;
  PW = floor(PW / 100);
  PVA = floor(Uef * Ief);
  PowerFactor = floor(100 * PW / PVA) / 100;
  Wh += PW / 360; //Every 10s
} 

  
