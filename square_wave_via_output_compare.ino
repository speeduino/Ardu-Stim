
/*
 * 60-2 wheel pattern generator
 * Copyright (C) 2014 David J. Andruczyk
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 *
 */
 
 volatile unsigned char edgecount = 0;
 volatile unsigned char teethcount = 0;
 volatile unsigned char pinstate = 0;
 volatile unsigned char tmp = 0;
 volatile unsigned char real_teeth = 58;
 volatile unsigned char missing_edges = 4;  /* 2 teeth * 2 edges per tooth (rising and falling) */
  
 void setup() {
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
   OCR1A = 500;   /* 16000 RPM */
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
 }
 
 void loop() {
/* We could do one of the following:
 * programmatically screw with the OCR1A register to adjust the RPM (i.e. auto-sweep)
 * read a pot and modify it
 * read the serial port and modify it
 * read other inputs to switch wheel modes
 */

 }
