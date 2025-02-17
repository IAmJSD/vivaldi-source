// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DISCOVERY_COMMON_REPORTING_CLIENT_H_
#define DISCOVERY_COMMON_REPORTING_CLIENT_H_

#include "platform/base/error.h"

namespace openscreen::discovery {

// This class is implemented by the embedder who wishes to use discovery. The
// discovery implementation will use this API to report back errors and metrics.
// NOTE: All methods in the reporting client will be called from the task runner
// thread.
// TODO(issuetracker.google.com/281739775): Report state changes back to the
// caller.
class ReportingClient {
 public:
  virtual ~ReportingClient() = default;

  // This method is called when an error is detected by the underlying
  // infrastructure from which recovery cannot be initiated. For example, an
  // error binding a multicast socket.
  virtual void OnFatalError(const Error& error) = 0;

  // This method is called when an error is detected by the underlying
  // infrastructure which does not prevent further functionality of the runtime.
  // For example, a conversion failure between DnsSdInstanceRecord and the
  // externally supplied class.
  virtual void OnRecoverableError(const Error& error) = 0;
};

}  // namespace openscreen::discovery

#endif  // DISCOVERY_COMMON_REPORTING_CLIENT_H_
