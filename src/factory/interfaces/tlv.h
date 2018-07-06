/*
 * Copyright (C) 2018 prpl Foundation
 * Written by Arnout Vandecappelle (Essensium/Mind) <arnout@mind.be.
 *
 * License is still under discussion. For now, no license is granted, all rights reserved.
 */
#ifndef TLV_H
#define TLV_H

/** @file
 *
 * @brief Generic TLV interface.
 *
 * This file defines a generic TLV interface. Generic TLV parsing, forging, printing and comparison functions are
 * provided.
 *
 * The concrete TLV types are handled through the tlv_def structure. A TLV implementation must define the full array
 * of TLV types tlv_defs_t. Undefined types can be left as 0.
 *
 * The TLV functions take lists of TLVs, represented in the abstract tlv_list type. TLVs are always allocated and freed
 * as a full list.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // size_t

struct tlv_def;

/** @brief List of ::tlv%s.
 *
 * This is an abstract type. It is created by tlv_parse(), updated with tlv_add(), and deleted with tlv_free().
 */
struct tlv_list;

/** @brief Type-Length-Value object.
 *
 * This is an abstract type. It is created by tlv_parse(), updated with tlv_add(), and deleted with tlv_free(). It is
 * not intended to be manipulated directly.
 *
 * The intended use is as a member of the structure representing the specific type. E.g.
 * @code
 * struct tlv_foo
 * {
 *     struct tlv tlv;
 *     unsigned   bar;
 * };
 * @endcode
 * From a ::tlv object, the containing specific type can be obtained using container_of().
 *
 */
struct tlv
{
    uint8_t type; /**< @private */
};

/** @brief Unknown TLV.
 *
 * If an unknown TLV type is encountered while parsing a TLV buffer, an object of this type is returned.
 */
struct tlv_unknown
{
    struct tlv tlv;  /**< @brief The TLV type. */
    uint16_t length; /**< @brief The length. */
    uint8_t *value;  /**< @brief The uninterpreted value. */
};

/** @brief Definition of a TLV type.
 *
 * For a 0-length TLV, only tlv_def::type and tlv_def::name must be set.
 */
struct tlv_def
{
    /** @brief The type identifier. */
    uint8_t type;

    /** @brief Human-readable name.
     *
     * Must be set to identify a valid type, NULL for an unknown type.
     */
    const char *name;

    /** @brief TLV parse virtual function.
     *
     * @param def The TLV type definition. The same parse function may be used for different types.
     *
     * @param buffer The value part of the TLV.
     *
     * @param length The length of the value.
     *
     * @return The @a tlv member of a newly allocated TLV structure. NULL in case of error.
     *
     * This function must create a new TLV structure and initialise its value. The type and length are already parsed
     * out of the @a buffer, so it points directly to the value.
     *
     * If the TLV is zero-length (and the full TLV structure only contains a ::tlv member), it is not necessary to
     * implement this function.
     */
    struct tlv *(*parse)(const struct tlv_def *def, const uint8_t *buffer, size_t length);

    /** @brief TLV length virtual function.
     *
     * @param tlv The TLV to forge.
     *
     * @return the length of the TLV value (i.e., without type and length fields).
     *
     * This function is called when forging a TLV list, to determine the size of the buffer that must be allocated.
     *
     * If left as NULL, defaults to 0 length.
     */
    uint16_t (*length)(const struct tlv *tlv);

    /** @brief TLV forge virtual function.
     *
     * @param tlv The TLV to forge.
     *
     * @param buf Buffer in which to forge the value.
     *
     * @param length Remaining length of @a buf.
     *
     * @return true. If false is returned, it's a programming error: either tlv_def::length returned a wrong value, or
     * the structure was not consistent.
     *
     * This function is called when forging a TLV list, after allocating the buffer based on the calls to
     * tlv_def::length. Note that @a length is the total length of the buffer, not just for this TLV.
     *
     * Use _I1BL() and friends to fill @buf and update @length.
     *
     * May be left as NULL for 0 length TLVs. In that case, also tlv_def::length must be NULL.
     */
    bool (*forge)(const struct tlv *tlv, uint8_t **buf, size_t *length);

    /** @brief TLV print virtual function.
     *
     * @param tlv The TLV to print.
     *
     * @param write_function The print callback.
     *
     * @param prefix Prefix to be added to every line. This prefix will contain the TLV type name.
     *
     * May be left as NULL for 0 length TLVs or where the value is not relevant to print.
     */
    void (*print)(const struct tlv *tlv, void (*write_function)(const char *fmt, ...), const char *prefix);

    /** @brief TLV delete virtual function.
     *
     * @param tlv The TLV to delete.
     *
     * This function must delete the TLV and everything allocated by the tlv_def::parse function.
     *
     * May be left as NULL if tlv_def::parse only allocates a single structure.
     */
    void (*free)(struct tlv *tlv);

