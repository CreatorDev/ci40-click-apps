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
#include <unistd.h>
#include <fcntl.h>

#define PATH_MAX    64
#define BUFFER_MAX  8

int PwmExport(unsigned int pwmNumber)
{
    int fd;
    int bytesWritten, status = 0;
    char buffer[BUFFER_MAX];

    fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open export for writing\n");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%u", pwmNumber);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to export pwm\n");
    }
    close(fd);
    return 0;
}

int PwmUnexport(unsigned int pwmNumber)
{
    int fd;
    int bytesWritten, status = 0;
    char buffer[BUFFER_MAX];

    fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open unexport for writing");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%u", pwmNumber);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to unexport pwm");
    }

    close(fd);
    return status;
}

int PwmPeriod(unsigned int pwmNumber, unsigned int period)
{
    int fd;
    char path[PATH_MAX];
    char buffer[BUFFER_MAX];
    int bytesWritten, status = 0;

    snprintf(path, PATH_MAX, "/sys/class/pwm/pwmchip0/pwm%u/period", pwmNumber);
    fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open pwm direction for writing\n");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%u", period);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to set pwm period\n");
    }
    close(fd);
    return status;
}

int PwmDutyCycle(unsigned int pwmNumber, unsigned int dutyCycle)
{
    int fd;
    char path[PATH_MAX];
    char buffer[BUFFER_MAX];
    int bytesWritten, status = 0;

    snprintf(path, PATH_MAX, "/sys/class/pwm/pwmchip0/pwm%u/duty_cycle", pwmNumber);
    fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open pwm duty_cycle for writing\n");
        return -1;
    }

    bytesWritten = snprintf(buffer, BUFFER_MAX, "%u", dutyCycle);
    status = write(fd, buffer, bytesWritten);
    if (status == -1)
    {
        fprintf(stderr, "Failed to set pwm duty_cycle\n");
    }
    close(fd);
    return status;
}

int PwmEnable(unsigned int pwmNumber)
{
    int fd;
    int status = 0;
    char path[PATH_MAX];

    snprintf(path, PATH_MAX, "/sys/class/pwm/pwmchip0/pwm%u/enable", pwmNumber);
    fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open pwm enable for writing\n");
        return -1;
    }

    status = write(fd, "1", sizeof("1"));
    if (status == -1)
    {
        fprintf(stderr, "Failed to set pwm enable\n");
    }
    close(fd);
    return status;
}
