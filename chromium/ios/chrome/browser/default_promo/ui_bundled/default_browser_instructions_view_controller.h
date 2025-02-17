// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_DEFAULT_PROMO_UI_BUNDLED_DEFAULT_BROWSER_INSTRUCTIONS_VIEW_CONTROLLER_H_
#define IOS_CHROME_BROWSER_DEFAULT_PROMO_UI_BUNDLED_DEFAULT_BROWSER_INSTRUCTIONS_VIEW_CONTROLLER_H_

#import <UIKit/UIKit.h>

@protocol ConfirmationAlertActionHandler;

// The accessibility identifier of the default browser instructions view
// animation id.
extern NSString* const kDefaultBrowserInstructionsViewAnimationViewId;

// The accessibility identifier of the default browser instructions view dark
// animation id.
extern NSString* const kDefaultBrowserInstructionsViewDarkAnimationViewId;

// View for the displaying default browser instructions.
@interface DefaultBrowserInstructionsViewController : UIViewController

// Creates the view with specified `titleText` based on provided parameters.
// If `titleText` is nil, default title will be used.
- (instancetype)initWithDismissButton:(BOOL)hasDismissButton
                     hasRemindMeLater:(BOOL)hasRemindMeLater
                             hasSteps:(BOOL)hasSteps
                        actionHandler:
                            (id<ConfirmationAlertActionHandler>)actionHandler
                            titleText:(NSString*)titleText;

@end

#endif  // IOS_CHROME_BROWSER_DEFAULT_PROMO_UI_BUNDLED_DEFAULT_BROWSER_INSTRUCTIONS_VIEW_CONTROLLER_H_
