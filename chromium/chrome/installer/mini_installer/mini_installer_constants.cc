// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/installer/mini_installer/mini_installer_constants.h"

#include "build/branding_buildflags.h"

namespace mini_installer {

// Various filenames and prefixes.
// The target name of the installer extracted from resources.
const wchar_t kSetupExe[] = L"setup.exe";
// The prefix of the chrome archive resource.
const wchar_t kChromeArchivePrefix[] = L"vivaldi";
// The prefix of the installer resource.
const wchar_t kSetupPrefix[] = L"setup";

// Command line switch names for setup.exe.
const wchar_t kCmdInstallArchive[] = L"install-archive";
const wchar_t kCmdUncompressedArchive[] = L"uncompressed-archive";
const wchar_t kCmdUpdateSetupExe[] = L"update-setup-exe";
const wchar_t kCmdNewSetupExe[] = L"new-setup-exe";
const wchar_t kCmdPreviousVersion[] = L"previous-version";

// Temp directory prefix that this process creates.
const wchar_t kTempPrefix[] = L"CR_";
// ap value suffix to force subsequent updates to use the full rather than
// differential updater.
const wchar_t kFullInstallerSuffix[] = L"-full";

// The resource types that would be unpacked from the mini installer.
// Uncompressed binary.
const wchar_t kBinResourceType[] = L"BN";
#if defined(COMPONENT_BUILD)
// Uncompressed dependency for component builds.
const wchar_t kDepResourceType[] = L"BD";
#endif
// LZ compressed binary.
const wchar_t kLZCResourceType[] = L"BL";
// 7zip archive.
const wchar_t kLZMAResourceType[] = L"B7";

// Registry value names.
// The name of an app's Client State registry value that holds its tag/channel.
const wchar_t kApRegistryValue[] = L"ap";
// The name of the value in kCleanupRegistryKey that tells the installer not to
// delete extracted files.
const wchar_t kCleanupRegistryValue[] = L"ChromeInstallerCleanup";
// These values provide installer result codes to Omaha.
const wchar_t kInstallerErrorRegistryValue[] = L"InstallerError";
const wchar_t kInstallerExtraCode1RegistryValue[] = L"InstallerExtraCode1";
const wchar_t kInstallerResultRegistryValue[] = L"InstallerResult";
const wchar_t kPvRegistryValue[] = L"pv";
const wchar_t kUninstallArgumentsRegistryValue[] = L"UninstallArguments";
// The name of an app's Client State registry value that holds the path to its
// uninstaller.
const wchar_t kUninstallRegistryValue[] = L"UninstallString";

// Registry key paths.
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
// The path to the key containing each app's Clients registry key. The trailing
// slash is required.
const wchar_t kClientsKeyBase[] = L"Software\\Google\\Update\\Clients\\";
// The path to the key containing each app's Client State registry key. The
// trailing slash is required.
const wchar_t kClientStateKeyBase[] =
    L"Software\\Google\\Update\\ClientState\\";
// The path to the key in which kCleanupRegistryValue is found.
const wchar_t kCleanupRegistryKey[] = L"Software\\Google";
#elif BUILDFLAG(GOOGLE_CHROME_FOR_TESTING_BRANDING)
// The path to the key containing each app's Clients registry key.
// No trailing slash on this one because the app's GUID is not appended.
const wchar_t kClientsKeyBase[] = L"Software\\Chrome for Testing";
// The path to the key containing each app's Client State registry key.
// No trailing slash on this one because the app's GUID is not appended.
const wchar_t kClientStateKeyBase[] = L"Software\\Chrome for Testing";
// The path to the key in which kCleanupRegistryValue is found.
const wchar_t kCleanupRegistryKey[] = L"Software\\Chrome for Testing";
#elif defined(VIVALDI_BUILD)
// The path to the key containing each app's Clients registry key.
// No trailing slash on this one because the app's GUID is not appended.
const wchar_t kClientsKeyBase[] = L"Software\\Vivaldi";
// The path to the key containing each app's Client State registry key.
const wchar_t kClientStateKeyBase[] =
    L"Software\\Vivaldi\\Update\\ClientState\\";
// The path to the key in which kCleanupRegistryValue is found.
const wchar_t kCleanupRegistryKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vivaldi";
#else
// The path to the key containing each app's Clients registry key.
// No trailing slash on this one because the app's GUID is not appended.
const wchar_t kClientsKeyBase[] = L"Software\\Chromium";
// The path to the key containing each app's Client State registry key.
// No trailing slash on this one because the app's GUID is not appended.
const wchar_t kClientStateKeyBase[] = L"Software\\Chromium";
// The path to the key in which kCleanupRegistryValue is found.
const wchar_t kCleanupRegistryKey[] = L"Software\\Chromium";
#endif

}  // namespace mini_installer
