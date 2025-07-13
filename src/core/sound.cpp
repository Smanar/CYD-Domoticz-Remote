#include "sound.h"
#include <Arduino.h>

//https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-generator-pwm/streams-generator-pwm.ino
//https://letmeknow.fr/fr/blog/176-tuto-jouer-de-la-musique-avec-lesp32
//https://github.com/damellis/PCM

unsigned long startTime;
unsigned long duration;

const int tonePin = 26; // Pin for tone output
bool sound_running = false;

enum {
    TYPE_OFF,
    TYPE_PHASE1,
    TYPE_PHASE2,
    TYPE_PHASE3
};
int phase = TYPE_OFF;
unsigned int freq;

void initsound(void)
{

}

void make_sound(unsigned int frequency, unsigned int duration)
{
    startTime = millis();
    tone(tonePin, frequency);
    sound_running = true;
}

void make_custom_sound(void)
{
    startTime = millis();
    freq = 0;
    sound_running = true;
}

bool sound_hasExpired() {
    return (millis() - startTime >= duration);
}

void sound_loop(void)
{
    if (!sound_running) return;

    if (sound_hasExpired())
    {
        noTone(tonePin);
        sound_running = false;
    }
}

void sound_loop_custom(void)
{
    if (!sound_running) return;

    //Need to wait at least 100ms
    if ((millis() - startTime) < 100) return;

    switch (phase)
    {
        case TYPE_OFF:
            return;
        case TYPE_PHASE1:
            freq +=1;
            if (freq > 500) { phase = TYPE_PHASE2; }
            break;
        case TYPE_PHASE3:
            freq -=1;
            if (freq < 0) { phase = TYPE_OFF; }
            break;
        case TYPE_PHASE2:
            break;
    }

    tone(tonePin, freq);

}