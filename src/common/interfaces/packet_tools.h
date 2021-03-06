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

#ifndef _PACKET_TOOLS_H_
#define _PACKET_TOOLS_H_

#include <tlv.h> // mac_address

#include <stdbool.h> // bool
#include <stddef.h>  // size_t
#include <stdint.h>
#include <string.h> // memcpy()

// Auxiliary functions to:
//
//   A) Extract 1, 2 or 4 bytes from a stream received from the network.
//
//   B) Insert  1, 2 or 4 bytes into a stream which is going to be sent into
//      the network.
//
// These functions do three things:
//
//   1. Avoid unaligned memory accesses (which might cause slowdowns or even
//      exceptions on some architectures)
//
//   2. Convert from network order to host order (and the other way)
//
//   3. Advance the packet pointer as many bytes as those which have just
//      been extracted/inserted.

// Extract/insert 1 byte
//
static inline void _E1B(uint8_t **packet_ppointer, uint8_t *memory_pointer)
{
    *memory_pointer     = **packet_ppointer;
    (*packet_ppointer) += 1;
}

static inline void _I1B(const uint8_t *memory_pointer, uint8_t **packet_ppointer)
{
    **packet_ppointer   = *memory_pointer;
    (*packet_ppointer) += 1;
}

static inline bool _E1BL(const uint8_t **packet_ppointer, uint8_t *memory_pointer, size_t *length)
{
    if (*length < 1)
    {
        return false;
    }
    else
    {
        _E1B((uint8_t **)packet_ppointer, memory_pointer);
        (*length) -= 1;
        return true;
    }
}

static inline bool _I1BL(const uint8_t *memory_pointer, uint8_t **packet_ppointer, size_t *length)
{
    if (*length < 1)
    {
        return false;
    }
    else
    {
        _I1B(memory_pointer, packet_ppointer);
        (*length) -= 1;
        return true;
    }
}


// Extract/insert 2 bytes
//
static inline void _E2B(uint8_t **packet_ppointer, uint16_t *memory_pointer)
{
#if _HOST_IS_BIG_ENDIAN_ == 1
    *(((uint8_t *)memory_pointer)+0)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+1)  = **packet_ppointer; (*packet_ppointer)++;
#elif _HOST_IS_LITTLE_ENDIAN_ == 1
    *(((uint8_t *)memory_pointer)+1)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+0)  = **packet_ppointer; (*packet_ppointer)++;
#else
#error You must specify your architecture endianess
#endif
}

static inline void _I2B(const uint16_t *memory_pointer, uint8_t **packet_ppointer)
{
#if _HOST_IS_BIG_ENDIAN_ == 1
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+0); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+1); (*packet_ppointer)++;
#elif _HOST_IS_LITTLE_ENDIAN_ == 1
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+1); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+0); (*packet_ppointer)++;
#else
#error You must specify your architecture endianess
#endif
}

static inline bool _E2BL(const uint8_t **packet_ppointer, uint16_t *memory_pointer, size_t *length)
{
    if (*length < 2)
    {
        return false;
    }
    else
    {
        _E2B((uint8_t **)packet_ppointer, memory_pointer);
        (*length) -= 2;
        return true;
    }
}

static inline bool _I2BL(const uint16_t *memory_pointer, uint8_t **packet_ppointer, size_t *length)
{
    if (*length < 2)
    {
        return false;
    }
    else
    {
        _I2B(memory_pointer, packet_ppointer);
        (*length) -= 2;
        return true;
    }
}


// Extract/insert 4 bytes
//
static inline void _E4B(uint8_t **packet_ppointer, uint32_t *memory_pointer)
{
#if _HOST_IS_BIG_ENDIAN_ == 1
    *(((uint8_t *)memory_pointer)+0)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+1)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+2)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+3)  = **packet_ppointer; (*packet_ppointer)++;
#elif _HOST_IS_LITTLE_ENDIAN_ == 1
    *(((uint8_t *)memory_pointer)+3)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+2)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+1)  = **packet_ppointer; (*packet_ppointer)++;
    *(((uint8_t *)memory_pointer)+0)  = **packet_ppointer; (*packet_ppointer)++;
#else
#error You must specify your architecture endianess
#endif
}

static inline void _I4B(const uint32_t *memory_pointer, uint8_t **packet_ppointer)
{
#if _HOST_IS_BIG_ENDIAN_ == 1
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+0); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+1); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+2); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+3); (*packet_ppointer)++;
#elif _HOST_IS_LITTLE_ENDIAN_ == 1
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+3); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+2); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+1); (*packet_ppointer)++;
    **packet_ppointer = *(((const uint8_t *)memory_pointer)+0); (*packet_ppointer)++;
#else
#error You must specify your architecture endianess
#endif
}

static inline bool _E4BL(const uint8_t **packet_ppointer, uint32_t *memory_pointer, size_t *length)
{
    if (*length < 4)
    {
        return false;
    }
    else
    {
        _E4B((uint8_t **)packet_ppointer, memory_pointer);
        (*length) -= 4;
        return true;
    }
}

static inline bool _I4BL(const uint32_t *memory_pointer, uint8_t **packet_ppointer, size_t *length)
{
    if (*length < 4)
    {
        return false;
    }
    else
    {
        _I4B(memory_pointer, packet_ppointer);
        (*length) -= 4;
        return true;
    }
}



// Extract/insert N bytes (ignore endianess)
//
static inline void _EnB(uint8_t **packet_ppointer, void *memory_pointer, uint32_t n)
{
    memcpy(memory_pointer, *packet_ppointer, n);
    (*packet_ppointer) += n;
}

static inline void _InB(const void *memory_pointer, uint8_t **packet_ppointer, uint32_t n)
{
    memcpy(*packet_ppointer, memory_pointer, n);
    (*packet_ppointer) += n;
}

static inline bool _EnBL(const uint8_t **packet_ppointer, void *memory_pointer, size_t n, size_t *length)
{
    if (*length < n)
    {
        return false;
    }
    else
    {
        _EnB((uint8_t **)packet_ppointer, memory_pointer, n);
        (*length) -= n;
        return true;
    }
}

static inline bool _InBL(const void *memory_pointer, uint8_t **packet_ppointer, size_t n, size_t *length)
{
    if (*length < n)
    {
        return false;
    }
    else
    {
        _InB(memory_pointer, packet_ppointer, n);
        (*length) -= n;
        return true;
    }
}

// Specific instances of _EnBL/_InBL for mac_addresses.
static inline bool _EmBL(const uint8_t **packet_ppointer, mac_address addr, size_t *length)
{
    return _EnBL(packet_ppointer, addr, 6, length);
}

static inline bool _ImBL(const mac_address addr, uint8_t **packet_ppointer, size_t *length)
{
    return _InBL(addr, packet_ppointer, 6, length);
}

#endif
