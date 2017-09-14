// ----------------------------------------------------------------------------
// Copyright 2017 Nervana Systems Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// ----------------------------------------------------------------------------

#include "ngraph/descriptor/tensor_view.hpp"

using namespace ngraph;
using namespace descriptor;

const Tensor& PrimaryTensorView::get_tensor() const
{
    return m_tensor;
}

Tensor& PrimaryTensorView::get_tensor()
{
    return m_tensor;
}