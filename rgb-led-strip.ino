#include "FastLED.h"
#include <EEPROM.h>

#define NUM_LEDS 13
CRGB leds[NUM_LEDS];
#define PIN 8

byte selectedEffect = 0; 

const int buttonPin = 2; // the number of the pushbutton pin
const byte potPin = 0;   // Defines the pin the potentiometer is on

volatile int level; // Keeps the brightness level of the LED

int buttonState;           // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers
// ----------------------------------------------------------------

void setup()
{
    FastLED.addLeds<WS2811, PIN, BRG>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    pinMode(buttonPin, INPUT);
    pinMode(potPin, INPUT); // Sets the potentiometer pin to INPUT

    attachInterrupt(digitalPinToInterrupt(buttonPin), changeEffect, CHANGE); // pressed                      // pressed

    Serial.begin(9600);
}

// *** REPLACE FROM HERE ***

/*

    todo - add brightness controller
    todo - add delay controller

*/
void loop()
{
    EEPROM.get(0, selectedEffect);

    if (selectedEffect > 10)
    {
        selectedEffect = 0;
        EEPROM.put(0, 0);
    }

    level = analogRead(potPin);          // Reads in the value from the potentiometer
    level = map(level, 0, 1023, 0, 255); // Maps those values to value for LED brightness

    switch (selectedEffect)
    {
    case 0:
        RGBLoop();
        break;

    case 1:
        Twinkle(0xff, 0x00, 0xff, 10, 100, false);
        break;

    case 2:
        TwinkleRandom(20, 100, false);
        break;

    case 3:
        colorWipe(0xff, 0x00, 0x00, 500);
        colorWipe(0x00, 0xff, 0x00, 500);
        colorWipe(0x00, 0x00, 0xff, 500);
        break;

    case 4:
        rainbowCycle(20);
        break;

    case 5:
        rainbowCycle2(500);
        break;

    case 6:
        rainbowCycle3(20);
        break;

    case 7:
        rainbowCycle4(50);
        break;

    case 8:
        whiteLoop();
        break;

    case 9:
        colorWipe2();
        break;

    case 10:
        colorRun();
        break;
    }
}

void changeEffect()
{

    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 600ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 600)
    {

        if (digitalRead(buttonPin) == HIGH)
        {
            selectedEffect++;
            EEPROM.put(0, selectedEffect);
            asm volatile("  jmp 0");
        }
    }
    last_interrupt_time = interrupt_time;
}

// *************************
// ** LEDEffect Functions **
// *************************

void whiteLoop()
{
    setAll(255, 255, 255);
}
void RGBLoop()
{
    for (int j = 0; j < 3; j++)
    {
        // Fade IN
        for (int k = 0; k < 256; k++)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            delay(3);
        }
        // Fade OUT
        for (int k = 255; k >= 0; k--)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            delay(3);
        }
    }
}

void colorRun()
{
    int SpeedDelay = 200;

    int v = 0;

    for (uint16_t j = 0; j < 256; j++)
    {

        byte *c = Wheel(j);

        for (uint16_t i = 0; i < NUM_LEDS; i++)
        {

            setPixel(i, *c, *(c + 1), *(c + 2));
            setPixel(v, 0x00, 0x00, 0x00);
            v = i;
            showStrip();
            delay(SpeedDelay);
        }
    }
}
void colorWipe2()
{
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    {
        c = Wheel(j);
        colorWipe(*c, *(c + 1), *(c + 2), 50);
        colorWipe(0x00, 0x00, 0x00, 50);
    }
}
void FadeInOut(byte red, byte green, byte blue)
{
    float r, g, b;

    for (int k = 0; k < 256; k = k + 1)
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }

    for (int k = 255; k >= 0; k = k - 2)
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause)
{
    for (int j = 0; j < StrobeCount; j++)
    {
        setAll(red, green, blue);
        showStrip();
        delay(FlashDelay);
        setAll(0, 0, 0);
        showStrip();
        delay(FlashDelay);
    }

    delay(EndPause);
}

void HalloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause)
{
    randomSeed(analogRead(0));

    int i;
    int StartPoint = random(0, NUM_LEDS - (2 * EyeWidth) - EyeSpace);
    int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

    for (i = 0; i < EyeWidth; i++)
    {
        setPixel(StartPoint + i, red, green, blue);
        setPixel(Start2ndEye + i, red, green, blue);
    }

    showStrip();

    if (Fade == true)
    {
        float r, g, b;

        for (int j = Steps; j >= 0; j--)
        {
            r = j * (red / Steps);
            g = j * (green / Steps);
            b = j * (blue / Steps);

            for (i = 0; i < EyeWidth; i++)
            {
                setPixel(StartPoint + i, r, g, b);
                setPixel(Start2ndEye + i, r, g, b);
            }

            showStrip();
            delay(FadeDelay);
        }
    }

    setAll(0, 0, 0); // Set all black

    delay(EndPause);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{

    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);

    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);
}

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

// used by NewKITT
void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--)
    {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

// used by NewKITT
void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++)
    {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

// used by NewKITT
void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

// used by NewKITT
void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne)
{
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++)
    {
        setPixel(random(NUM_LEDS), red, green, blue);
        showStrip();
        delay(SpeedDelay);
        if (OnlyOne)
        {
            setAll(0, 0, 0);
        }
    }

    delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne)
{
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++)
    {
        setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
        showStrip();
        delay(SpeedDelay);
        if (OnlyOne)
        {
            setAll(0, 0, 0);
        }
    }

    delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay)
{
    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
    setPixel(Pixel, 0, 0, 0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay)
{
    setAll(red, green, blue);

    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, 0xff, 0xff, 0xff);
    showStrip();
    delay(SparkleDelay);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay)
{
    int Position = 0;

    for (int i = 0; i < NUM_LEDS * 2; i++)
    {
        Position++; // = 0; //Position + Rate;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            // sine wave, 3 offset waves make a rainbow!
            // float level = sin(i+Position) * 127 + 128;
            // setPixel(i,level,0,0);
            // float level = sin(i+Position) * 127 + 128;
            setPixel(i, ((sin(i + Position) * 127 + 128) / 255) * red,
                     ((sin(i + Position) * 127 + 128) / 255) * green,
                     ((sin(i + Position) * 127 + 128) / 255) * blue);
        }

        showStrip();
        delay(WaveDelay);
    }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay)
{
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        setPixel(i, red, green, blue);
        showStrip();
        delay(SpeedDelay);
    }
}

void rainbowCycle(int SpeedDelay)
{
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++)
    { // 5 cycles of all colors on wheel
        for (i = 0; i < NUM_LEDS; i++)
        {
            c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
            setPixel(i, *c, *(c + 1), *(c + 2));
        }
        showStrip();
        delay(SpeedDelay);
    }
}

void rainbowCycle2(int SpeedDelay)
{
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    { // 5 cycles of all colors on wheel
        for (i = 0; i < NUM_LEDS; i++)
        {
            c = Wheel(j);
            setPixel(i, *c, *(c + 1), *(c + 2));
        }

        showStrip();
        delay(SpeedDelay);
    }
}

void rainbowCycle3(int SpeedDelay)
{
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    { // 5 cycles of all colors on wheel

        int k = 0;
        for (i = 0; i < NUM_LEDS; i++)
        {
            c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
            switch (k)
            {
            case 0:
                setPixel(i, *c, *(c + 1), *(c + 2));
                k++;
                break;

            case 1:
                setPixel(i, *(c + 2), *c, *(c + 1));
                k++;
                break;

            case 2:
                setPixel(i, *(c + 1), *(c + 2), *c);
                k = 0;
                break;
            }
        }

        showStrip();
        delay(SpeedDelay);
    }
}

void rainbowCycle4(int SpeedDelay)
{
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    { // 5 cycles of all colors on wheel

        int k = 0;
        for (i = 0; i < NUM_LEDS; i++)
        {
            c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
            switch (k)
            {
            case 0:
                setPixel(i, *c, *(c + 2), *(c + 1));
                k++;
                break;

            case 1:
                setPixel(i, *(c + 1), *c, *(c + 2));
                k++;
                break;

            case 2:
                setPixel(i, *(c + 2), *(c + 1), *c);
                k = 0;
                break;
            }
        }

        showStrip();
        delay(SpeedDelay);
    }
}

