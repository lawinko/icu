// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package com.ibm.icu.impl.number.parse;

import com.ibm.icu.impl.StaticUnicodeSets;
import com.ibm.icu.impl.StringSegment;
import com.ibm.icu.text.UnicodeSet;

/**
 * @author sffc
 *
 */
public class IgnorablesMatcher extends SymbolMatcher implements NumberParseMatcher.Flexible {

    private static final IgnorablesMatcher DEFAULT = new IgnorablesMatcher(
            StaticUnicodeSets.get(StaticUnicodeSets.Key.DEFAULT_IGNORABLES));

    private static final IgnorablesMatcher STRICT = new IgnorablesMatcher(
            StaticUnicodeSets.get(StaticUnicodeSets.Key.STRICT_IGNORABLES));

    public static IgnorablesMatcher getInstance(int parseFlags) {
        if (0 != (parseFlags & ParsingUtils.PARSE_FLAG_STRICT_IGNORABLES)) {
            return STRICT;
        } else {
            return DEFAULT;
        }
    }

    private IgnorablesMatcher(UnicodeSet ignorables) {
        super("", ignorables);
    }

    @Override
    protected boolean isDisabled(ParsedNumber result) {
        return false;
    }

    @Override
    protected void accept(StringSegment segment, ParsedNumber result) {
        // No-op
    }

    @Override
    public String toString() {
        return "<IgnorablesMatcher>";
    }
}
