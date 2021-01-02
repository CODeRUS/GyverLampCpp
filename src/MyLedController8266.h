#pragma once

#include <FastLED.h>

static uint8_t ledPin = 0;

class PinHolder
{
public:
    static void setLedPin(uint8_t pinNum)
    {
        ledPin = pinNum;
    }
};

FASTLED_NAMESPACE_BEGIN

// Info on reading cycle counter from https://github.com/kbeckmann/nodemcu-firmware/blob/ws2812-dual/app/modules/ws2812.c
__attribute__ ((always_inline)) inline static uint32_t __clock_custom_cycles() {
  uint32_t cyc;
  __asm__ __volatile__ ("rsr %0,ccount":"=a" (cyc));
  return cyc;
}

template <int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int WAIT_TIME = 50>
class ClocklessCustomController : public CPixelLEDController<RGB_ORDER> {
    typedef volatile RwReg * data_ptr_t;
    typedef RwReg data_t;

    data_t mPinMask;
    data_ptr_t mPort;
    CMinWait<WAIT_TIME> mWait;
public:
    static void setLedPin(uint8_t pinNum)
    {
        ledPin = pinNum;
    }

    virtual void init()
    {
        Serial.print(F("Using led pin: "));
        Serial.println(ledPin);

        pinMode(ledPin, OUTPUT);
        mPinMask = ledPin == 16 ? 1 : (1UL << ledPin);
        mPort = (volatile RwReg*)(ledPin == 16 ? &GP16O : &GPO);
    }

    virtual uint16_t getMaxRefreshRate() const
    {
        return 800;
    }

protected:

    virtual void showPixels(PixelController<RGB_ORDER> & pixels)
    {
        // mWait.wait();
        int cnt = FASTLED_INTERRUPT_RETRY_COUNT;
        while ((showRGBInternal(pixels) == 0) && cnt--) {
#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
            _retry_cnt++;
#endif
            os_intr_unlock();
            delayMicroseconds(WAIT_TIME);
            os_intr_lock();
        }
    }

    template<int BITS>
    __attribute__ ((always_inline)) inline void writeBits(uint32_t & last_mark, uint32_t b)
    {
        b <<= 24; b = ~b;
        for (uint32_t i = BITS; i > 0; i--) {
            while((__clock_custom_cycles() - last_mark) < (T1 + T2 + T3));
            last_mark = __clock_custom_cycles();
            *mPort |= mPinMask;

            while ((__clock_custom_cycles() - last_mark) < T1);
            if (b & 0x80000000L) {
                *mPort &= ~mPinMask;
            }
            b <<= 1;

            while((__clock_custom_cycles() - last_mark) < (T1 + T2));
            *mPort &= ~mPinMask;
        }
    }

    uint32_t ICACHE_RAM_ATTR showRGBInternal(PixelController<RGB_ORDER> pixels)
    {
        // Setup the pixel controller and load/scale the first byte
        pixels.preStepFirstByteDithering();
        uint32_t b = pixels.loadAndScale0();
        pixels.preStepFirstByteDithering();
        os_intr_lock();
        uint32_t start = __clock_custom_cycles();
        uint32_t last_mark = start;
        while (pixels.has(1)) {
            // Write first byte, read next byte
            writeBits<8>(last_mark, b);
            b = pixels.loadAndScale1();

            // Write second byte, read 3rd byte
            writeBits<8>(last_mark, b);
            b = pixels.loadAndScale2();

            // Write third byte, read 1st byte of next pixel
            writeBits<8>(last_mark, b);
            b = pixels.advanceAndLoadAndScale0();

#if (FASTLED_ALLOW_INTERRUPTS == 1)
            os_intr_unlock();
#endif

            pixels.stepDithering();

#if (FASTLED_ALLOW_INTERRUPTS == 1)
            os_intr_lock();
            // if interrupts took longer than 45µs, punt on the current frame
            if ((int32_t)(__clock_custom_cycles()-last_mark) > 0) {
                if ((int32_t)(__clock_custom_cycles()-last_mark) > (T1+T2+T3+((WAIT_TIME-INTERRUPT_THRESHOLD)*CLKS_PER_US))) {
                    sei();
                    return 0;
                }
            }
#endif
        };

        os_intr_unlock();
#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
        _frame_cnt++;
#endif
        return __clock_custom_cycles() - start;
    }
};

FASTLED_NAMESPACE_END
