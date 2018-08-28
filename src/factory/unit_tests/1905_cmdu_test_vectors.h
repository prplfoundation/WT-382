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

#ifndef _1905_CMDU_TEST_VECTORS_H_
#define _1905_CMDU_TEST_VECTORS_H_

#include "1905_cmdus.h"
#include "1905_tlvs.h"

extern struct CMDU   x1905_cmdu_structure_001;
extern uint8_t        *x1905_cmdu_streams_001[];
extern uint16_t        x1905_cmdu_streams_len_001[];

extern struct CMDU   x1905_cmdu_structure_002;
extern uint8_t        *x1905_cmdu_streams_002[];
extern uint16_t        x1905_cmdu_streams_len_002[];

extern struct CMDU   x1905_cmdu_structure_003;
extern uint8_t        *x1905_cmdu_streams_003[];
extern uint16_t        x1905_cmdu_streams_len_003[];

extern struct CMDU   x1905_cmdu_structure_004;
extern uint8_t        *x1905_cmdu_streams_004[];
extern uint16_t        x1905_cmdu_streams_len_004[];

extern struct CMDU   x1905_cmdu_structure_005;
extern uint8_t        *x1905_cmdu_streams_005[];
extern uint16_t        x1905_cmdu_streams_len_005[];

/** @defgroup tv_cmdu_header CMDU header parsing test vectors
 */

/** @defgroup tv_cmdu_header_001 CMDU header with last fragment indicator
 *
 * @ingroup tv_cmdu_header
 * @{
 */
extern struct CMDU_header x1905_cmdu_header_001;
extern uint8_t            x1905_cmdu_packet_001[];
extern size_t             x1905_cmdu_packet_len_001;
/** @} */

/** @defgroup tv_cmdu_header_002 CMDU header without last fragment indicator
 *
 * @ingroup tv_cmdu_header
 * @{
 */
extern struct CMDU_header x1905_cmdu_header_002;
extern uint8_t            x1905_cmdu_packet_002[];
extern size_t             x1905_cmdu_packet_len_002;
/** @} */

/** @defgroup tv_cmdu_header_003 CMDU header with wrong ether type
 *
 * @ingroup tv_cmdu_header
 * @{
 */
extern uint8_t            x1905_cmdu_packet_003[];
extern size_t             x1905_cmdu_packet_len_003;
/** @} */

/** @defgroup tv_cmdu_header_004 CMDU header is too short
 *
 * @ingroup tv_cmdu_header
 * @{
 */
extern uint8_t            x1905_cmdu_packet_004[];
extern size_t             x1905_cmdu_packet_len_004;
/** @} */



#endif
