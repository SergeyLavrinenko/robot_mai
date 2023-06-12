#include "drink_control.h"

void Drink_control::open(){
    system("gpio mode 0 out");
    system("gpio write 0 1");
}

void Drink_control::close(){
    system("gpio write 0 0");
}

bool Drink_control::drink_ready(){
    if(true){
        this->close();
    }
    return true;
}

Drink_control::Drink_control(){
    start_light();
}


int NUM_LEDS = 10;
WS2812B::Led leds(0, NUM_LEDS);



void Drink_control::setPixelColor(int pixelNumber, int r, int g, int b){
    WS2812B::RGB color = leds.getColor(pixelNumber);
    bool r_ok = false;
    bool g_ok = false;
    bool b_ok = false;
    while (true){
        if (!r_ok){
            if (color._r > r) color._r --;
            else if (color._r < r) color._r++;
            else r_ok = true;
        }
        if (!g_ok){
            if (color._g > g) color._g --;
            else if (color._g < g) color._g++;
            else g_ok = true;
        }
        if (!b_ok){
            if (color._b > b) color._b --;
            else if (color._b < b) color._b++;
            else b_ok = true;
        }
        leds.setColor(color, pixelNumber);
        leds.show();
        if (r_ok && g_ok && b_ok) return;
    }
}

void Drink_control::start_light(){
    for (int i = 0; i < NUM_LEDS; i++) {
        setPixelColor(i, 60, 60, 20);
    }
}