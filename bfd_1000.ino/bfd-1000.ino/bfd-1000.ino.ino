/* YourDuino.com Example Software Sketch
 7-Way Robot Line Tracking Sensor Test
 http://arduino-info.wikispaces.com/Robot+7-wayMultiTrackingSensor
 terry@yourduino.com */

/*-----( Import needed libraries )-----*/

/*-----( Declare Constants )-----*/

/*-(Servo controls for Robot .. later)-*/
#define MOTOR_LEFT 9  /*-(servo for Left wheel)-*/
#define MOTOR_RIGHT 10 /*-(servo for Right wheel)-*/
#define LEFT_ZERO_TRIM  0  
#define RIGHT_ZERO_TRIM 2;  

/*-(Connections to NewWay 7-Way Robot Line Follower Sensor)-*/
#define SS1_LEFT_OUT  2 
#define SS2_LEFT_IN   4
#define SS3_CENTER    7
#define CLP_BUMP      8
#define SS4_RIGHT_IN  11
#define SS5_RIGHT_OUT 12  // Don't use pin 13 as an input..

#define NEAR          A5 // Used as digital, probably


/*-----( Declare objects )-----*/

/*-----( Declare Variables )-----*/

byte  LeftOut; 
byte  LeftIn;
byte  Center;
byte  RightIn;
byte  RightOut;

byte  Bump;
int   Near;  

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);
  Serial.println("YourDuino.com : 7-Way Sensor Test");
}/*--(end setup )---*/

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  Serial.println("Test Start");
  getSensorData();
  showSensorData();
  delay(1000);
}/* --(end main loop )-- */

/*-----( Declare User-written Functions )-----*/

void getSensorData()
{
  LeftOut  = digitalRead(SS1_LEFT_OUT);  
  LeftIn   = digitalRead(SS2_LEFT_IN);  
  Center   = digitalRead(SS3_CENTER);  
  RightIn  = digitalRead(SS4_RIGHT_IN); 
  RightOut = digitalRead(SS5_RIGHT_OUT);  
  Bump     = digitalRead(CLP_BUMP);   
  Near     = digitalRead(NEAR);     
}  

void showSensorData()
{
  if (LeftOut  == 1) {Serial.print("L ");} else {Serial.print("- ");} 
  if (LeftIn   == 1) {Serial.print("l ");} else {Serial.print("- ");}
  if (Center   == 1) {Serial.print("C ");} else {Serial.print("- ");}
  if (RightIn  == 1) {Serial.print("r ");} else {Serial.print("- ");}  
  if (RightOut == 1) {Serial.print("R ");} else {Serial.print("- ");}  
  if (Bump     == 1) {Serial.print(" BUMP!");} else {Serial.print("      ");} 
  if (Near     == 1) {Serial.print(" NEAR");}  else {Serial.print("    ");}   
  Serial.println();
}  
/* ( THE END ) */

