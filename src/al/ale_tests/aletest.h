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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /* size_t */

/** Print the contents of @a buf, wrapping at 80 characters, indent every line with @a indent + 1 space */
void dump_bytes(const uint8_t *buf, size_t buf_len, const char *indent);

/** @brief Byte + mask combination.
 *
 * The 8 most significant bits of this type are the inverse of a bitmask, the 8 LSB are the bits to mask against.
 *
 * This representation is very compact for the common case where we want to check all bits, because then the mask is 0
 * and we can just put the byte we want to check.
 */
typedef uint16_t maskedbyte_t;

/** @brief Compare masked bytes.
 *
 * @param buf The buffer to check.
 * @param buf_len Length of @a buf.
 * @param expected The expected bytes (including their mask).
 * @param expected_len The expected length. @a buf_len may be larger than @a expected_len, the rest must be 0 bytes.
 * @return false if @a buf differs from @a expected, taking intou account the mask.
 */
bool compare_masked(const uint8_t *buf, size_t buf_len, const maskedbyte_t *expected, size_t expected_len);

/** @brief Verify that received bytes are what is expected.
 *
 * @param buf The buffer to check.
 * @param buf_len Length of @a buf.
 * @param expected The expected bytes (including their mask).
 * @param expected_len The expected length. @a buf_len may be larger than @a expected_len, the rest must be 0 bytes.
 * @param message The message to be printed in case of failure, with additional printf arguments.
 * @return false in case of failure.
 *
 * In case of failure, the @a message is printed and @a buf is dumped.
 */
bool check_expected_bytes(const uint8_t *buf, size_t buf_len, const maskedbyte_t *expected, size_t expected_len,
                          const char *message, ...) __attribute__((format(printf, 5, 6)));

/** @brief Expect a packet on socket @a s, with timeout.
 *
 * This function receives and discards packets from socket @a s, until either a packet is received that matches @a
 * expected with length @a expected_len, or the timeout is reached.
 *
 * @return true if the expected packet was received, @false if not.
 *
 * @note This function has no way to report that packets were discarded, or if there is an error on the socket.
 */
bool expect_packet(int s, const maskedbyte_t *expected, size_t expected_len, unsigned timeout_ms);

/** Wrapper around expect_packet() that covers the common case */
#define CHECK_EXPECT_PACKET(s, expected, timeout_ms, result) \
    do { \
        if (expect_packet(s, expected, ARRAY_SIZE(expected), timeout_ms)) { \
            PLATFORM_PRINTF_DEBUG_INFO("Received expected " #expected "\n"); \
        } else { \
            PLATFORM_PRINTF_DEBUG_ERROR("<- Did not receive " #expected " within " #timeout_ms " ms\n"); \
            (result)++; \
        } \
    } while (0)

#endif

