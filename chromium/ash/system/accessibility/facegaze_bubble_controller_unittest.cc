// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/system/accessibility/facegaze_bubble_controller.h"

#include "ash/accessibility/accessibility_controller.h"
#include "ash/shell.h"
#include "ash/system/accessibility/facegaze_bubble_view.h"
#include "ash/test/ash_test_base.h"
#include "base/test/scoped_feature_list.h"
#include "ui/accessibility/accessibility_features.h"
#include "ui/chromeos/styles/cros_tokens_color_mappings.h"
#include "ui/color/color_provider.h"
#include "ui/views/accessibility/view_accessibility.h"

namespace ash {

class FaceGazeBubbleControllerTest : public AshTestBase {
 public:
  FaceGazeBubbleControllerTest() = default;
  ~FaceGazeBubbleControllerTest() override = default;
  FaceGazeBubbleControllerTest(const FaceGazeBubbleControllerTest&) = delete;
  FaceGazeBubbleControllerTest& operator=(const FaceGazeBubbleControllerTest&) =
      delete;

  // AshTestBase:
  void SetUp() override {
    scoped_feature_list_.InitAndEnableFeature(
        ::features::kAccessibilityFaceGaze);
    AshTestBase::SetUp();
    Shell::Get()->accessibility_controller()->face_gaze().SetEnabled(true);
  }

  FaceGazeBubbleController* GetController() {
    return Shell::Get()
        ->accessibility_controller()
        ->GetFaceGazeBubbleControllerForTest();
  }

  void Update(const std::u16string& text, bool is_warning) {
    GetController()->UpdateBubble(text, is_warning);
  }

  FaceGazeBubbleView* GetView() {
    return GetController()->facegaze_bubble_view_;
  }

  const std::u16string& GetBubbleText() {
    return GetView()->GetTextForTesting();
  }

 private:
  base::test::ScopedFeatureList scoped_feature_list_;
};

TEST_F(FaceGazeBubbleControllerTest, LabelText) {
  EXPECT_FALSE(GetView());
  Update(u"Testing", /*is_warning=*/false);
  EXPECT_TRUE(GetView());
  EXPECT_EQ(u"Testing", GetBubbleText());

  Update(u"", /*is_warning=*/false);
  EXPECT_TRUE(GetView());
  EXPECT_EQ(u"", GetBubbleText());
}

TEST_F(FaceGazeBubbleControllerTest, UpdateColor) {
  EXPECT_FALSE(GetView());
  Update(u"Default", /*is_warning=*/false);
  EXPECT_TRUE(GetView());
  ui::ColorProvider* color_provider = GetView()->GetColorProvider();
  EXPECT_EQ(
      color_provider->GetColor(cros_tokens::kCrosSysSystemBaseElevatedOpaque),
      GetView()->color());

  Update(u"Warning", /*is_warning=*/true);
  EXPECT_TRUE(GetView());
  EXPECT_EQ(color_provider->GetColor(cros_tokens::kCrosSysWarningContainer),
            GetView()->color());

  Update(u"Default", /*is_warning=*/false);
  EXPECT_TRUE(GetView());
  EXPECT_EQ(
      color_provider->GetColor(cros_tokens::kCrosSysSystemBaseElevatedOpaque),
      GetView()->color());
}

TEST_F(FaceGazeBubbleControllerTest, AccessibleProperties) {
  Update(u"", /*is_warning=*/false);
  ui::AXNodeData data;
  GetView()->GetViewAccessibility().GetAccessibleNodeData(&data);
  EXPECT_EQ(data.role, ax::mojom::Role::kGenericContainer);
}

}  // namespace ash
