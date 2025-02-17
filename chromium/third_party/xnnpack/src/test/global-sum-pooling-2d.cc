// Copyright 2022 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <gtest/gtest.h>
#include "xnnpack.h"
#include "xnnpack/math.h"
#include "xnnpack/node-type.h"
#include "xnnpack/subgraph.h"
#include "replicable_random_device.h"

template <typename T> class GlobalSumPooling2DTest : public ::testing::Test {
 protected:
  GlobalSumPooling2DTest() {
    shape_dist = std::uniform_int_distribution<size_t>(3, XNN_MAX_TENSOR_DIMS);
    dim_dist = std::uniform_int_distribution<size_t>(1, 9);
    f32dist = std::uniform_real_distribution<float>();

    input_dims = RandomShape();
    output_dims = input_dims;
    output_dims[output_dims.size() - 3] = 1;
    output_dims[output_dims.size() - 2] = 1;
    input =
        std::vector<T>(XNN_EXTRA_BYTES / sizeof(T) + NumElements(input_dims));
    operator_output = std::vector<T>(NumElements(output_dims));
    subgraph_output = std::vector<T>(operator_output.size());
    batch_size = 1;
    for (size_t i = 0; i < input_dims.size() - 3; i++) {
      batch_size *= input_dims[i];
    }
    input_width =
        input_dims[input_dims.size() - 3] * input_dims[input_dims.size() - 2];
    channels = input_dims[input_dims.size() - 1];
  }

  std::vector<size_t> RandomShape() {
    std::vector<size_t> dims(shape_dist(rng));
    std::generate(dims.begin(), dims.end(), [&] { return dim_dist(rng); });
    return dims;
  }

  size_t NumElements(std::vector<size_t>& dims)
  {
    return std::accumulate(
        dims.begin(), dims.end(), size_t(1), std::multiplies<size_t>());
  }

  xnnpack::ReplicableRandomDevice rng;
  std::uniform_int_distribution<size_t> shape_dist;
  std::uniform_int_distribution<size_t> dim_dist;
  std::uniform_real_distribution<float> f32dist;

  float output_min = -std::numeric_limits<float>::infinity();
  float output_max = std::numeric_limits<float>::infinity();
  size_t batch_size;
  size_t input_width;
  size_t channels;

  std::vector<size_t> input_dims;
  std::vector<size_t> output_dims;

  std::vector<T> input;
  std::vector<T> operator_output;
  std::vector<T> subgraph_output;
};

using GlobalSumPooling2DTestF16 = GlobalSumPooling2DTest<xnn_float16>;
using GlobalSumPooling2DTestF32 = GlobalSumPooling2DTest<float>;

TEST_F(GlobalSumPooling2DTestF16, define)
{
  ASSERT_EQ(xnn_status_success, xnn_initialize(/*allocator=*/nullptr));

  xnn_subgraph_t subgraph = nullptr;
  ASSERT_EQ(xnn_status_success, xnn_create_subgraph(2, /*flags=*/0, &subgraph));
  std::unique_ptr<xnn_subgraph, decltype(&xnn_delete_subgraph)> auto_subgraph(subgraph, xnn_delete_subgraph);

  uint32_t input_id = XNN_INVALID_NODE_ID;
  ASSERT_EQ(
    xnn_status_success, xnn_define_tensor_value(
                          subgraph, xnn_datatype_fp16, input_dims.size(), input_dims.data(), nullptr,
                          /*external_id=*/0, /*flags=*/0, &input_id));
  ASSERT_NE(input_id, XNN_INVALID_NODE_ID);

  uint32_t output_id = XNN_INVALID_NODE_ID;
  ASSERT_EQ(
    xnn_status_success, xnn_define_tensor_value(
                          subgraph, xnn_datatype_fp16, output_dims.size(), output_dims.data(), nullptr,
                          /*external_id=*/1, /*flags=*/0, &output_id));
  ASSERT_NE(output_id, XNN_INVALID_NODE_ID);

  ASSERT_EQ(
    xnn_status_success,
    xnn_define_global_sum_pooling_2d(subgraph, output_min, output_max, input_id, output_id, /*flags=*/0));

  ASSERT_EQ(subgraph->num_nodes, 1);
  const struct xnn_node* node = &subgraph->nodes[0];
  ASSERT_EQ(node->type, xnn_node_type_static_sum);
  ASSERT_EQ(node->compute_type, xnn_compute_type_fp16);
  ASSERT_EQ(node->num_inputs, 1);
  ASSERT_EQ(node->inputs[0], input_id);
  ASSERT_EQ(node->num_outputs, 1);
  ASSERT_EQ(node->outputs[0], output_id);
  ASSERT_EQ(node->params.reduce.num_reduction_axes, 2);
  ASSERT_EQ(node->params.reduce.reduction_axes[0], input_dims.size() - 3);
  ASSERT_EQ(node->params.reduce.reduction_axes[1], input_dims.size() - 2);
  ASSERT_EQ(node->flags, 0);
}

TEST_F(GlobalSumPooling2DTestF32, define)
{
  ASSERT_EQ(xnn_status_success, xnn_initialize(/*allocator=*/nullptr));

  xnn_subgraph_t subgraph = nullptr;
  ASSERT_EQ(xnn_status_success, xnn_create_subgraph(2, /*flags=*/0, &subgraph));
  std::unique_ptr<xnn_subgraph, decltype(&xnn_delete_subgraph)> auto_subgraph(subgraph, xnn_delete_subgraph);

  uint32_t input_id = XNN_INVALID_NODE_ID;
  ASSERT_EQ(
    xnn_status_success, xnn_define_tensor_value(
                          subgraph, xnn_datatype_fp32, input_dims.size(), input_dims.data(), nullptr,
                          /*external_id=*/0, /*flags=*/0, &input_id));
  ASSERT_NE(input_id, XNN_INVALID_NODE_ID);

  uint32_t output_id = XNN_INVALID_NODE_ID;
  ASSERT_EQ(
    xnn_status_success, xnn_define_tensor_value(
                          subgraph, xnn_datatype_fp32, output_dims.size(), output_dims.data(), nullptr,
                          /*external_id=*/1, /*flags=*/0, &output_id));
  ASSERT_NE(output_id, XNN_INVALID_NODE_ID);

  ASSERT_EQ(
    xnn_status_success,
    xnn_define_global_sum_pooling_2d(subgraph, output_min, output_max, input_id, output_id, /*flags=*/0));

  ASSERT_EQ(subgraph->num_nodes, 1);
  const struct xnn_node* node = &subgraph->nodes[0];
  ASSERT_EQ(node->type, xnn_node_type_static_sum);
  ASSERT_EQ(node->compute_type, xnn_compute_type_fp32);
  ASSERT_EQ(node->num_inputs, 1);
  ASSERT_EQ(node->inputs[0], input_id);
  ASSERT_EQ(node->num_outputs, 1);
  ASSERT_EQ(node->outputs[0], output_id);
  ASSERT_EQ(node->params.reduce.num_reduction_axes, 2);
  ASSERT_EQ(node->params.reduce.reduction_axes[0], input_dims.size() - 3);
  ASSERT_EQ(node->params.reduce.reduction_axes[1], input_dims.size() - 2);
  ASSERT_EQ(node->flags, 0);
}
