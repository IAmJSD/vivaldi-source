// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.findinpage;

import android.content.Context;
import android.content.res.ColorStateList;
import android.util.AttributeSet;
import android.view.View;

import androidx.appcompat.content.res.AppCompatResources;
import androidx.core.widget.ImageViewCompat;

import org.chromium.chrome.R;
import org.chromium.components.browser_ui.styles.ChromeColors;
import org.chromium.components.browser_ui.styles.SemanticColorUtils;

// Vivaldi
import android.view.ViewGroup;

import org.vivaldi.browser.common.VivaldiUtils;
import org.vivaldi.browser.preferences.VivaldiPreferences;

/** A phone specific version of the {@link FindToolbar}. */
public class FindToolbarPhone extends FindToolbar {
    /**
     * Creates an instance of a {@link FindToolbarPhone}.
     * @param context The Context to create the {@link FindToolbarPhone} under.
     * @param attrs The AttributeSet used to create the {@link FindToolbarPhone}.
     */
    public FindToolbarPhone(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void handleActivate() {
        assert isWebContentAvailable();
        setVisibility(View.VISIBLE);

        // Note(david@vivaldi.com): Place the find in page toolbar always on top of the address bar.
        boolean stackVisible = VivaldiUtils.isTabStackVisible();
        int height = (int) getResources().getDimension(R.dimen.tab_strip_height);
        if (!VivaldiPreferences.getSharedPreferencesManager().readBoolean(
                    VivaldiPreferences.SHOW_TAB_STRIP, true))
            height = 0;
        if (stackVisible) height *= 2;
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) getLayoutParams();
        if (params != null) {
            if (VivaldiUtils.isTopToolbarOn())
                params.topMargin = height;
            else {
                if (VivaldiPreferences.getSharedPreferencesManager().readBoolean(
                            VivaldiPreferences.TAB_STACK_TOOLBAR_VISIBLE, false))
                    params.topMargin = 0;
                else
                    params.bottomMargin = height;
            }
            setLayoutParams(params);
        }

        super.handleActivate();
    }

    @Override
    protected void handleDeactivation(boolean clearSelection) {
        setVisibility(View.GONE);
        super.handleDeactivation(clearSelection);
    }

    @Override
    protected void updateVisualsForTabModel(boolean isIncognito) {
        setBackgroundColor(ChromeColors.getDefaultThemeColor(getContext(), isIncognito));
        final ColorStateList color = ChromeColors.getPrimaryIconTint(getContext(), isIncognito);
        ImageViewCompat.setImageTintList(mFindNextButton, color);
        ImageViewCompat.setImageTintList(mFindPrevButton, color);
        ImageViewCompat.setImageTintList(mCloseFindButton, color);

        int queryTextColorId;
        int queryHintTextColorId;
        if (isIncognito) {
            queryTextColorId = R.color.find_in_page_query_white_color;
            queryHintTextColorId = R.color.find_in_page_query_incognito_hint_color;
            mDivider.setBackgroundResource(R.color.white_alpha_12);
        } else {
            queryTextColorId = R.color.default_text_color_list;
            queryHintTextColorId = R.color.find_in_page_query_default_hint_color;
            mDivider.setBackgroundColor(SemanticColorUtils.getDividerLineBgColor(getContext()));
        }
        mFindQuery.setTextColor(
                AppCompatResources.getColorStateList(getContext(), queryTextColorId));
        mFindQuery.setHintTextColor(getContext().getColor(queryHintTextColorId));
    }

    @Override
    protected int getStatusColor(boolean failed, boolean incognito) {
        if (incognito) {
            final int colorResourceId = failed ? R.color.default_red_light : R.color.white_alpha_50;
            return getContext().getColor(colorResourceId);
        }
        return super.getStatusColor(failed, incognito);
    }
}
