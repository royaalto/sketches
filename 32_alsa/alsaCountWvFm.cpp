// countwave.c
// Counts the number of waveform devices upon each ALSA sound card in the system.
//
// Compile as:
// gcc -o countwave countwave.c -lasound

//http://home.roadrunner.com/~jgglatt/tech/aplay.htm

#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>

int main(int argc, char **argv)
{
   register int  err;
   int           cardNum;

   // Start with first card
   cardNum = -1;

   for (;;)
   {
      snd_ctl_t *cardHandle;

      // Get next sound card's card number. When "cardNum" == -1, then ALSA
      // fetches the first card
      if ((err = snd_card_next(&cardNum)) < 0)
      {
         printf("Can't get the next card number: %s\n", snd_strerror(err));
         break;
      }

      // No more cards? ALSA sets "cardNum" to -1 if so
      if (cardNum < 0) break;

      // Open this card's control interface. We specify only the card number -- not
      // any device nor sub-device too
      {
      char   str[64];

      sprintf(str, "hw:%i", cardNum);
      if ((err = snd_ctl_open(&cardHandle, str, 0)) < 0)
      {
         printf("Can't open card %i: %s\n", cardNum, snd_strerror(err));
         continue;
      }
      }

      {
      int      devNum, totalDevices;

      // No waveform devices found yet
      totalDevices = 0;

      // Start with the first wave device on this card
      devNum = -1;
		
      for (;;)
      {
         // Get the number of the next wave device on this card
         if ((err = snd_ctl_pcm_next_device(cardHandle, &devNum)) < 0)
         {
            printf("Can't get next wave device number: %s\n", snd_strerror(err));
            break;
         }

         // No more wave devices on this card? ALSA sets "devNum" to -1 if so.
         // NOTE: It's possible that this sound card may have no wave devices on it
         // at all, for example if it's only a MIDI card
         if (devNum < 0) break;

         // Another wave device found on this card, so bump the count
         ++totalDevices;
      }

      printf("Found %i digital audio devices on card %i\n", totalDevices, cardNum);
      }

      // Close the card's control interface after we're done with it
      snd_ctl_close(cardHandle);
   }

   // ALSA allocates some mem to load its config file when we call some of the
   // above functions. Now that we're done getting the info, let's tell ALSA
   // to unload the info and free up that mem
   snd_config_update_free_global();
}