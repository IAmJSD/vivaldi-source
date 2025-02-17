/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "third_party/blink/renderer/core/svg/svg_fe_blend_element.h"

#include "third_party/blink/renderer/core/svg/graphics/filters/svg_filter_builder.h"
#include "third_party/blink/renderer/core/svg/svg_animated_string.h"
#include "third_party/blink/renderer/core/svg/svg_enumeration_map.h"
#include "third_party/blink/renderer/core/svg_names.h"
#include "third_party/blink/renderer/platform/graphics/filters/fe_blend.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

namespace blink {

static BlendMode ToBlendMode(SVGFEBlendElement::Mode mode) {
#define MAP_BLEND_MODE(MODENAME)           \
  case SVGFEBlendElement::kMode##MODENAME: \
    return BlendMode::k##MODENAME

  switch (mode) {
    MAP_BLEND_MODE(Normal);
    MAP_BLEND_MODE(Multiply);
    MAP_BLEND_MODE(Screen);
    MAP_BLEND_MODE(Darken);
    MAP_BLEND_MODE(Lighten);
    MAP_BLEND_MODE(Overlay);
    MAP_BLEND_MODE(ColorDodge);
    MAP_BLEND_MODE(ColorBurn);
    MAP_BLEND_MODE(HardLight);
    MAP_BLEND_MODE(SoftLight);
    MAP_BLEND_MODE(Difference);
    MAP_BLEND_MODE(Exclusion);
    MAP_BLEND_MODE(Hue);
    MAP_BLEND_MODE(Saturation);
    MAP_BLEND_MODE(Color);
    MAP_BLEND_MODE(Luminosity);
    default:
      NOTREACHED();
  }
#undef MAP_BLEND_MODE
}

template <>
const SVGEnumerationMap& GetEnumerationMap<SVGFEBlendElement::Mode>() {
  static constexpr auto enum_items = std::to_array<const char* const>({
      "normal",
      "multiply",
      "screen",
      "darken",
      "lighten",
      "overlay",
      "color-dodge",
      "color-burn",
      "hard-light",
      "soft-light",
      "difference",
      "exclusion",
      "hue",
      "saturation",
      "color",
      "luminosity",
  });
  static const SVGEnumerationMap entries(enum_items);
  return entries;
}

SVGFEBlendElement::SVGFEBlendElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(svg_names::kFEBlendTag, document),
      in1_(MakeGarbageCollected<SVGAnimatedString>(this, svg_names::kInAttr)),
      in2_(MakeGarbageCollected<SVGAnimatedString>(this, svg_names::kIn2Attr)),
      mode_(MakeGarbageCollected<SVGAnimatedEnumeration<Mode>>(
          this,
          svg_names::kModeAttr,
          SVGFEBlendElement::kModeNormal)) {}

void SVGFEBlendElement::Trace(Visitor* visitor) const {
  visitor->Trace(in1_);
  visitor->Trace(in2_);
  visitor->Trace(mode_);
  SVGFilterPrimitiveStandardAttributes::Trace(visitor);
}

bool SVGFEBlendElement::SetFilterEffectAttribute(
    FilterEffect* effect,
    const QualifiedName& attr_name) {
  FEBlend* blend = static_cast<FEBlend*>(effect);
  if (attr_name == svg_names::kModeAttr)
    return blend->SetBlendMode(ToBlendMode(mode_->CurrentEnumValue()));

  return SVGFilterPrimitiveStandardAttributes::SetFilterEffectAttribute(
      effect, attr_name);
}

void SVGFEBlendElement::SvgAttributeChanged(
    const SvgAttributeChangedParams& params) {
  const QualifiedName& attr_name = params.name;
  if (attr_name == svg_names::kModeAttr) {
    PrimitiveAttributeChanged(attr_name);
    return;
  }

  if (attr_name == svg_names::kInAttr || attr_name == svg_names::kIn2Attr) {
    Invalidate();
    return;
  }

  SVGFilterPrimitiveStandardAttributes::SvgAttributeChanged(params);
}

FilterEffect* SVGFEBlendElement::Build(SVGFilterBuilder* filter_builder,
                                       Filter* filter) {
  FilterEffect* input1 = filter_builder->GetEffectById(
      AtomicString(in1_->CurrentValue()->Value()));
  FilterEffect* input2 = filter_builder->GetEffectById(
      AtomicString(in2_->CurrentValue()->Value()));
  DCHECK(input1);
  DCHECK(input2);

  auto* effect = MakeGarbageCollected<FEBlend>(
      filter, ToBlendMode(mode_->CurrentEnumValue()));
  FilterEffectVector& input_effects = effect->InputEffects();
  input_effects.reserve(2);
  input_effects.push_back(input1);
  input_effects.push_back(input2);
  return effect;
}

SVGAnimatedPropertyBase* SVGFEBlendElement::PropertyFromAttribute(
    const QualifiedName& attribute_name) const {
  if (attribute_name == svg_names::kInAttr) {
    return in1_.Get();
  } else if (attribute_name == svg_names::kIn2Attr) {
    return in2_.Get();
  } else if (attribute_name == svg_names::kModeAttr) {
    return mode_.Get();
  } else {
    return SVGFilterPrimitiveStandardAttributes::PropertyFromAttribute(
        attribute_name);
  }
}

void SVGFEBlendElement::SynchronizeAllSVGAttributes() const {
  SVGAnimatedPropertyBase* attrs[]{in1_.Get(), in2_.Get(), mode_.Get()};
  SynchronizeListOfSVGAttributes(attrs);
  SVGFilterPrimitiveStandardAttributes::SynchronizeAllSVGAttributes();
}

}  // namespace blink
