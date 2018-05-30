/*
 *  Broadband Forum BUS (Broadband User Services) Work Area
 *
 *  Copyright (c) 2017, Broadband Forum
 *  Copyright (c) 2017, MaxLinear, Inc. and its affiliates
 *
 *  This is draft software, is subject to change, and has not been
 *  approved by members of the Broadband Forum. It is made available to
 *  non-members for internal study purposes only. For such study
 *  purposes, you have the right to make copies and modifications only
 *  for distributing this software internally within your organization
 *  among those who are working on it (redistribution outside of your
 *  organization for other than study purposes of the original or
 *  modified works is not permitted). For the avoidance of doubt, no
 *  patent rights are conferred by this license.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  Unless a different date is specified upon issuance of a draft
 *  software release, all member and non-member license rights under the
 *  draft software release will expire on the earliest to occur of (i)
 *  nine months from the date of issuance, (ii) the issuance of another
 *  version of the same software release, or (iii) the adoption of the
 *  draft software release as final.
 *
 *  ---
 *
 *  This version of this source file is part of the Broadband Forum
 *  WT-382 IEEE 1905.1/1a stack project.
 *
 *  Please follow the release link (given below) for further details
 *  of the release, e.g. license validity dates and availability of
 *  more recent draft or final releases.
 *
 *  Release name: WT-382_draft1
 *  Release link: https://www.broadband-forum.org/software#WT-382_draft1
 */

#include "aletest.h"

#include <platform.h>
#include <utils.h>

#include <linux/if_packet.h>  // sockaddr_ll
#include <net/if.h>           // struct ifreq, IFNAZSIZE
#include <netinet/ether.h>    // ETH_P_ALL, ETH_A_LEN
#include <poll.h>             // poll()
#include <sys/ioctl.h>        // ioctl(), SIOCGIFINDEX
#include <sys/socket.h>       // socket()
#include <time.h>             // clock_gettime()
#include <unistd.h>           // close()

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void dump_bytes(const uint8_t *buf, size_t buf_len, const char *indent)
{
    size_t i;
    int bytes_per_line = (80 - 1 - (int)strlen(indent)) / 3;
    int bytecount;

    /* If indent is too long, just print 8 bytes per line */
    if (bytes_per_line < 8)
        bytes_per_line = 8;

    for (i = 0; i < buf_len; /* i is incremented in inner loop */)
    {
        PLATFORM_PRINTF("%s", indent);
        for (bytecount = 0; bytecount < bytes_per_line && i < buf_len; bytecount++, i++)
        {
            PLATFORM_PRINTF(" %02x", buf[i]);
        }
        PLATFORM_PRINTF("\n");
    }
}

bool compare_masked(const uint8_t *buf, size_t buf_len, const maskedbyte_t *expected, size_t expected_len)
{
    size_t i;

    if (buf_len < expected_len) {
        PLATFORM_PRINTF_DEBUG_DETAIL("Buffer shorter than expected: %llu < %llu\n", (unsigned long long)buf_len,
                                     (unsigned long long)expected_len);
        return false;
    }

    for (i = 0; i < expected_len; i++) {
        uint8_t mask = ~((uint8_t)(expected[i] >> 8));
        /* Note: the mask implicitly has the effect of converting expected[i] to uint8_t */
        if ((buf[i] & mask) != (expected[i] & mask))
        {
            PLATFORM_PRINTF_DEBUG_DETAIL("Buffer differs at %llu: 0x%02x != 0x%02x mask 0x%02x\n",
                                         (unsigned long long)i, buf[i], expected[i], mask);
            return false;
        }
    }

    /* Remaining padding bytes must be 0 */
    for (; i < buf_len; i++) {
        if (buf[i] != 0)
        {
            PLATFORM_PRINTF_DEBUG_DETAIL("Buffer padding byte is not 0 at %llu: 0x%02x\n",
                                         (unsigned long long)i, buf[i]);
            return false;
        }
    }

    return true;
}

