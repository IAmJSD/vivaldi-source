// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.download.settings;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.preference.Preference;

import org.chromium.base.supplier.ObservableSupplier;
import org.chromium.base.supplier.ObservableSupplierImpl;
import org.chromium.chrome.browser.download.DownloadDialogBridge;
import org.chromium.chrome.browser.download.DownloadPromptStatus;
import org.chromium.chrome.browser.download.MimeUtils;
import org.chromium.chrome.browser.download.R;
import org.chromium.chrome.browser.pdf.PdfUtils;
import org.chromium.chrome.browser.preferences.Pref;
import org.chromium.chrome.browser.settings.ChromeBaseSettingsFragment;
import org.chromium.chrome.browser.settings.ChromeManagedPreferenceDelegate;
import org.chromium.components.browser_ui.settings.ChromeSwitchPreference;
import org.chromium.components.browser_ui.settings.ManagedPreferenceDelegate;
import org.chromium.components.browser_ui.settings.SettingsUtils;
import org.chromium.components.user_prefs.UserPrefs;

// Vivaldi
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;
import org.chromium.build.BuildConfig;
import org.chromium.ui.base.DeviceFormFactor;
import org.vivaldi.browser.preferences.VivaldiPreferences;

/** Fragment containing Download settings. */
public class DownloadSettings extends ChromeBaseSettingsFragment
        implements Preference.OnPreferenceChangeListener {
    public static final String PREF_LOCATION_CHANGE = "location_change";
    public static final String PREF_LOCATION_PROMPT_ENABLED = "location_prompt_enabled";
    public static final String PREF_AUTO_OPEN_PDF_ENABLED = "auto_open_pdf_enabled";

    private DownloadLocationPreference mLocationChangePref;
    private ChromeSwitchPreference mLocationPromptEnabledPref;
    private ManagedPreferenceDelegate mLocationPromptEnabledPrefDelegate;
    private ChromeSwitchPreference mAutoOpenPdfEnabledPref;
    private final ObservableSupplierImpl<String> mPageTitle = new ObservableSupplierImpl<>();

    //Vivaldi
    private static final String PREF_EXTERNAL_DOWNLOAD_MANAGER = "external_download_manager";
    private Preference mExternalDownloadManagerPref;

    @Override
    public void onCreatePreferences(@Nullable Bundle savedInstanceState, String s) {
        mPageTitle.set(getString(R.string.menu_downloads));
        SettingsUtils.addPreferencesFromResource(this, R.xml.download_preferences);

        mLocationPromptEnabledPref =
                (ChromeSwitchPreference) findPreference(PREF_LOCATION_PROMPT_ENABLED);
        mLocationPromptEnabledPref.setOnPreferenceChangeListener(this);
        mLocationPromptEnabledPrefDelegate =
                new ChromeManagedPreferenceDelegate(getProfile()) {
                    @Override
                    public boolean isPreferenceControlledByPolicy(Preference preference) {
                        return DownloadDialogBridge.isLocationDialogManaged(getProfile());
                    }
                };
        mLocationPromptEnabledPref.setManagedPreferenceDelegate(mLocationPromptEnabledPrefDelegate);
        mLocationChangePref = (DownloadLocationPreference) findPreference(PREF_LOCATION_CHANGE);
        mLocationChangePref.setDownloadLocationHelper(new DownloadLocationHelperImpl(getProfile()));

        mAutoOpenPdfEnabledPref =
                (ChromeSwitchPreference) findPreference(PREF_AUTO_OPEN_PDF_ENABLED);
        if (PdfUtils.shouldOpenPdfInline(getProfile().isOffTheRecord())) {
            mAutoOpenPdfEnabledPref.setVisible(false);
        } else {
            mAutoOpenPdfEnabledPref.setOnPreferenceChangeListener(this);
            String summary =
                    (MimeUtils.getPdfIntentHandlers().size() == 1)
                            ? getActivity()
                                    .getString(
                                            R.string.auto_open_pdf_enabled_with_app_description,
                                            MimeUtils.getDefaultPdfViewerName())
                            : getActivity().getString(R.string.auto_open_pdf_enabled_description);
            mAutoOpenPdfEnabledPref.setSummaryOn(summary);
        }

        // Vivaldi
        mExternalDownloadManagerPref = findPreference(PREF_EXTERNAL_DOWNLOAD_MANAGER);
    }

    @Override
    public ObservableSupplier<String> getPageTitle() {
        return mPageTitle;
    }

    @Override
    public void onDisplayPreferenceDialog(Preference preference) {
        if (preference instanceof DownloadLocationPreference) {
            DownloadLocationPreferenceDialog dialogFragment =
                    DownloadLocationPreferenceDialog.newInstance(
                            (DownloadLocationPreference) preference);
            dialogFragment.setTargetFragment(this, 0);
            dialogFragment.show(getFragmentManager(), DownloadLocationPreferenceDialog.TAG);
        } else {
            super.onDisplayPreferenceDialog(preference);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        updateDownloadSettings();
    }

    private void updateDownloadSettings() {
        mLocationChangePref.updateSummary();

        if (DownloadDialogBridge.isLocationDialogManaged(getProfile())) {
            // Location prompt can be controlled by the enterprise policy.
            mLocationPromptEnabledPref.setChecked(
                    DownloadDialogBridge.getPromptForDownloadPolicy(getProfile()));
        } else {
            // Location prompt is marked enabled if the prompt status is not DONT_SHOW.
            boolean isLocationPromptEnabled =
                    DownloadDialogBridge.getPromptForDownloadAndroid(getProfile())
                            != DownloadPromptStatus.DONT_SHOW;
            mLocationPromptEnabledPref.setChecked(isLocationPromptEnabled);
            mLocationPromptEnabledPref.setEnabled(true);
        }
        if (!PdfUtils.shouldOpenPdfInline(getProfile().isOffTheRecord())) {
            mAutoOpenPdfEnabledPref.setChecked(
                    UserPrefs.get(getProfile()).getBoolean(Pref.AUTO_OPEN_PDF_ENABLED));
            mAutoOpenPdfEnabledPref.setEnabled(true);
        }

        // Vivaldi - update external download Manager summary
        if (mExternalDownloadManagerPref != null) {
            mExternalDownloadManagerPref.setSummary(
                    VivaldiPreferences.getSharedPreferencesManager().readString(
                            VivaldiPreferences.EXTERNAL_DOWNLOAD_MANAGER_APPLICATION, "Off"));
        }
        // End Vivaldi
    }

    // Preference.OnPreferenceChangeListener implementation.
    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (PREF_LOCATION_PROMPT_ENABLED.equals(preference.getKey())) {
            if ((boolean) newValue) {
                // Only update if the download location dialog has been shown before.
                if (DownloadDialogBridge.getPromptForDownloadAndroid(getProfile())
                        != DownloadPromptStatus.SHOW_INITIAL) {
                    DownloadDialogBridge.setPromptForDownloadAndroid(
                            getProfile(), DownloadPromptStatus.SHOW_PREFERENCE);
                }
            } else {
                DownloadDialogBridge.setPromptForDownloadAndroid(
                        getProfile(), DownloadPromptStatus.DONT_SHOW);
            }
        } else if (PREF_AUTO_OPEN_PDF_ENABLED.equals(preference.getKey())) {
            UserPrefs.get(getProfile()).setBoolean(Pref.AUTO_OPEN_PDF_ENABLED, (boolean) newValue);
        }
        return true;
    }

    public ManagedPreferenceDelegate getLocationPromptEnabledPrefDelegateForTesting() {
        return mLocationPromptEnabledPrefDelegate;
    }

    /** Vivaldi **/
    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        if (BuildConfig.IS_VIVALDI) {
            boolean isTablet = DeviceFormFactor
                    .isNonMultiDisplayContextOnTablet(view.getContext());
            if (isTablet) {
                getView().setBackgroundColor(getContext()
                        .getColor(R.color.tablet_panel_bg_color));
                Toolbar toolbar = new Toolbar(getContext());
                if (getView() != null) {
                    ((ViewGroup) getView()).addView(toolbar, 0);
                    toolbar.getLayoutParams().height =
                    Math.round(56 * getResources().getDisplayMetrics().density);
                    toolbar.setBackgroundColor(
                            getContext().getColor(R.color.tablet_panel_bg_color));
                    toolbar.setSubtitle(getResources().getString(R.string.downloads));
                    toolbar.setNavigationIcon(R.drawable.vivaldi_nav_button_back);
                    ((LinearLayout.LayoutParams) toolbar.getLayoutParams()).gravity = Gravity.TOP;
                    toolbar.setNavigationOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            getParentFragmentManager().popBackStack();
                        }
                    });
                }

            }
        }
        super.onViewCreated(view, savedInstanceState);
    }
}
