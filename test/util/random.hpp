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

#include <functional>
#include <random>

#include "ngraph/types/element_type.hpp"

namespace ngraph
{
    namespace test
    {
        /// @brief A predictable pseudo-random number generator
        /// The seed is initialized so that we get repeatable pseudo-random numbers for tests
        template <typename ET>
        class Uniform
        {
        public:
            using type = typename ET::type;

            Uniform(type min, type max, type seed = 0)
                : m_engine(seed)
                , m_distribution(min, max)
                , m_r(std::bind(m_distribution, m_engine))
            {
            }

            /// @brief Randomly initialize a tensor
            /// @param ptv The tensor to initialize
            const std::shared_ptr<runtime::ParameterizedTensorView<ET>>
                initialize(const std::shared_ptr<runtime::ParameterizedTensorView<ET>>& ptv)
            {
                auto vec = ptv->get_vector();
                for (auto& elt : vec)
                {
                    elt = m_r();
                }
                ptv->write(vec);
                return ptv;
            }

        protected:
            std::default_random_engine m_engine;
            std::uniform_real_distribution<type> m_distribution;
            std::function<type()> m_r;
        };
    }
}