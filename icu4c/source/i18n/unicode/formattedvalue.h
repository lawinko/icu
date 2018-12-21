// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __FORMATTEDVALUE_H__
#define __FORMATTEDVALUE_H__

#include "unicode/utypes.h"
#if !UCONFIG_NO_FORMATTING

#include "unicode/appendable.h"
#include "unicode/fpositer.h"
#include "unicode/unistr.h"
#include "unicode/uformattedvalue.h"

U_NAMESPACE_BEGIN


/**
 * Represents a span of a string containing a given field.
 *
 * This class differs from FieldPosition in the following ways:
 *
 *   1. It has information on the field category.
 *   2. It allows you to set constraints to use when iterating over field positions.
 *   3. It is used for the newer FormattedValue APIs.
 *
 * @draft ICU 64
 */
class U_I18N_API ConstrainedFieldPosition : public UMemory {
  public:

    /**
     * Initializes a ConstrainedFieldPosition.
     *
     * By default, the ConstrainedFieldPosition has no iteration constraints.
     *
     * @draft ICU 64
     */
    ConstrainedFieldPosition();

    /** @draft ICU 64 */
    virtual ~ConstrainedFieldPosition();

    /**
     * Resets this ConstrainedFieldPosition to its initial state, as if it were newly created.
     *
     * Removes any constraints that may have been set on the instance.
     *
     * @draft ICU 64
     */
    void reset();

    /**
     * Sets a constraint on the field category.
     * 
     * When this instance of ConstrainedFieldPosition is passed to FormattedValue#nextPosition,
     * positions are skipped unless they have the given category.
     *
     * Any previously set constraints are cleared.
     *
     * For example, to loop over only the number-related fields:
     *
     *     ConstrainedFieldPosition cfpos;
     *     cfpos.constrainCategory(UFIELDCATEGORY_NUMBER_FORMAT);
     *     while (fmtval.nextPosition(cfpos, status)) {
     *         // handle the number-related field position
     *     }
     *
     * @param category The field category to fix when iterating.
     * @draft ICU 64
     */
    void constrainCategory(UFieldCategory category);

    /**
     * Sets a constraint on the category and field.
     * 
     * When this instance of ConstrainedFieldPosition is passed to FormattedValue#nextPosition,
     * positions are skipped unless they have the given category and field.
     *
     * Any previously set constraints are cleared.
     *
     * For example, to loop over all grouping separators:
     *
     *     ConstrainedFieldPosition cfpos;
     *     cfpos.constrainField(UFIELDCATEGORY_NUMBER_FORMAT, UNUM_GROUPING_SEPARATOR_FIELD);
     *     while (fmtval.nextPosition(cfpos, status)) {
     *         // handle the grouping separator position
     *     }
     *
     * @param category The field category to fix when iterating.
     * @param field The field to fix when iterating.
     * @draft ICU 64
     */
    void constrainField(UFieldCategory category, int32_t field);

    /**
     * Gets the currently active constraint.
     *
     * @return The currently active constraint type.
     * @draft ICU 64
     */
    inline UCFPosConstraintType getConstraintType() const {
        return fConstraint;
    }

    /**
     * Gets the field category for the current position.
     *
     * The return value is well-defined only after FormattedValue#nextPosition returns TRUE.
     *
     * @return The field category saved in the instance.
     * @draft ICU 64
     */
    inline UFieldCategory getCategory() const {
        return fCategory;
    };

    /**
     * Gets the field for the current position.
     *
     * The return value is well-defined only after FormattedValue#nextPosition returns TRUE.
     *
     * @return The field saved in the instance.
     * @draft ICU 64
     */
    inline int32_t getField() const {
        return fField;
    };

    /**
     * Gets the INCLUSIVE start index for the current position.
     *
     * The return value is well-defined only after FormattedValue#nextPosition returns TRUE.
     *
     * @return The start index saved in the instance.
     * @draft ICU 64
     */
    inline int32_t getStart() const {
        return fStart;
    };

