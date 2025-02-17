// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/favicon/model/ios_chrome_large_icon_cache_factory.h"

#include "base/memory/ptr_util.h"
#include "base/no_destructor.h"
#include "components/keyed_service/ios/browser_state_dependency_manager.h"
#include "ios/chrome/browser/favicon/model/large_icon_cache.h"
#include "ios/chrome/browser/shared/model/browser_state/browser_state_otr_helper.h"
#include "ios/chrome/browser/shared/model/profile/profile_ios.h"

// static
LargeIconCache* IOSChromeLargeIconCacheFactory::GetForProfile(
    ProfileIOS* profile) {
  return static_cast<LargeIconCache*>(
      GetInstance()->GetServiceForBrowserState(profile, true));
}

// static
IOSChromeLargeIconCacheFactory* IOSChromeLargeIconCacheFactory::GetInstance() {
  static base::NoDestructor<IOSChromeLargeIconCacheFactory> instance;
  return instance.get();
}

IOSChromeLargeIconCacheFactory::IOSChromeLargeIconCacheFactory()
    : BrowserStateKeyedServiceFactory(
          "LargeIconCache",
          BrowserStateDependencyManager::GetInstance()) {}

IOSChromeLargeIconCacheFactory::~IOSChromeLargeIconCacheFactory() {}

std::unique_ptr<KeyedService>
IOSChromeLargeIconCacheFactory::BuildServiceInstanceFor(
    web::BrowserState* context) const {
  return base::WrapUnique(new LargeIconCache);
}

web::BrowserState* IOSChromeLargeIconCacheFactory::GetBrowserStateToUse(
    web::BrowserState* context) const {
  return GetBrowserStateOwnInstanceInIncognito(context);
}
