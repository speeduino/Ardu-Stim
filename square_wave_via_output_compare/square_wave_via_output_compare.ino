
/*
 * Arbritrary wheel pattern generator (60-2 and 36-1 so far... )
 * copyright 2014 David J. Andruczyk
 *
 */
 
/* The "RPM" of the wheel is dependent on the number of edges
 * so for a 60-2 wheel (120 edges), the time between teeth is
 * 8000000/RPM,  but for lesser teeth wheels this will be different
 * Thus we need a corresponding array to fix that, so that the 
 * requested RPM is actually exported as we want
 */


#define RPM_STEP 0
#define RPM_MAX 2000
#define RPM_MIN 300
#define RPM_STEP_DELAY 1
#define MAX_EDGES 150

 volatile unsigned int new_OCR1A = 8000; /* sane default */
 enum  { DESCENDING, ASCENDING };
 byte state = ASCENDING;

 unsigned int wanted_rpm = 8000;
 enum { 
   DIZZY_FOUR_CYLINDER,  /* 2 evenly spaced teeth */
   DIZZY_SIX_CYLINDER,   /* 3 evenly spaced teeth */
   DIZZY_EIGHT_CYLINDER, /* 4 evenly spaced teeth */
   SIXTY_MINUS_TWO,      /* 60-2 crank only */
   THIRTY_SIX_MINUS_ONE, /* 36-1 crank only */
   FOUR_MINUS_ONE_WITH_CAM, /* 4-1 crank + cam */
   EIGHT_MINUS_ONE,       /* 8-1 */
   SIX_MINUS_ONE_WITH_CAM,/* 6-1 crank + cam */
   TWELVE_MINUS_ONE,      /* 12-1 crank + cam */
   MAX_WHEELS,
 };
 //volatile byte selected_wheel = SIXTY_MINUS_TWO;
 volatile byte selected_wheel = TWELVE_MINUS_ONE;
 volatile unsigned char edge_counter = 0;
 const float rpm_scaler[MAX_WHEELS] = {
   0.03333, /* dizzy 4 */
   0.05, /* dizzy 6 */
   0.06667, /* dizzy 8 */
   1.0, /* 60-2 */
   0.6, /* 36-1  (72 edges/120) */
   0.13333, /* 4-1 with cam */
   0.13333, /* 8-1 */
   0.3,     /* 6-1 with cam */
   1.2,    /* 12-1 with cam */
 };
 const byte wheel_max_edges[MAX_WHEELS] = {
   4, /* dizzy 4 */
   6, /* dizzy 6 */
   6, /* dizzy 8 */
   120, /* 60-2 */
   72,  /* 36 -1 */
   16,  /* 4-1 with cam */
   16,  /* 8-1 */
   36,  /* 6-1 with cam */
   144,  /* 12-1 with cam */
 };
 
 const byte edge_states[MAX_WHEELS][MAX_EDGES] = {
   { /* dizzy 4 cyl */
     1,0,1,0
  },
   { /* dizzy 6 cyl */
     1,0,1,0,1,0
   },
   { /* dizzy 8 cyl */
     1,0,1,0,1,0,1,0
     
   },
   { /* 60-2 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,0,0,0,0
   },
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     0,0
   },
   { /* 4-1 with cam */
     0,1,0,1,0,1,0,0,0,1, \
     2,1,0,1,0,0
   },
   { /* 8-1 */
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,0
   },
   { /* 6-1 with cam */
     0,0,1,0,0,1,0,0,1,0, \
     0,1,0,0,1,0,0,0,0,0, \
     1,0,0,1,2,2,1,0,0,1, \
     0,0,1,0,0,0
   },
   { /* 12-1 with cam */
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,0,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,2,2,2,2, \
     2,1,0,0,0,0,0,1,0,0, \
     0,0,0,0
   },
};

   
 void setup() {
   Serial.begin(9600);
   cli(); // stop interrupts
   
   // Set timer1 to generate pulses
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   // Set compare registers 
   // OCR1A = 8000;  /* 1000 RPM */
   OCR1A = 4000;  /* 2000  RPM */ 
   OCR1A = 2000;  /* 4000  RPM */
   OCR1A = 1000;  /* 8000  RPM */
   //OCR1A = 500;   /* 16000 RPM */
   //OCR1A = 250;   /* 32000 RPM */

   // Turn on CTC mode
   TCCR1B |= (1 << WGM12); // Normal mode (not PWM)
   // Set prescaler to 1
   TCCR1B |= (1 << CS10); /* Prescaler of 1 */
   // Enable output compare interrupt
   TIMSK1 |= (1 << OCIE1A);
   
   sei(); // Enable interrupts
   DDRB = B00000011; /* Set pin 8 and 9 as output (crank and cam respectively) */
   //pinMode(8, OUTPUT);
 } // End setup
 
 ISR(TIMER1_COMPA_vect) {
   /* This is VERY simple, just walk the array and wrap when we hit the limit */
   edge_counter++;
   if (edge_counter >= wheel_max_edges[selected_wheel]) {
     edge_counter = 0;
   }
   PORTB = edge_states[selected_wheel][edge_counter];   /* Write it to the port */
   OCR1A = new_OCR1A;  /* Apply new "RPM" from main loop, i.e. speed up/down the virtual "wheel" */
 }
 
 void loop() {
   
/* We could do one of the following:
 * programmatically screw with the OCR1A register to adjust the RPM (i.e. auto-sweep)
 * read a pot and modify it
 * read the serial port and modify it
 * read other inputs to switch wheel modes
 */
   
   switch (state) {
     case DESCENDING:
     wanted_rpm -= RPM_STEP;
     if (wanted_rpm <= RPM_MIN) {
       state = ASCENDING;
     }
     //Serial.print("Descending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);
     break;
     case ASCENDING:
     wanted_rpm += RPM_STEP;
     if (wanted_rpm >= RPM_MAX) {
       state = DESCENDING;
     }
     //Serial.print("Ascending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);    break;   
   }
   new_OCR1A=8000000/(wanted_rpm*rpm_scaler[selected_wheel]);
   //Serial.print("new_OCR1A var is: ");
   //Serial.println(new_OCR1A);
   delay(RPM_STEP_DELAY);


 }
