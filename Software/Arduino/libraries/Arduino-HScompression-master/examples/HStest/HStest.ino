// Demo Code for Heatshrink (Copyright (c) 2013-2015, Scott Vokes <vokes.s@gmail.com>)
// embedded compression library
// Craig Versek, Apr. 2016

#include <stdint.h>
#include <ctype.h>
#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"

#define arduinoLED 13   // Arduino LED on board

/******************************************************************************/
// TEST CODE from adapted from test_heatshrink_static.c
#if HEATSHRINK_DYNAMIC_ALLOC
#error HEATSHRINK_DYNAMIC_ALLOC must be false for static allocation test suite.
#endif

//#define HEATSHRINK_DEBUG

static heatshrink_encoder hse;
static heatshrink_decoder hsd;

//static void fill_with_pseudorandom_letters(uint8_t *buf, uint16_t size, uint32_t seed) {
//    uint64_t rn = 9223372036854775783; // prime under 2^64
//    for (int i=0; i<size; i++) {
//        rn = rn*seed + seed;
//        buf[i] = (rn % 26) + 'a';
//    }
//}

#ifdef HEATSHRINK_DEBUG
static void dump_buf(const char *name, uint8_t *buf, uint16_t count) {
    for (int i=0; i<count; i++) {
        uint8_t c = (uint8_t)buf[i];
        //printf("%s %d: 0x%02x ('%c')\n", name, i, c, isprint(c) ? c : '.');
        Serial.print(name);
        Serial.print(F(" "));
        Serial.print(i);
        Serial.print(F(": 0x"));
        Serial.print(c, HEX);
        Serial.print(F(" ('"));
        Serial.print(isprint(c) ? c : '.', BYTE);
        Serial.print(F("')\n"));
    }
}
#endif

static void compress(uint8_t *input,
                     uint32_t input_size,
                     uint8_t *output,
                     uint32_t &output_size
                    ){
    heatshrink_encoder_reset(&hse);
    
    #ifdef HEATSHRINK_DEBUG
    Serial.print(F("\n^^ COMPRESSING\n"));
    dump_buf("input", input, input_size);
    #endif
        
    size_t   count  = 0;
    uint32_t sunk   = 0;
    uint32_t polled = 0;
    while (sunk < input_size) {
        //ASSERT(heatshrink_encoder_sink(&hse, &input[sunk], input_size - sunk, &count) >= 0);
        heatshrink_encoder_sink(&hse, &input[sunk], input_size - sunk, &count);
        sunk += count;
        #ifdef HEATSHRINK_DEBUG
        Serial.print(F("^^ sunk "));
        Serial.print(count);
        Serial.print(F("\n"));
        #endif
        if (sunk == input_size) {
            //ASSERT_EQ(HSER_FINISH_MORE, heatshrink_encoder_finish(&hse));
            heatshrink_encoder_finish(&hse);
        }

        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(&hse,
                                           &output[polled],
                                           output_size - polled,
                                           &count);
            //ASSERT(pres >= 0);
            polled += count;
            #ifdef HEATSHRINK_DEBUG
            Serial.print(F("^^ polled "));
            Serial.print(polled);
            Serial.print(F("\n"));
            #endif
        } while (pres == HSER_POLL_MORE);
        //ASSERT_EQ(HSER_POLL_EMPTY, pres);
        #ifdef HEATSHRINK_DEBUG
        if (polled >= output_size){
            Serial.print(F("FAIL: Exceeded the size of the output buffer!\n"));
        }
        #endif
        if (sunk == input_size) {
            //ASSERT_EQ(HSER_FINISH_DONE, heatshrink_encoder_finish(&hse));
            heatshrink_encoder_finish(&hse);
        }
    }
    #ifdef HEATSHRINK_DEBUG
    Serial.print(F("in: "));
    Serial.print(input_size);
    Serial.print(F(" compressed: "));
    Serial.print(polled);
    Serial.print(F(" \n"));
    #endif
    //update the output size to the (smaller) compressed size
    output_size = polled;
    
    #ifdef HEATSHRINK_DEBUG
    dump_buf("output", output, output_size);
    #endif
}

