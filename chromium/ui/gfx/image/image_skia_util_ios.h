// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_IMAGE_IMAGE_SKIA_UTIL_IOS_H_
#define UI_GFX_IMAGE_IMAGE_SKIA_UTIL_IOS_H_

#include "base/component_export.h"

#ifdef __OBJC__
@class UIImage;
#else
class UIImage;
#endif

namespace gfx {
class ImageSkia;
class ImageSkiaRep;

// Converts to ImageSkia from UIImage.
COMPONENT_EXPORT(GFX) gfx::ImageSkia ImageSkiaFromUIImage(UIImage* image);

// Converts to an ImageSkiaRep of |scale_factor| from UIImage.
// |scale| is passed explicitly in order to allow this method to be used
// with a |scale| which is not supported by the platform.
COMPONENT_EXPORT(GFX)
gfx::ImageSkiaRep ImageSkiaRepOfScaleFromUIImage(UIImage* image, float scale);

// Converts to UIImage from ImageSkia. The returned UIImage will be at the scale
// of the ImageSkiaRep in |image_skia| which most closely matches the device's
// scale factor (eg Retina iPad -> 2x).
COMPONENT_EXPORT(GFX)
UIImage* UIImageFromImageSkia(const gfx::ImageSkia& image_skia);

// Converts to UIImage from ImageSkiaRep.
COMPONENT_EXPORT(GFX)
UIImage* UIImageFromImageSkiaRep(const gfx::ImageSkiaRep& image_skia_rep);

}  // namespace gfx

#endif  // UI_GFX_IMAGE_IMAGE_SKIA_UTIL_IOS_H_
