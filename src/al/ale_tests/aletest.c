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
#include <1905_l2.h>

#include <poll.h>             // poll()
#include <time.h>             // clock_gettime()
#include <unistd.h>           // close()
#include <sys/types.h>        // recv()
#include <sys/socket.h>       // recv()

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

static int64_t get_time_ns()
{
    struct timespec t;
    /* We want real hardware time, but timer should be stopped while suspended (simulation) */
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (int64_t)t.tv_sec * 1000000000 + (int64_t)t.tv_nsec;
}

struct CMDU *expect_cmdu(int s, unsigned timeout_ms, const char *testname, uint16_t expected_cmdu_type,
                         mac_address expected_src_addr, mac_address expected_src_al_addr, mac_address expected_dst_address)
{
    int64_t deadline = get_time_ns() + (int64_t)timeout_ms * 1000000;
    int64_t remaining_ns;
    int remaining_ms;
    struct pollfd p = { .fd = s, .events = POLLIN, .revents = 0, };
    int poll_result;
    uint8_t buf[1500];
    ssize_t received;
    struct CMDU_header cmdu_header;

    while (true) {
        remaining_ns = (deadline - get_time_ns());
        if (timeout_ms > 0) {
            if (remaining_ns <= 0)
            {
                PLATFORM_PRINTF_DEBUG_INFO("Timed out while expecting %s\n", testname);
                return NULL;
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
                PLATFORM_PRINTF_DEBUG_ERROR("Receive failed while expecting %s\n", testname);
                return NULL;
            }
            if (!parse_1905_CMDU_header_from_packet((uint8_t*)buf, (size_t) received, &cmdu_header))
            {
                PLATFORM_PRINTF_DEBUG_ERROR("Failed to parse CMDU header while expecting %s\n", testname);
                PLATFORM_PRINTF_DEBUG_DETAIL("  Received:\n");
                dump_bytes(buf, (size_t)received, "   ");
            }
            else if (expected_cmdu_type != cmdu_header.message_type)
            {
                PLATFORM_PRINTF_DEBUG_INFO("Received CMDU of type 0x%04x while expecting %s\n",
                                           cmdu_header.message_type, testname);
            }
            else if (0 != memcmp(expected_dst_address, cmdu_header.dst_addr, 6))
            {
                PLATFORM_PRINTF_DEBUG_INFO("Received CMDU with destination " MACSTR " while expecting %s\n",
                                           MAC2STR(cmdu_header.dst_addr), testname);
            }
            else if (0 != memcmp(expected_src_addr, cmdu_header.src_addr, 6) &&
                     0 != memcmp(expected_src_al_addr, cmdu_header.src_addr, 6))
            {
                PLATFORM_PRINTF_DEBUG_INFO("Received CMDU with source " MACSTR " while expecting %s\n",
                                           MAC2STR(cmdu_header.src_addr), testname);
            }
            else
            {
                INT8U *packets[] = {(INT8U *)buf + (6+6+2), NULL};
                struct CMDU *cmdu = parse_1905_CMDU_from_packets(packets);
                if (NULL == cmdu)
                {
                    PLATFORM_PRINTF_DEBUG_ERROR("Failed to parse CMDU %s\n", testname);
                    return NULL;
                }
                else
                {
                    return cmdu;
                }
            }
        }
        else if (poll_result < 0)
        {
            PLATFORM_PRINTF_DEBUG_ERROR("Poll error while expecting packet\n");
            return NULL;
        }
        // else check timeout again, poll may not be accurate enough.
    }
    // Unreachable
}

int expect_cmdu_match(int s, unsigned timeout_ms, const char *testname, const struct CMDU *expected_cmdu,
                      mac_address expected_src_addr, mac_address expected_src_al_addr, mac_address expected_dst_address)
{
    int ret = 1;
    struct CMDU *cmdu = expect_cmdu(s, timeout_ms, testname, expected_cmdu->message_type,
                                    expected_src_addr, expected_src_al_addr, expected_dst_address);
    if (NULL != cmdu)
    {
        cmdu->message_id = expected_cmdu->message_id;
        if (0 != compare_1905_CMDU_structures(cmdu, expected_cmdu))
        {
            PLATFORM_PRINTF_DEBUG_ERROR("Received something else than expected %s\n", testname);
            PLATFORM_PRINTF_DEBUG_INFO("  Expected CMDU:\n");
            visit_1905_CMDU_structure(expected_cmdu, print_callback, PLATFORM_PRINTF_DEBUG_INFO, "");
            PLATFORM_PRINTF_DEBUG_INFO("  Received CMDU:\n");
            visit_1905_CMDU_structure(cmdu, print_callback, PLATFORM_PRINTF_DEBUG_INFO, "");
        }
        else
        {
            PLATFORM_PRINTF_DEBUG_DETAIL("Received expected %s\n", testname);
            ret = 0;
        }
        free_1905_CMDU_structure(cmdu);
    }
    return ret;
}

int send_cmdu(int s, mac_address dst_addr, mac_address src_addr, const struct CMDU *cmdu)
{
    INT8U  **streams;
    INT16U  *streams_lens;
    int i;
    int ret = 0;

    streams = forge_1905_CMDU_from_structure(cmdu, &streams_lens);

    if (streams == NULL)
    {
        PLATFORM_PRINTF_DEBUG_ERROR("Failed to forge CMDU:\n");
        visit_1905_CMDU_structure(cmdu, print_callback, PLATFORM_PRINTF_DEBUG_ERROR, "  ");
        return 1;
    }

    for (i = 0; streams[i] != NULL; i++)
    {
        uint8_t *buf = malloc(streams_lens[i] + 6 + 6 + 2);
        memcpy (buf, dst_addr, 6);
        memcpy (buf + 6, src_addr, 6);
        buf[6+6] = 0xff & (ETHERTYPE_1905 >> 8);
        buf[6+6+1] = 0xff & (ETHERTYPE_1905);
        memcpy(buf + 6 + 6 + 2, streams[i], streams_lens[i]);

        if (-1 == send(s, buf, streams_lens[i] + 6 + 6 + 2, 0))
        {
            PLATFORM_PRINTF_DEBUG_ERROR("Packet could not be sent!\n");
            ret++;
        }
        free(buf);
    }

    free_1905_CMDU_packets(streams);
    PLATFORM_FREE(streams_lens);
    return ret;
}
