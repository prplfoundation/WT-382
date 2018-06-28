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

//
// This file tests the "parse_1905_CMDU_from_packets()" function by providing
// some test input streams and checking the generated output structure.
//

#include "platform.h"
#include "utils.h"

#include "1905_tlvs.h"
#include "1905_cmdus.h"
#include "1905_cmdu_test_vectors.h"

#include <string.h> // memcmp

static int check_parse_1905_cmdu(const char *test_description, INT8U **input, struct CMDU *expected_output)
{
    int result;
    struct CMDU *real_output;

    real_output = parse_1905_CMDU_from_packets(input);

    if (0 == compare_1905_CMDU_structures(real_output, expected_output))
    {
        result = 0;
        PLATFORM_PRINTF("%-100s: OK\n", test_description);
    }
    else
    {
        result = 1;
        PLATFORM_PRINTF("%-100s: KO !!!\n", test_description);
        PLATFORM_PRINTF("  Expected output:\n");
        visit_1905_CMDU_structure(expected_output, print_callback, PLATFORM_PRINTF, "");
        PLATFORM_PRINTF("  Real output    :\n");
        visit_1905_CMDU_structure(real_output, print_callback, PLATFORM_PRINTF, "");
    }

    return result;
}

static int check_parse_1905_cmdu_header(const char *test_description, uint8_t *input, size_t input_len,
                                        struct CMDU_header *expected_output)
{
    int result = 1;
    struct CMDU_header real_output;

    memset(&real_output, 0x42, sizeof(real_output));

    if (parse_1905_CMDU_header_from_packet(input, input_len, &real_output))
    {
        if (NULL != expected_output)
        {
            if (0 == memcmp(expected_output, &real_output, sizeof(real_output)))
            {
                result = 0;
            }
        }
        // Else failed because we expected parse to fail
    }
    else
    {
        if (NULL == expected_output)
        {
            result = 0;
        }
    }

    if (0 == result)
    {
        PLATFORM_PRINTF("%-100s: OK\n", test_description);
    }
    else
    {
        PLATFORM_PRINTF("%-100s: KO !!!\n", test_description);
        if (NULL != expected_output)
        {
            PLATFORM_PRINTF("  Expected output:\n    dst_addr: " MACSTR "\n    src_addr: " MACSTR "\n"
                            "    MID: 0x%04x FID: 0x%02x Last fragment: %d\n",
                            MAC2STR(expected_output->dst_addr), MAC2STR(expected_output->src_addr),
                            expected_output->mid, expected_output->fragment_id, expected_output->last_fragment_indicator);
        }
        PLATFORM_PRINTF("  Real output:\n    dst_addr: " MACSTR "\n    src_addr: " MACSTR "\n"
                        "    MID: 0x%04x FID: 0x%02x Last fragment: %d\n",
                        MAC2STR(real_output.dst_addr), MAC2STR(real_output.src_addr),
                        real_output.mid, real_output.fragment_id, real_output.last_fragment_indicator);
    }

    return result;
}

int main(void)
{
    int result = 0;

    #define x1905CMDUPARSE001 "x1905CMDUPARSE001 - Parse link metric query CMDU (x1905_cmdu_streams_001)"
    result += check_parse_1905_cmdu(x1905CMDUPARSE001, x1905_cmdu_streams_001, &x1905_cmdu_structure_001);

    #define x1905CMDUPARSE002 "x1905CMDUPARSE002 - Parse link metric query CMDU (x1905_cmdu_streams_002)"
    result += check_parse_1905_cmdu(x1905CMDUPARSE002, x1905_cmdu_streams_002, &x1905_cmdu_structure_002);

    #define x1905CMDUPARSE003 "x1905CMDUPARSE003 - Parse link metric query CMDU (x1905_cmdu_streams_004)"
    result += check_parse_1905_cmdu(x1905CMDUPARSE003, x1905_cmdu_streams_004, &x1905_cmdu_structure_004);

    #define x1905CMDUPARSE004 "x1905CMDUPARSE004 - Parse topology query CMDU (x1905_cmdu_streams_005)"
    result += check_parse_1905_cmdu(x1905CMDUPARSE004, x1905_cmdu_streams_005, &x1905_cmdu_structure_005);

    result += check_parse_1905_cmdu_header("x1905CMDUPARSEHDR001 - Parse CMDU packet last fragment",
                                           x1905_cmdu_packet_001, x1905_cmdu_packet_len_001, &x1905_cmdu_header_001);

    result += check_parse_1905_cmdu_header("x1905CMDUPARSEHDR002 - Parse CMDU packet not last fragment",
                                           x1905_cmdu_packet_002, x1905_cmdu_packet_len_002, &x1905_cmdu_header_002);

    result += check_parse_1905_cmdu_header("x1905CMDUPARSEHDR003 - Parse CMDU packet wrong ethertype",
                                           x1905_cmdu_packet_003, x1905_cmdu_packet_len_003, NULL);

    result += check_parse_1905_cmdu_header("x1905CMDUPARSEHDR004 - Parse CMDU packet too short",
                                           x1905_cmdu_packet_004, x1905_cmdu_packet_len_004, NULL);

    // Return the number of test cases that failed
    //
    return result;
}







