// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __UFORMATTEDVALUE_H__
#define __UFORMATTEDVALUE_H__

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/ufieldpositer.h"


/**
 * All possible field categories in ICU. Every entry in this enum corresponds
 * to another enum that exists in ICU.
 *
 * @draft ICU 64
 */
typedef enum UFieldCategory {
    /**
     * For an undefined field category.
     * 
     * @draft ICU 64
     */
    UFIELD_CATEGORY_UNDEFINED = 0,

    /**
     * For fields in UDateFormatField (udat.h), from ICU 3.0.
     *
     * @draft ICU 64
     */
    UFIELD_CATEGORY_DATE,

    /**
     * For fields in UNumberFormatFields (unum.h), from ICU 49.
     *
     * @draft ICU 64
     */
    UFIELD_CATEGORY_NUMBER,

    /**
     * For fields in UListFormatterField (ulistformatter.h), from ICU 63.
     *
     * @draft ICU 64
     */
    UFIELD_CATEGORY_LIST,

} UFieldCategory;


/**
 * Represents the type of constraint for ConstrainedFieldPosition.
 *
 * Constraints are used to control the behavior of iteration in FormattedValue.
 *
 * @draft ICU 64
 */
typedef enum UCFPosConstraintType {
    /**
     * Represents the lack of a constraint.
     *
     * @draft ICU 64
     */
    UCFPOS_CONSTRAINT_NONE,

    /**
     * Represents that the field category is constrained.
     *
     * Use getCategory to access the category. FormattedValue implementations
     * should not change that values while this constraint is active.
     *
     * @draft ICU 64
     */
    UCFPOS_CONSTRAINT_CATEGORY,

    /**
     * Represents that the field and field category are constrained.
     *
     * Use getCategory and getField to access the category and field.
     * FormattedValue implementations should not change those values while
     * this constraint is active.
     *
     * @draft ICU 64
     */
    UCFPOS_CONSTRAINT_FIELD
} UCFPosConstraintType;


struct UConstrainedFieldPosition;
/**
 * Represents a span of a string containing a given field.
 *
 * This struct differs from UFieldPosition in the following ways:
 *
 *   1. It has information on the field category.
 *   2. It allows you to set constraints to use when iterating over field positions.
 *   3. It is used for the newer FormattedValue APIs.
 *
 * @draft ICU 64
 */
typedef struct UConstrainedFieldPosition UConstrainedFieldPosition;


/**
 * Creates a new UConstrainedFieldPosition.
 *
 * By default, the UConstrainedFieldPosition has no iteration constraints.
 *
 * @param ec Set if an error occurs.
 * @return The new object, or NULL if an error occurs.
 * @draft ICU 64
 */
U_DRAFT UConstrainedFieldPosition* U_EXPORT2
ucfpos_open(UErrorCode* ec);


/**
 * Resets a UConstrainedFieldPosition to its initial state, as if it were newly created.
 *
 * Removes any constraints that may have been set on the instance.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_reset(UConstrainedFieldPosition* ucfpos);


/**
 * Destroys a UConstrainedFieldPosition and releases its memory.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_close(UConstrainedFieldPosition* ucfpos);


/**
 * Sets a constraint on the field category.
 * 
 * When this instance of UConstrainedFieldPosition is passed to ufmtval_nextPosition,
 * positions are skipped unless they have the given category.
 *
 * Any previously set constraints are cleared.
 *
 * For example, to loop over only the number-related fields:
 *
 *     UConstrainedFieldPosition* ucfpos = ucfpos_open(ec);
 *     ucfpos_constrainCategory(ucfpos, UFIELDCATEGORY_NUMBER_FORMAT, ec);
 *     while (ufmtval_nextPosition(ufmtval, ucfpos, ec)) {
 *         // handle the number-related field position
 *     }
 *     ucfpos_close(ucfpos);
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param category The field category to fix when iterating.
 * @param ec Set if an error occurs.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_constrainCategory(
    UConstrainedFieldPosition* ucfpos,
    UFieldCategory category,
    UErrorCode* ec);


/**
 * Sets a constraint on the category and field.
 * 
 * When this instance of UConstrainedFieldPosition is passed to ufmtval_nextPosition,
 * positions are skipped unless they have the given category and field.
 *
 * Any previously set constraints are cleared.
 *
 * For example, to loop over all grouping separators:
 *
 *     UConstrainedFieldPosition* ucfpos = ucfpos_open(ec);
 *     ucfpos_constrainField(ucfpos, UFIELDCATEGORY_NUMBER_FORMAT, UNUM_GROUPING_SEPARATOR_FIELD, ec);
 *     while (ufmtval_nextPosition(ufmtval, ucfpos, ec)) {
 *         // handle the grouping separator position
 *     }
 *     ucfpos_close(ucfpos);
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param category The field category to fix when iterating.
 * @param field The field to fix when iterating.
 * @param ec Set if an error occurs.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_constrainField(
    UConstrainedFieldPosition* ucfpos,
    UFieldCategory category,
    int32_t field,
    UErrorCode* ec);


/**
 * Gets the currently active constraint.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param ec Set if an error occurs.
 * @return The currently active constraint type.
 * @draft ICU 64
 */