bool check_expected_bytes(const uint8_t *buf, size_t buf_len, const maskedbyte_t *expected, size_t expected_len,
                          const char *message, ...)
{
    if (!compare_masked(buf, buf_len, expected, expected_len)) {
        va_list ap;
        PLATFORM_PRINTF_DEBUG_INFO("Buffer does not match with expected: ");
        va_start(ap, message);
        vprintf(message, ap);
        va_end(ap);
        dump_bytes(buf, buf_len, " ");
        return false;
    } else {
        return true;
    }
}

int openPacketSocket(const char *interface_name, INT16U eth_type, struct sockaddr_ll *socket_address)
{
    int                 s;
    struct ifreq        ifr;
    int                 ifindex;

    PLATFORM_PRINTF_DEBUG_DETAIL("[PLATFORM] Opening interface '%s'\n", interface_name);

    s = socket(AF_PACKET, SOCK_RAW, eth_type);
    if (-1 == s)
    {
        PLATFORM_PRINTF_DEBUG_ERROR("[PLATFORM] socket('%s') returned with errno=%d (%s) while opening a RAW socket\n",
                                    interface_name, errno, strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ);
    if (ioctl(s, SIOCGIFINDEX, &ifr) == -1)
    {
          PLATFORM_PRINTF_DEBUG_ERROR("[PLATFORM] ioctl('%s',SIOCGIFINDEX) returned with errno=%d (%s) while opening a RAW socket\n",
                                      interface_name, errno, strerror(errno));
          close(s);
          return -1;
    }
    ifindex = ifr.ifr_ifindex;

    memset(socket_address, 0, sizeof(*socket_address));
    socket_address->sll_family   = AF_PACKET;
    socket_address->sll_ifindex  = ifindex;
    socket_address->sll_protocol = eth_type;

    if (-1 == bind(s, (struct sockaddr*)socket_address, sizeof(*socket_address)))
    {
        PLATFORM_PRINTF_DEBUG_ERROR("[PLATFORM] socket('%s') returned with errno=%d (%s) while binding a RAW socket\n",
                                    interface_name, errno, strerror(errno));
        close(s);
        return -1;
    }

    return s;
}

static int64_t get_time_ns()
{
    struct timespec t;
    /* We want real hardware time, but timer should be stopped while suspended (simulation) */
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (int64_t)t.tv_sec * 1000000000 + (int64_t)t.tv_nsec;
}

bool expect_packet(int s, const maskedbyte_t *expected, size_t expected_len, unsigned timeout_ms)
{
    int64_t deadline = get_time_ns() + (int64_t)timeout_ms * 1000000;
    int64_t remaining_ns;
    int remaining_ms;
    struct pollfd p = { .fd = s, .events = POLLIN, .revents = 0, };
    int poll_result;
    uint8_t buf[1500];
    ssize_t received;

    while (true) {
        remaining_ns = (deadline - get_time_ns());
        if (timeout_ms > 0) {
            if (remaining_ns <= 0)
            {
                PLATFORM_PRINTF_DEBUG_INFO("Timed out while expecting packet\n");
                return false;
            }
            remaining_ms = (int)(remaining_ns / 1000000);
            if (remaining_ms <= 0)
                remaining_ms = 1;
        } else {
            remaining_ms = 0;
        }
        poll_result = poll(&p, 1, remaining_ms);
        if (poll_result == 1)
        {
            received = recv(s, buf, sizeof(buf), 0);
            if (-1 == received)
            {
                PLATFORM_PRINTF_DEBUG_ERROR("Receive failed while expecting packet\n");
                return false;
            }
            if (compare_masked(buf, (size_t)received, expected, expected_len))
            {
                return true;
            }
            else
            {
                PLATFORM_PRINTF_DEBUG_DETAIL("Received something else than expected:");
                dump_bytes(buf, (size_t)received, " ");
            }
        }
        else if (poll_result < 0)
        {
            PLATFORM_PRINTF_DEBUG_ERROR("Poll error while expecting packet\n");
            return false;
        }
        // else check timeout again, poll may not be accurate enough.
    }
    // Unreachable
}
