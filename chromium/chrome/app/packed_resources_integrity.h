// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Corresponding .cc file is generated by
// //chrome/app:packed_resources_integrity_hash. This header used to be
// generated by the same action, but that unnecessarily serializes the build.
// This checked-in header improves build parallelism.

#include <stdint.h>

#include <array>

#ifndef CHROME_APP_PACKED_RESOURCES_INTEGRITY_H_
#define CHROME_APP_PACKED_RESOURCES_INTEGRITY_H_

// Variable names should be kept in-sync with the files_to_hash list in the GN
// //chrome:packed_resources target.
extern const std::array<uint8_t, 32> kSha256_resources_pak;

extern const std::array<uint8_t, 32> kSha256_vivaldi_100_percent_pak;

extern const std::array<uint8_t, 32> kSha256_vivaldi_200_percent_pak;

#endif  // CHROME_APP_PACKED_RESOURCES_INTEGRITY_H_