U_DRAFT UCFPosConstraintType U_EXPORT2
ucfpos_getConstraintType(
    const UConstrainedFieldPosition* ucfpos,
    UErrorCode* ec);


/**
 * Gets the field category for the current position.
 *
 * The return value is well-defined only after ufmtval_nextPosition returns TRUE.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param ec Set if an error occurs.
 * @return The field category saved in the instance.
 * @draft ICU 64
 */
U_DRAFT UFieldCategory U_EXPORT2
ucfpos_getCategory(
    const UConstrainedFieldPosition* ucfpos,
    UErrorCode* ec);


/**
 * Gets the field for the current position.
 *
 * The return value is well-defined only after ufmtval_nextPosition returns TRUE.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param ec Set if an error occurs.
 * @return The field saved in the instance.
 * @draft ICU 64
 */
U_DRAFT int32_t U_EXPORT2
ucfpos_getField(
    const UConstrainedFieldPosition* ucfpos,
    UErrorCode* ec);


/**
 * Gets the INCLUSIVE start and EXCLUSIVE end index stored for the current position.
 *
 * The output values are well-defined only after ufmtval_nextPosition returns TRUE.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param pStartIndex Set to the start index saved in the instance. Ignored if nullptr.
 * @param pEndIndex Set to the end index saved in the instance. Ignored if nullptr.
 * @param ec Set if an error occurs.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_getIndexes(
    const UConstrainedFieldPosition* ucfpos,
    int32_t* pStart,
    int32_t* pLimit,
    UErrorCode* ec);


/**
 * Gets an int64 that FormattedValue implementations may use for storage.
 *
 * The initial value is zero.
 *
 * Users of FormattedValue should not need to call this method.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param ec Set if an error occurs.
 * @return The current iteration context from ucfpos_setInt64IterationContext.
 * @draft ICU 64
 */
U_DRAFT int64_t U_EXPORT2
ucfpos_getInt64IterationContext(
    const UConstrainedFieldPosition* ucfpos,
    UErrorCode* ec);


/**
 * Sets an int64 that FormattedValue implementations may use for storage.
 *
 * Intended to be used by FormattedValue implementations.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param context The new iteration context.
 * @param ec Set if an error occurs.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_setInt64IterationContext(
    const UConstrainedFieldPosition* ucfpos,
    int64_t context,
    UErrorCode* ec);


/**
 * Sets new values for the primary public getters.
 *
 * Intended to be used by FormattedValue implementations.
 *
 * @param ucfpos The instance of UConstrainedFieldPosition.
 * @param category The new field category.
 * @param field The new field.
 * @param start The new inclusive start index.
 * @param limit The new exclusive end index.
 * @param ec Set if an error occurs.
 * @draft ICU 64
 */
U_DRAFT void U_EXPORT2
ucfpos_setState(
    const UConstrainedFieldPosition* ucfpos,
    UFieldCategory category,
    int32_t field,
    int32_t start,
    int32_t limit,
    UErrorCode* ec);


struct UFormattedValue;
/**
 * An abstract formatted value: a string with associated field attributes.
 * Many formatters format to types compatible with UFormattedValue.
 *
 * @draft ICU 64
 */
typedef struct UFormattedValue UFormattedValue;


/**
 * Returns a pointer to the formatted string. The pointer is owned by the UFormattedValue. The
 * return value is valid only as long as the UFormattedValue is present and unchanged in memory.
 *
 * The return value is NUL-terminated but could contain internal NULs.
 *
 * @param ufmtval
 *         The object containing the formatted string and attributes.
 * @param pLength Output variable for the length of the string. Ignored if NULL.
 * @param ec Set if an error occurs.
 * @return A NUL-terminated char16 string owned by the UFormattedValue.
 * @draft ICU 64
 */
U_DRAFT const UChar* U_EXPORT2
ufmtval_getString(
    const UFormattedValue* ufmtval,
    int32_t* pLength,
    UErrorCode* ec);


/**
 * Iterates over field positions in the UFormattedValue. This lets you determine the position
 * of specific types of substrings, like a month or a decimal separator.
 *
 * To loop over all field positions:
 *
 *     UConstrainedFieldPosition* ucfpos = ucfpos_open(ec);
 *     while (ufmtval_nextPosition(ufmtval, ucfpos, ec)) {
 *         // handle the field position; get information from ucfpos
 *     }
 *     ucfpos_close(ucfpos);
 *
 * @param ufmtval
 *         The object containing the formatted string and attributes.
 * @param ucfpos
 *         The object used for iteration state; can provide constraints to iterate over only
 *         one specific category or field;
 *         see ucfpos_constrainCategory
 *         and ucfpos_constrainField.
 * @param ec Set if an error occurs.
 * @return TRUE if another position was found; FALSE otherwise.
 * @draft ICU 64
 */
U_DRAFT UBool U_EXPORT2
ufmtval_nextPosition(
    const UFormattedValue* ufmtval,
    UConstrainedFieldPosition* ucfpos,
    UErrorCode* ec);


#endif /* #if !UCONFIG_NO_FORMATTING */
#endif // __UFORMATTEDVALUE_H__
