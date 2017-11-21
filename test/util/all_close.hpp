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

#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "ngraph/runtime/parameterized_tensor_view.hpp"
#include "ngraph/types/element_type.hpp"

namespace ngraph
{
    namespace test
    {
        /// @brief Same as numpy.allclose
        /// @param a First tensor to compare
        /// @param b Second tensor to compare
        /// @param rtol Relative tolerance
        /// @param atol Absolute tolerance
        /// @returns true if shapes match and for all elements, |a_i-b_i| <= atol + rtol*|b_i|.
        template <typename T>
        bool all_close(const std::vector<T>& a,
                       const std::vector<T>& b,
                       T rtol = 1e-5f,
                       T atol = 1e-8f)
        {
            assert(a.size() == b.size());
            for (size_t i = 0; i < a.size(); ++i)
            {
                if (std::abs(a[i] - b[i]) > atol + rtol * std::abs(b[i]))
                {
                    return false;
                }
            }
            return true;
        }

        /// @brief Same as numpy.allclose
        /// @param a First tensor to compare
        /// @param b Second tensor to compare
        /// @param rtol Relative tolerance
        /// @param atol Absolute tolerance
        /// Returns true if shapes match and for all elements, |a_i-b_i| <= atol + rtol*|b_i|.
        template <typename ET>
        bool all_close(const std::shared_ptr<ngraph::runtime::ParameterizedTensorView<ET>>& a,
                       const std::shared_ptr<ngraph::runtime::ParameterizedTensorView<ET>>& b,
                       typename ET::type rtol = 1e-5f,
                       typename ET::type atol = 1e-8f)
        {
            // Check that the layouts are compatible
            if (*a->get_tensor_view_layout() != *b->get_tensor_view_layout())
            {
                throw ngraph_error("Cannot compare tensors with different layouts");
            }

            if (a->get_shape() != b->get_shape())
                return false;

            return all_close(a->get_vector(), b->get_vector(), rtol, atol);
        }

        /// @brief Same as numpy.allclose
        /// @param as First tensors to compare
        /// @param bs Second tensors to compare
        /// @param rtol Relative tolerance
        /// @param atol Absolute tolerance
        /// Returns true if shapes match and for all elements, |a_i-b_i| <= atol + rtol*|b_i|.
        template <typename ET>
        bool all_close(
            const std::vector<std::shared_ptr<ngraph::runtime::ParameterizedTensorView<ET>>>& as,
            const std::vector<std::shared_ptr<ngraph::runtime::ParameterizedTensorView<ET>>>& bs,
            typename ET::type rtol,
            typename ET::type atol)
        {
            if (as.size() != bs.size())
            {
                return false;
            }
            for (size_t i = 0; i < as.size(); ++i)
            {
                if (!all_close(as[i], bs[i], rtol, atol))
                {
                    return false;
                }
            }
            return true;
        }
    }
}