static void decompress(uint8_t *input,
                       uint32_t input_size,
                       uint8_t *output,
                       uint32_t &output_size
                      ){
    heatshrink_decoder_reset(&hsd);
    #ifdef HEATSHRINK_DEBUG
    Serial.print(F("\n^^ DECOMPRESSING\n"));
    dump_buf("input", input, input_size);
    #endif
    size_t   count  = 0;
    uint32_t sunk   = 0;
    uint32_t polled = 0;
    while (sunk < input_size) {
        //ASSERT(heatshrink_decoder_sink(&hsd, &comp[sunk], input_size - sunk, &count) >= 0);
        heatshrink_decoder_sink(&hsd, &input[sunk], input_size - sunk, &count);
        sunk += count;
        #ifdef HEATSHRINK_DEBUG
        Serial.print(F("^^ sunk "));
        Serial.print(count);
        Serial.print(F("\n"));
        #endif
        if (sunk == input_size) {
            //ASSERT_EQ(HSDR_FINISH_MORE, heatshrink_decoder_finish(&hsd));
            heatshrink_decoder_finish(&hsd);
        }

        HSD_poll_res pres;
        do {
            pres = heatshrink_decoder_poll(&hsd, &output[polled],
                output_size - polled, &count);
            //ASSERT(pres >= 0);
            polled += count;
            #ifdef HEATSHRINK_DEBUG
            Serial.print(F("^^ polled "));
            Serial.print(polled);
            Serial.print(F("\n"));
            #endif
        } while (pres == HSDR_POLL_MORE);
        //ASSERT_EQ(HSDR_POLL_EMPTY, pres);
        if (sunk == input_size) {
            HSD_finish_res fres = heatshrink_decoder_finish(&hsd);
            //ASSERT_EQ(HSDR_FINISH_DONE, fres);
        }
        if (polled > output_size) {
            #ifdef HEATSHRINK_DEBUG
            Serial.print(F("FAIL: Exceeded the size of the output buffer!"));
            #endif
        }
    }
    #ifdef HEATSHRINK_DEBUG
    Serial.print(F("in: "));
    Serial.print(input_size);
    Serial.print(F(" decompressed: "));
    Serial.print(polled);
    Serial.print(F(" \n"));
    #endif
    //update the output size
    output_size = polled;
    
    #ifdef HEATSHRINK_DEBUG
    dump_buf("output", output, output_size);
    #endif
}

/******************************************************************************/

#define BUFFER_SIZE 256
uint8_t orig_buffer[BUFFER_SIZE];
uint8_t comp_buffer[BUFFER_SIZE];
uint8_t decomp_buffer[BUFFER_SIZE];

void setup() {
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off
  Serial.begin(9600);
  delay(5000);
  //write some data into the compression buffer
  const char test_data[] = "\x01\x00\x00\x02\x00\x00\x03\x00\x00\x03\x00\x00\x04\x00\x00\x04\x00\x00\x03\x00\x00\x01\x00\x00\x04\x00\x00\x08\x00\x00\x01\x00\x00\x08\x00\x00\x07\x00\x00\x05\x00\x00";
  uint32_t orig_size = 42;//strlen(test_data);
  uint32_t comp_size   = BUFFER_SIZE; //this will get updated by reference
  uint32_t decomp_size = BUFFER_SIZE; //this will get updated by reference
  memcpy(orig_buffer, test_data, orig_size);
  uint32_t t1 = micros();
  compress(orig_buffer,orig_size,comp_buffer,comp_size);
  uint32_t t2 = micros();
  decompress(comp_buffer,comp_size,decomp_buffer,decomp_size);
  uint32_t t3 = micros();
  Serial.print("Size of orginal data: ");Serial.println(orig_size);
  Serial.print("Size of compressed data: ");Serial.println(comp_size);
  float comp_ratio = ((float) orig_size / comp_size);
  Serial.print("Compression ratio: ");Serial.println(comp_ratio);
  Serial.print("Time to compress: ");Serial.println((t2-t1)/1e6,6);
  Serial.print("Time to decompress: ");Serial.println((t3-t2)/1e6,6);
}

void loop() {
  delay(100);
}