// used by rainbowCycle and theaterChaseRainbow
byte *Wheel(byte WheelPos)
{
    static byte c[3];

    if (WheelPos < 85)
    {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else
    {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }

    return c;
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay)
{
    for (int j = 0; j < 10; j++)
    { // do 10 cycles of chasing
        for (int q = 0; q < 3; q++)
        {
            for (int i = 0; i < NUM_LEDS; i = i + 3)
            {
                setPixel(i + q, red, green, blue); // turn every third pixel on
            }
            showStrip();

            delay(SpeedDelay);

            for (int i = 0; i < NUM_LEDS; i = i + 3)
            {
                setPixel(i + q, 0, 0, 0); // turn every third pixel off
            }
        }
    }
}

void theaterChaseRainbow(int SpeedDelay)
{
    byte *c;

    for (int j = 0; j < 256; j++)
    { // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++)
        {
            for (int i = 0; i < NUM_LEDS; i = i + 3)
            {
                c = Wheel((i + j) % 255);
                setPixel(i + q, *c, *(c + 1), *(c + 2)); // turn every third pixel on
            }
            showStrip();

            delay(SpeedDelay);

            for (int i = 0; i < NUM_LEDS; i = i + 3)
            {
                setPixel(i + q, 0, 0, 0); // turn every third pixel off
            }
        }
    }
}

void Fire(int Cooling, int Sparking, int SpeedDelay)
{
    static byte heat[NUM_LEDS];
    int cooldown;

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

        if (cooldown > heat[i])
        {
            heat[i] = 0;
        }
        else
        {
            heat[i] = heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < Sparking)
    {
        int y = random(7);
        heat[y] = heat[y] + random(160, 255);
        // heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        setPixelHeatColor(j, heat[j]);
    }

    showStrip();
    delay(SpeedDelay);
}

void setPixelHeatColor(int Pixel, byte temperature)
{
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;              // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80)
    { // hottest
        setPixel(Pixel, 255, 255, heatramp);
    }
    else if (t192 > 0x40)
    { // middle
        setPixel(Pixel, 255, heatramp, 0);
    }
    else
    { // coolest
        setPixel(Pixel, heatramp, 0, 0);
    }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous)
{
    float Gravity = -9.81;
    int StartHeight = 1;

    float Height[BallCount];
    float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
    float ImpactVelocity[BallCount];
    float TimeSinceLastBounce[BallCount];
    int Position[BallCount];
    long ClockTimeSinceLastBounce[BallCount];
    float Dampening[BallCount];
    boolean ballBouncing[BallCount];
    boolean ballsStillBouncing = true;

    for (int i = 0; i < BallCount; i++)
    {
        ClockTimeSinceLastBounce[i] = millis();
        Height[i] = StartHeight;
        Position[i] = 0;
        ImpactVelocity[i] = ImpactVelocityStart;
        TimeSinceLastBounce[i] = 0;
        Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
        ballBouncing[i] = true;
    }

    while (ballsStillBouncing)
    {
        for (int i = 0; i < BallCount; i++)
        {
            TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
            Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

            if (Height[i] < 0)
            {
                Height[i] = 0;
                ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
                ClockTimeSinceLastBounce[i] = millis();

                if (ImpactVelocity[i] < 0.01)
                {
                    if (continuous)
                    {
                        ImpactVelocity[i] = ImpactVelocityStart;
                    }
                    else
                    {
                        ballBouncing[i] = false;
                    }
                }
            }
            Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
        }

        ballsStillBouncing = false; // assume no balls bouncing
        for (int i = 0; i < BallCount; i++)
        {
            setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
            if (ballBouncing[i])
            {
                ballsStillBouncing = true;
            }
        }

        showStrip();
        setAll(0, 0, 0);
    }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    setAll(0, 0, 0);

    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++)
    {

        // fade brightness all LEDs one step
        for (int j = 0; j < NUM_LEDS; j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                fadeToBlack(j, meteorTrailDecay);
            }
        }

        // draw meteor
        for (int j = 0; j < meteorSize; j++)
        {
            if ((i - j < NUM_LEDS) && (i - j >= 0))
            {
                setPixel(i - j, red, green, blue);
            }
        }

        showStrip();
        delay(SpeedDelay);
    }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

    strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

// *** REPLACE TO HERE ***

// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip()
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    FastLED.show();
#endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue)
{

    red = map(level, 0, 255, 0, red);
    green = map(level, 0, 255, 0, green);
    blue = map(level, 0, 255, 0, blue);

#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[Pixel].r = red;
    leds[Pixel].g = green;
    leds[Pixel].b = blue;
#endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        setPixel(i, red, green, blue);
    }
    showStrip();
}