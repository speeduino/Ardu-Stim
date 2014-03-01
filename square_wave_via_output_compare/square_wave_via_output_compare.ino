
/*
 * 60-2 wheel pattern generator
 * Copyright (C) 2014 David J. Andruczyk
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 *
 */
 
 volatile unsigned char edgecount = 0;
 volatile unsigned char teethcount = 0;
 volatile unsigned char pinstate = 0;
 volatile unsigned char real_teeth = 58;
 volatile unsigned char missing_edges = 4;  /* 2 teeth * 2 edges per tooth (rising and falling) */
 volatile unsigned int new_OCR1A = 8000;
 enum  { DESCENDING, ASCENDING };
 byte state = ASCENDING;
 #define RPM_STEP 1
 #define RPM_MAX 16000
 #define RPM_MIN 100
 #define RPM_STEP_DELAY 1
 unsigned int wanted_rpm = 1000;
   
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
   DDRB = B00000001; /* Set pin 8 as output */
   //pinMode(8, OUTPUT);
 } // End setup
 
 ISR(TIMER1_COMPA_vect) {
   if (pinstate) { /* If high count it as a tooth */
     teethcount++; /* increment tooth counter... */
   }
   if (teethcount >= real_teeth && pinstate == 0) { /* End of "real" teeth, beginning of missing tooth window */
     edgecount++; /* Start counting edges (2 edges per tooth, interrupt handlers runs for each edge */
     if (edgecount == missing_edges) { /* 4 edges == 2 teeth equivalent (the missing ones... ) */
       teethcount = 0; /* Reset counters and return */
       edgecount = 0;

       return;
     }
     return;
   }
   pinstate ^= 1; /* Toggle pin state */
   PORTB = pinstate;   /* Write it to the port */
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
   new_OCR1A=8000000/(wanted_rpm);
   //Serial.print("new_OCR1A var is: ");
   //Serial.println(new_OCR1A);
   delay(RPM_STEP_DELAY);


 }
