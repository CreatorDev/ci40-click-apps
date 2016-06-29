/***************************************************************************************************
 * Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies
 * and/or licensors
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "gpio.h"

#define INFINITE_TIMEOUT  -1

static bool g_Running = true;

static void SignalHandler(int sig)
{
    g_Running = false;
    signal(sig, SIG_IGN);
}

static void PrintUsage(const char *program)
{
    printf("Usage: %s [options]\n\n"
        " -m  <arg>  mikroBUS number\n"
        " -h         display this message\n\n",
        program);
}

static int ParseCmdOpts(int argc, char *argv[], uint8_t *gpio)
{
    int opt;
    uint8_t num;

    opterr = 0;

    while ((opt = getopt(argc, argv, "m:h")) != -1)
    {
        switch (opt)
        {
            case 'm':
                num = strtoul(optarg, NULL, 0);
                if (num != 1 && num != 2)
                {
                    fprintf(stderr, "Error: invalid mikroBUS\n");
                    return -1;
                }
                *gpio = GPIO_FOR_MIKROBUS(num);
                break;

            case 'h':
                PrintUsage(argv[0]);
                return 0;

            default:
                PrintUsage(argv[0]);
                return -1;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int8_t status = 0;
    struct sigaction Sa = {{0}};
    uint8_t gpio = GPIO_FOR_MIKROBUS(1);

    Sa.sa_handler = &SignalHandler;
    sigemptyset(&Sa.sa_mask);
    Sa.sa_flags = 0;

    if (sigaction(SIGINT, &Sa, NULL) != 0)
    {
        fprintf(stderr, "Could not set signal handler for SIGINT\n");
        return -1;
    }

    status = ParseCmdOpts(argc, argv, &gpio);
    if (status <= 0)
    {
        return status;
    }

    status = GpioExport(gpio);
    if (status == -1)
    {
        goto cleanup;
    }

    status = GpioDirection(gpio, GPIO_DIRECTION_IN);
    if (status == -1)
    {
        goto cleanup;
    }

    status = GpioEdge(gpio, GPIO_EDGE_RISING);
    if (status == -1)
    {
        goto cleanup;
    }

    while (g_Running)
    {
        printf("\nReady for detecting motion...\n");
        status = GpioWaitForInterrupt(gpio, INFINITE_TIMEOUT);
        if (status == -1)
        {
            break;
        }
        else if (status > 0)
        {
            printf("detected\n");
        }
    }

cleanup:
    GpioUnexport(gpio);

    return status;
}
