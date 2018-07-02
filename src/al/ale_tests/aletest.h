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

#ifndef _ALETEST_H_
#define _ALETEST_H_

#include <platform.h> /* PLATFORM_PRINTF_* */
#include <1905_cmdus.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /* size_t */

#define ADDR_AL "\x02\xee\xff\x33\x44\x00"
#define ADDR_MAC0 "\x00\xee\xff\x33\x44\x00"
#define ADDR_MAC1 "\x00\xee\xff\x33\x44\x10"
#define ADDR_MAC2 "\x00\xee\xff\x33\x44\x20"
#define ADDR_MAC3 "\x00\xee\xff\x33\x44\x30"

#define ADDR_AL_PEER0 "\x02\xaa\xbb\x33\x44\x00"
#define ADDR_AL_PEER1 "\x02\xaa\xbb\x33\x44\x10"
#define ADDR_AL_PEER2 "\x02\xaa\xbb\x33\x44\x20"
#define ADDR_AL_PEER3 "\x02\xaa\xbb\x33\x44\x30"
#define ADDR_MAC_PEER0 "\x00\xee\xff\x33\x44\x01"
#define ADDR_MAC_PEER1 "\x00\xee\xff\x33\x44\x11"
#define ADDR_MAC_PEER2 "\x00\xee\xff\x33\x44\x21"
#define ADDR_MAC_PEER3 "\x00\xee\xff\x33\x44\x31"

/** Print the contents of @a buf, wrapping at 80 characters, indent every line with @a indent + 1 space */
void dump_bytes(const void *buf, size_t buf_len, const char *indent);

struct CMDU *expect_cmdu(int s, unsigned timeout_ms, const char *testname, uint16_t expected_cmdu_type,
                         mac_address expected_src_addr, mac_address expected_src_al_addr, mac_address expected_dst_address);

int expect_cmdu_match(int s, unsigned timeout_ms, const char *testname, const struct CMDU *expected_cmdu,
                      mac_address expected_src_addr, mac_address expected_src_al_addr, mac_address expected_dst_address);

/** Send a CMDU. If it fails, print an error and return a value >= 1, else return 0. */
int send_cmdu(int s, mac_address dst_addr, mac_address src_addr, const struct CMDU *cmdu);

#endif

