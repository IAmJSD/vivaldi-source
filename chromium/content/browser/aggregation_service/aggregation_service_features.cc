// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/aggregation_service/aggregation_service_features.h"

namespace content {

BASE_FEATURE(kPrivacySandboxAggregationService,
             "PrivacySandboxAggregationService",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kPrivacySandboxAggregationServiceFilteringIds,
             "PrivacySandboxAggregationServiceFilteringIds",
             base::FEATURE_ENABLED_BY_DEFAULT);

}  // namespace content