    /**
     * Gets the EXCLUSIVE end index stored for the current position.
     *
     * The return value is well-defined only after FormattedValue#nextPosition returns TRUE.
     *
     * @return The end index saved in the instance.
     * @draft ICU 64
     */
    inline int32_t getLimit() const {
        return fLimit;
    };

    ////////////////////////////////////////////////////////////////////
    //// The following methods are for FormattedValue implementers; ////
    //// most users can ignore them.                                ////
    ////////////////////////////////////////////////////////////////////

    /**
     * Gets an int64 that FormattedValue implementations may use for storage.
     *
     * The initial value is zero.
     *
     * Users of FormattedValue should not need to call this method.
     *
     * @return The current iteration context from {@link #setInt64IterationContext}.
     * @draft ICU 64
     */
    inline int64_t getInt64IterationContext() const {
        return fContext;
    }

    /**
     * Sets an int64 that FormattedValue implementations may use for storage.
     *
     * Intended to be used by FormattedValue implementations.
     *
     * @param context The new iteration context.
     * @draft ICU 64
     */
    void setInt64IterationContext(int64_t context);

    /**
     * Sets new values for the primary public getters.
     *
     * Intended to be used by FormattedValue implementations.
     *
     * @param category The new field category.
     * @param field The new field.
     * @param start The new inclusive start index.
     * @param limit The new exclusive end index.
     * @draft ICU 64
     */
    void setState(
        UFieldCategory category,
        int32_t field,
        int32_t start,
        int32_t limit);

  private:
    int64_t fContext = 0LL;
    int32_t fField = 0;
    int32_t fStart = 0;
    int32_t fLimit = 0;
    UCFPosConstraintType fConstraint = UCFPOS_CONSTRAINT_NONE;
    UFieldCategory fCategory = UFIELD_CATEGORY_UNDEFINED;
};


/**
 * An abstract formatted value: a string with associated field attributes.
 * Many formatters format to classes implementing FormattedValue.
 *
 * @draft ICU 64
 */
class U_I18N_API FormattedValue {
  public:
    virtual ~FormattedValue();

    /**
     * Returns the formatted string as a self-contained UnicodeString.
     *
     * If you need the string within the current scope only, consider #toTempString.
     *
     * @param status Set if an error occurs.
     * @return a UnicodeString containing the formatted string.
     *
     * @draft ICU 64
     */
    virtual UnicodeString toString(UErrorCode& status) const = 0;

    /**
     * Returns the formatted string as a read-only alias to memory owned by the FormattedValue.
     *
     * The return value is valid only as long as this FormattedValue is present and unchanged in
     * memory. If you need the string outside the current scope, consider #toString.
     *
     * @param status Set if an error occurs.
     * @return a temporary UnicodeString containing the formatted string.
     *
     * @draft ICU 64
     */
    virtual UnicodeString toTempString(UErrorCode& status) const = 0;

    /**
     * Appends the formatted string to an Appendable.
     *
     * @param appendable
     *         The Appendable to which to append the string output.
     * @param status Set if an error occurs.
     * @return The same Appendable, for chaining.
     *
     * @draft ICU 64
     * @see Appendable
     */
    virtual Appendable& appendTo(Appendable& appendable, UErrorCode& status) const = 0;

    /**
     * Iterates over field positions in the FormattedValue. This lets you determine the position
     * of specific types of substrings, like a month or a decimal separator.
     *
     * To loop over all field positions:
     *
     *     ConstrainedFieldPosition cfpos;
     *     while (fmtval.nextPosition(cfpos, status)) {
     *         // handle the field position; get information from cfpos
     *     }
     *
     * @param cfpos
     *         The object used for iteration state. This can provide constraints to iterate over
     *         only one specific category or field;
     *         see ConstrainedFieldPosition#constrainCategory
     *         and ConstrainedFieldPosition#constrainField.
     * @param status Set if an error occurs.
     * @return TRUE if a new occurrence of the field was found;
     *         FALSE otherwise or if an error was set.
     *
     * @draft ICU 64
     */
    virtual UBool nextPosition(ConstrainedFieldPosition& cfpos, UErrorCode& status) const = 0;
};


U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */
#endif // __FORMATTEDVALUE_H__
