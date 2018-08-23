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

#ifndef INTERFACE_H
#define INTERFACE_H

#include <tlv.h>

#include <stdbool.h> // bool
#include <stddef.h>  // size_t

/** @file
 *
 * Definition of interfaces.
 *
 * Interface control can be implemented in various ways. The ::interface struct collects the functions needed to
 * get statistics from an interface and to control it.
 */

/** @brief Defintion of a BSS. */
struct bss_info {
    mac_address bssid;
    const char *ssid;
};

/** @brief Definition of an interface
 *
 * The interface stores some information, but mostly the information is retrieved through callback functions.
 */
struct interface
{
    /** @brief Interface name, e.g. eth0. */
    char  *name;
    /** @brief Interface address or Radio Unique Identifier. */
    mac_address addr;
    /** @brief List of BSS for which this interface is an AP.
     *
     * The number of elements in this list is given by num_bss.
     *
     * If the interface is not an AP or no BSS are configured on it, this is NULL and num_bss is 0.
     */
    struct bss_info *bss_info;
    /** @brief Number of elements in bss_info. */
    size_t num_bss;

    /** @brief Implementation callback to fill bss_info.
     *
     * The implementation of this function must set or update the bss_info and num_bss fields of @a interface.
     *
     * This function is called at startup.
     *
     * Interfaces that are not access points can leave this as NULL, in which case bss_info will stay NULL as well.
     *
     * @param interface The interface for which to get BSS info.
     * @return true If bss_info was updated, false if not.
     *
     * @todo Foresee a way to update BSS info dynamically.
     */
    bool (*get_bss_info) (struct interface *interface);

    /** @brief Implementation callback to block a station from a specific BSS
     *
     * This function is called when an authenticated controller sends a Client steering request to block clients.
     *
     * @param interface The interface to which the steering request applies.
     * @param bss The BSS to which the steering request applies.
     * @param clients The list of clients to block.
     * @param num_clients The number of elements in @a clients.
     *
     * @return true if the block request succeeded, false if it failed.
     */
    bool (*block_client) (
            struct interface *interface,
            const struct bss_info *bss,
            mac_address *clients,
            size_t num_clients);
};


#endif // INTERFACE_H
