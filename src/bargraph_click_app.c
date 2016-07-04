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
#include <stdint.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "spi_common.h"
#include "pwm_common.h"

static volatile bool g_Running = true;

/* bottom 10 bits of display_seg argument correspond to 10 display segments */
static int send_to_bargraph(int fd, unsigned int display_seg)
{
    int ret;
    unsigned int buf[2];
    struct spi_ioc_transfer tr[2];

    /* buf[0] will be shifted into segments 8 and 9 */
    buf[0] = (display_seg >> 8) & 3;
    /* buf[1] sets segments 0 to 7 */
    buf[1] = display_seg & 0x00ff;

    /* send message consisting of two transfers */
    memset(&tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)&buf[0];
    tr[0].len = 1;
    tr[1].tx_buf = (unsigned long)&buf[1];
    tr[1].len = 1;
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);

    if (ret == -1)
	{
        perror("SPI error: can't send data\n");
        return -1;
    }

    return 0;
}

static void SignalHandler(int sig)
{
	printf("\n in signal handler");
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

static int ParseCmdOpts(int argc, char *argv[], char **spi_path)
{
    int opt;
    uint8_t num;

    opterr = 0;

    while ((opt = getopt(argc, argv, "m:h")) != -1)
    {
        switch (opt)
        {
            case 'm':
                num = strtoul(optarg, NULL, 10);
                if (num != 1 && num != 2)
                {
                    fprintf(stderr, "Error: invalid mikroBUS\n");
                    return -1;
                }
                *spi_path = SPI_PATH_FOR_MIKROBUS(num);
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

void clean_stdin()
{
    while (getchar()!='\n');
	//return 1
}

int main(int argc, char *argv[])
{
	int fd, status;
	unsigned int num_to_display;
	char *spi_path = SPI_PATH_FOR_MIKROBUS(1);
	struct sigaction Sa = {{0}};

	Sa.sa_handler = &SignalHandler;
	sigemptyset(&Sa.sa_mask);
	Sa.sa_flags = 0;

	if (sigaction(SIGINT, &Sa, NULL) != 0)
	{
		fprintf(stderr, "Could not set signal handler for SIGINT\n");
		return -1;
	}

	status = ParseCmdOpts(argc, argv, &spi_path);
	if (status <= 0)
	{
		return status;
	}

    fd = mikrobus_spi_init(spi_path);
    if (fd < 0)
	{
        return -1;
    }

	status = PwmExport(0);
    if (status == -1)
    {
        goto cleanup;
    }
	status = PwmPeriod(0, 100000);
	if (status == -1)
	{
		goto cleanup;
	}
	status = PwmDutyCycle(0, 50000);
	if (status == -1)
	{
		goto cleanup;
	}
	status = PwmEnable(0);
	if (status == -1)
	{
		goto cleanup;
	}

	char c;
	while (g_Running)
	{
		printf("\nEnter number to display: ");
		fflush(stdout);
		if (scanf("%u%c", &num_to_display, &c) != 2 || c != '\n')
			clean_stdin();
		send_to_bargraph(fd, num_to_display);
	}

cleanup:
	send_to_bargraph(fd, 0);
    PwmUnexport(0);
    mikrobus_spi_free(fd);
    return status;
}
