
/*
 * 60-2 wheel pattern generator
 * copyright 2014 David J. Andruczyk
 *
 */
 
 volatile unsigned char edgecount = 0;
 volatile unsigned char teethcount = 0;
 volatile unsigned char pinstate = 0;
 volatile unsigned char tmp = 0;
 
 void setup() {
   cli(); // stop interrupts
   
   // Set timer1 to generate pulses
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   // Set compare registers 
   OCR1A = 8000; 

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
   if (teethcount >= 58 && pinstate == 0) { /* End of "real" teeth, beginning of missing tooth window */
     edgecount++; /* Start counting edges (2 edges per tooth, interrupt handlers runs for each edge */
     if (edgecount == 4) { /* 4 edges == 2 teeth equivalent (the missing ones... ) */
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
