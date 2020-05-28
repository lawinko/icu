// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING
#ifndef __GETUNITSDATA_H__
#define __GETUNITSDATA_H__

#include "charstr.h"
#include "cmemory.h"
#include "unicode/measunit.h"
#include "unicode/stringpiece.h"

U_NAMESPACE_BEGIN

/**
 * Looks up the unit category of a base unit identifier.
 *
 * Only supports base units, other units must be resolved to base units before
 * passing to this function.
 *
 * Categories are found in `unitQuantities` in the `units` resource (see
 * `units.txt`).
 *
 * TODO(hugovdm): if we give unitsdata.cpp access to the functionality of
 * `extractCompoundBaseUnit` which is currently in unitconverter.cpp, we could
 * support all units for which there is a category. Does it make sense to move
 * that function to unitsdata.cpp?
 */
CharString U_I18N_API getUnitCategory(const char *baseUnitIdentifier, UErrorCode &status);

/**
 * Encapsulates "convertUnits" information from units resources, specifying how
 * to convert from one unit to another.
 *
 * Information in this class is still in the form of strings: symbolic constants
 * need to be interpreted. Rationale: symbols can cancel out for higher
 * precision conversion - going from feet to inches should cancel out the
 * `ft_to_m` constant.
 */
class U_I18N_API ConversionRateInfo : public UMemory {
  public:
    ConversionRateInfo(){};
    ConversionRateInfo(StringPiece sourceUnit, StringPiece baseUnit, StringPiece factor,
                       StringPiece offset, UErrorCode &status)
        : sourceUnit(), baseUnit(), factor(), offset() {
        this->sourceUnit.append(sourceUnit, status);
        this->baseUnit.append(baseUnit, status);
        this->factor.append(factor, status);
        this->offset.append(offset, status);
    };
    CharString sourceUnit;
    CharString baseUnit;
    CharString factor;
    CharString offset;
};

/**
 * Returns ConversionRateInfo for all supported conversions.
 *
 * @param result Receives the set of conversion rates.
 * @param status Receives status.
 */
void U_I18N_API getAllConversionRates(MaybeStackVector<ConversionRateInfo> &result, UErrorCode &status);

/**
 * Contains all the supported conversion rates.
 */
class U_I18N_API ConversionRates {
  public:
    /**
     * Constructor
     *
     * @param status Receives status.
     */
    ConversionRates(UErrorCode &status) { getAllConversionRates(conversionInfo_, status); }

    /**
     * Returns a pointer to the conversion rate info that match the `source`.
     *
     * @param source Contains the source.
     * @param status Receives status.
     */
    const ConversionRateInfo *extractConversionInfo(StringPiece source, UErrorCode &status) const;

    // TODO(younies): hugovdm added this to resolve "git merge" issues. The API
    // should be improved to make this unnecessary.
    const MaybeStackVector<ConversionRateInfo> *getInternalList() const { return &conversionInfo_; };

  private:
    MaybeStackVector<ConversionRateInfo> conversionInfo_;
};

// Encapsulates unitPreferenceData information from units resources, specifying
// a sequence of output unit preferences.
struct U_I18N_API UnitPreference : public UMemory {
    UnitPreference() : geq(1) {}
    CharString unit;
    double geq;
    CharString skeleton;
};

namespace {

/**
 * Metadata about the preferences in UnitPreferences::unitPrefs_.
 *
 * This class owns all of its data.
 *
 * UnitPreferenceMetadata lives in the anonymous namespace, because it should
 * only be useful to internal code and unit testing code.
 */
class U_I18N_API UnitPreferenceMetadata : public UMemory {
  public:
    UnitPreferenceMetadata(){};
    // Constructor, makes copies of the parameters passed to it.
    UnitPreferenceMetadata(const char *category, const char *usage, const char *region,
                           int32_t prefsOffset, int32_t prefsCount, UErrorCode &status);

    // Unit category (e.g. "length", "mass", "electric-capacitance").
    CharString category;
    // Usage (e.g. "road", "vehicle-fuel", "blood-glucose"). Every category
    // should have an entry for "default" usage. TODO(hugovdm): add a test for
    // this.
    CharString usage;
    // Region code (e.g. "US", "CZ", "001"). Every usage should have an entry
    // for the "001" region ("world"). TODO(hugovdm): add a test for this.
    CharString region;
    // Offset into the UnitPreferences::unitPrefs_ list where the relevant
    // preferences are found.
    int32_t prefsOffset;
    // The number of preferences that form this set.
    int32_t prefsCount;

    int32_t compareTo(const UnitPreferenceMetadata &other) const;
    int32_t compareTo(const UnitPreferenceMetadata &other, bool *foundCategory, bool *foundUsage,
                      bool *foundRegion) const;
};

} // namespace

/**
 * Unit Preferences information for various locales and usages.
 */
class U_I18N_API UnitPreferences {
  public:
    /**
     * Constructor, loads all the preference data.
     *
     * @param status Receives status.
     */
    UnitPreferences(UErrorCode &status);

    /**
     * Returns the set of unit preferences in the particular category that best
     * matches the specified usage and region.
     *
     * If region can't be found, falls back to global (001). If usage can't be
     * found, falls back to "default".
     *
     * @param category The category within which to look up usage and region.
     * (TODO(hugovdm): improve docs on how to find the category, once the lookup
     * function is added.)
     * @param usage The usage parameter. (TODO(hugovdm): improve this
     * documentation. Add reference to some list of usages we support.) If the
     * given usage is not found, the method automatically falls back to
     * "default".
     * @param region The region whose preferences are desired. If there are no
     * specific preferences for the requested region, the method automatically
     * falls back to region "001" ("world").
     * @param outPreferences A pointer into an array of preferences: essentially
     * an array slice in combination with preferenceCount.
     * @param preferenceCount The number of unit preferences that belong to the
     * result set.
     * @param status Receives status.
     *
     * TODO(hugovdm): maybe replace `UnitPreference **&outPrefrences` with a slice class?
     */
    void getPreferencesFor(const char *category, const char *usage, const char *region,
                           const UnitPreference *const *&outPreferences, int32_t &preferenceCount,
                           UErrorCode &status) const;

  protected:
    // Metadata about the sets of preferences, this is the index for looking up
    // preferences in the unitPrefs_ list.
    MaybeStackVector<UnitPreferenceMetadata> metadata_;
    // All the preferences as a flat list: which usage and region preferences
    // are associated with is stored in `metadata_`.
    MaybeStackVector<UnitPreference> unitPrefs_;
};

U_NAMESPACE_END

#endif //__GETUNITSDATA_H__

#endif /* #if !UCONFIG_NO_FORMATTING */