    /** @brief TLV comparison virtual function.
     *
     * @param tlv1 The left-hand side TLV to compare.
     *
     * @param tlv2 The right-hand side TLV to compare.
     *
     * @return true if tlv1 is equal to tlv2, false if they differ.
     *
     * This function is called to compare TLVs. The type of @a tlv1 and @a tlv2 are guaranteed to be equal.
     *
     * May be left as NULL for 0 length TLVs.
     */
    bool (*compare)(const struct tlv *tlv1, const struct tlv *tlv2);

    /** @brief TLV aggregation virtual function.
     *
     * @param tlv1 The existing TLV.
     *
     * @param tlv2 The new TLV to be aggregated with it.
     *
     * @return @a tlv1, or a reallocation of @a tlv1 (i.e. in that case @a tlv1 must have been deleted), or NULL in
     * case the TLVs couldn't be aggregated.
     *
     * In some cases, a TLV may be split over several packets and therefore occur twice when parsing. This function is
     * called to handle such a case. Note that this only applies for TLVs with some dynamic content, e.g. a list of
     * addresses. For fixed TLVs, it's an error if the same TLV occurs twice.
     *
     * May be left as NULL for TLVs that can't be aggregated.
     *
     * @todo Not implemented yet.
     */
    struct tlv *(*aggregate)(struct tlv *tlv1, const struct tlv *tlv2);
};

/** @brief Definition of TLV metadata.
 *
 * To define concrete TLVs, the callback functions must be defined for each defined type.
 */
typedef const struct tlv_def tlv_defs_t[0x100];

/** @brief Find the definition of a specific TLV type. */
const struct tlv_def *tlv_find_def(tlv_defs_t defs, uint8_t tlv_type);

/** @brief Find the definition corresponding to a specific TLV. */
const struct tlv_def *tlv_find_tlv_def(tlv_defs_t defs, const struct tlv *tlv);

/** @brief Append a TLV to a list of TLVs.
 *
 * @param defs The TLV metadata.
 *
 * @param tlvs The TLV list to append to.
 *
 * @param tlv The TLV to append to @a tlvs.
 *
 * @return true if successful, false if failed.
 *
 * This function may fail when a TLV of type @a tlv already exists in @a tlvs and they can't be aggregated.
 */
bool tlv_add(tlv_defs_t defs, struct tlv_list *tlvs, struct tlv *tlv);

/** @brief Parse a list of TLVs.
 *
 * @param defs The TLV metadata.
 *
 * @param buffer The buffer to parse (starting at the first TLV).
 *
 * @param length The length of @a buffer.
 *
 * @return NULL in case of error, or a list of TLVs.
 */
struct tlv_list *tlv_parse(tlv_defs_t defs, const uint8_t *buffer, size_t length);

/** @brief Forge a list of TLVs.
 *
 * @param defs The TLV metadata.
 *
 * @param tlvs The tlvs to forge.
 *
 * @param max_length The maximum length of a single packet.
 *
 * @param[out] buffer The forged packets.
 *
 * @param[out] length The lengths of the forged packets.
 *
 * @return true if successful, false if not.
 *
 * If this function returns false, it's most likely a programming error.
 *
 * @todo Fragmentation is not implemented at the moment.
 *
 * @todo The allocated buffers should have some headroom, so they don't have to be copied again to from a full packet.
 */
bool tlv_forge(tlv_defs_t defs, const struct tlv_list *tlvs, size_t max_length, uint8_t **buffer, size_t *length);

/** @brief Print a list of TLVs.
 *
 * @param defs The TLV metadata.
 *
 * @param tlvs The tlvs to print.
 *
 * @param write_function The print callback.
 *
 * @param prefix Prefix to be added to every line.
 */
void tlv_print(tlv_defs_t defs, const struct tlv_list *tlvs, void (*write_function)(const char *fmt, ...), const char *prefix);

/** @brief Delete a list of TLVs.
 *
 * @param defs The TLV metadata.
 *
 * @param tlvs The tlvs to delete.
 *
 * This function deletes each ::tlv and also the ::tlv_list.
 */
void tlv_free(tlv_defs_t defs, struct tlv_list *tlvs);

/** @brief Compare two TLV lists.
 *
 * @param tlvs1 The left-hand side list of TLVs to compare.
 *
 * @param tlvs2 The right-hand side list of TLVs to compare.
 *
 * @return true if each TLV in @a tlvs1 is equal to the corresponding one in @a tlvs2, false if they differ.
 *
 * @todo Currently the lists are assumed to be ordered in the same way.
 */
bool tlv_compare(tlv_defs_t defs, const struct tlv_list *tlvs1, const struct tlv_list *tlvs2);

#endif // TLV_H
