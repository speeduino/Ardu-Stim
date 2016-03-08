
#include "ardustim.h"
#include "defines.h"
#include "loop.h"

extern SUI::SerialUI mySUI;

void loop() {
  uint16_t tmp_rpm = 0;
  extern volatile bool adc0_read_complete;
  extern volatile uint16_t adc0;
  /* Just handle the Serial UI, everything else is in 
   * interrupt handlers or callbacks from SerialUI.
   */

  if (mySUI.checkForUserOnce())
  {
    // Someone connected!
    mySUI.enter();
    while (mySUI.userPresent()) 
    {
      mySUI.handleRequests();
    }
  }
  if (adc0_read_complete == true)
  {
    adc0_read_complete = false;
    tmp_rpm = adc0 << TMP_RPM_SHIFT;
    if (tmp_rpm > TMP_RPM_CAP)
      tmp_rpm = TMP_RPM_CAP;
    reset_new_OCR1A(tmp_rpm);
  }
}

