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

#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

/** @file
 *
 * Platform-specific functions and data structures that are only available on Linux platforms.
 *
 * Only linux-specific source files (i.e., files in the src_linux directory) may include this header.
 */

/** @brief Get the interface index by name.
 *
 * @param[in] interface_name The name of the interface on which to listen.
 * @return The interface index on success, or -1 on error (errno will be set).
 *
 * The interface index can be used to set options on the interface and to bind a socket to the interface.
 */
int getIfIndex(const char *interface_name);

/** @brief Open a socket suitable for raw ethernet packets.
 *
 * @param[in] ifindex The interface index on which to listen (as returned by getIfIndex()).
 * @param[in] eth_type The protocol number (in host byte order)
 * @return The socket file descriptor on success, or -1 on error (errno will be set).
 *
 * The socket is created as SOCK_RAW, so the ethernet header must be added to / removed from the payload.
 *
 * The created socket will be bound to the given interface and protocol.
 *
 * Close the socket with close() when done.
 *
 * No messages are printed in case of error, but errno will be set upon return.
 *
 * @todo factor with the AL server itself.
 */
int openPacketSocket(int ifindex, uint16_t eth_type);



#endif // PLATFORM_LINUX_H
