
/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "project_cfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <devices.h>
#include <filesystem.h>
#include <storage/sdcard.h>

#include "llm.h"

// Define the prompt string
#define LLM_PROMPT_STRING "One day"

// Initialize th SD card
// see project_cfg.h for the pin configuration
handle_t init_storage(void)
{
    handle_t spi, gpio;
    configASSERT(spi = io_open("/dev/spi0"));
    configASSERT(gpio = io_open("/dev/gpio0"));
    handle_t sd0 = spi_sdcard_driver_install(spi, gpio, 7);
    io_close(spi);
    io_close(gpio);
    return sd0;
}

// Callback for when generation is complete
void generate_complete_cb(float tk_s)
{
    char buffer[50];
    sprintf(buffer, "%.2f tok/s", tk_s);
}

int main()
{
    printf("[LLM] Hello LLM\n");

    // Initialize the SD card
    handle_t sd0 = init_storage();
    configASSERT(sd0);
    configASSERT(filesystem_mount("/fs/0/", sd0) == 0);
    io_close(sd0);

    // Model path
    char *checkpoint_path = "/fs/0//stories260K.bin"; // e.g. out/model.bin
    char *tokenizer_path = "/fs/0/tok512.bin";

    // Default parameters
    float temperature = 0.8f;        // 0.0 = greedy deterministic. 1.0 = original. don't set higher
    float topp = 0.9f;               // top-p in nucleus sampling. 1.0 = off. 0.9 works well, but slower
    int steps = 256;                 // number of steps to run for

    char *prompt = LLM_PROMPT_STRING;             // prompt string
    unsigned long long rng_seed = 0; // seed rng with time by default

    // parameter validation/overrides
    if (rng_seed <= 0)
        rng_seed = (unsigned int)time(NULL);

    printf("[LLM] Model path: %s \n", checkpoint_path);

    // build the Transformer via the model .bin file
    Transformer transformer;
    build_transformer(&transformer, checkpoint_path);
    if (steps == 0 || steps > transformer.config.seq_len)
        steps = transformer.config.seq_len; // override to ~max length

    // build the Tokenizer via the tokenizer .bin file
    Tokenizer tokenizer;
    build_tokenizer(&tokenizer, tokenizer_path, transformer.config.vocab_size);

    // build the Sampler
    Sampler sampler;
    build_sampler(&sampler, transformer.config.vocab_size, temperature, topp, rng_seed);

    // run!
    generate(&transformer, &tokenizer, &sampler, prompt, steps, &generate_complete_cb);

    while (1)
        ;
}
