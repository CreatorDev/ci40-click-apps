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

/**
 * @brief Library for gpio operations.
 */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <inttypes.h>
#include "gpio.h"

#define PATH_MAX    64
#define BUFFER_MAX  8

int8_t GpioExport(uint8_t pin)
{
    int fd;
    int8_t bytesWritten, status = 0;
    char buffer[BUFFER_MAX];

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open export for writing\n");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%" PRIu8, pin);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to export gpio\n");
    }

    close(fd);
    return status;
}

int8_t GpioUnexport(uint8_t pin)
{
    int fd;
    int8_t bytesWritten, status = 0;
    char buffer[BUFFER_MAX];

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open unexport for writing");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%" PRIu8, pin);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to unexport gpio");
    }

    close(fd);
    return status;
}

int8_t GpioDirection(uint8_t pin, uint8_t direction)
{
    int fd;
    int8_t status = 0;
    char* directionsStr[] = {"in", "out"};
    char path[PATH_MAX];

    snprintf(path, PATH_MAX, "/sys/class/gpio/gpio%" PRIu8 "/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open gpio direction for writing\n");
        return -1;
    }

    status = write(fd, directionsStr[direction], strlen(directionsStr[direction]));
    if (status == -1)
    {
        fprintf(stderr, "Failed to set gpio direction\n");
    }

    close(fd);
    return status;
}

int8_t GpioEdge(uint8_t pin, uint8_t type)
{
    int fd;
    int8_t status = 0;
    char* typeStr[] = {"rising", "falling"};
    char path[PATH_MAX];

    snprintf(path, PATH_MAX, "/sys/class/gpio/gpio%" PRIu8 "/edge", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open gpio edge for writing\n");
        return -1;
    }

    status = write(fd, typeStr[type], strlen(typeStr[type]));
    if (status == -1)
    {
        fprintf(stderr, "Failed to set gpio edge type\n");
    }

    close(fd);
    return status;
}

int8_t GpioWaitForInterrupt(uint8_t gpio, uint8_t timeout)
{
    int fd;
    int8_t status;
    struct pollfd pfd;
    char path[PATH_MAX];
    char buffer[BUFFER_MAX] = {0};

    sprintf(path, "/sys/class/gpio/gpio%" PRIu8 "/value", gpio);
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        perror("error: failed to open gpio\n");
        return -1;
    }

    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    pfd.events = POLLPRI;

    lseek(pfd.fd, 0 , SEEK_SET);
    status = read(fd, buffer, sizeof(buffer));
    if (status < 0)
    {
        perror("read()");
        close(fd);
        return -1;
    }

    status = poll(&pfd, 1, timeout * 1000);
    if (status == -1)
    {
        perror("poll()");
    }
    else if (status == 0)
    {
        printf("timeout\n");
    }

    close(fd);
    return status;
}
