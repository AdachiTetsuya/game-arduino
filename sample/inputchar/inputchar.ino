
#include <ResKeypad.h> // library for keypad

// variables
int ain=A4;

ResKeypad keypad(ain,16,RESKEYPAD_4X4,RESKEYPAD_4X4_SIDE_A); 
// functions
void setup()
{
  Serial.begin(9600); // initialize serial port at 9600bps.
  while(!Serial); // wait for serial port to connect.
  Serial.println("Press keys.");
} // setup

void loop()
{
  signed char key;

  key=keypad.GetKey();
  if(key>=0) {
    Serial.println(key);
  } // if
} // loop
