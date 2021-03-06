# ******************************************************************************
# Copyright 2017-2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ******************************************************************************

set(SRC
    cpu_backend.cpp
    cpu_builder.cpp
    cpu_call_frame.cpp
    cpu_emitter.cpp
    cpu_external_function.cpp
    cpu_kernel_emitters.cpp
    cpu_kernel_utils.cpp
    cpu_kernels.cpp
    cpu_layout_descriptor.cpp
    cpu_tensor_view_wrapper.cpp
    cpu_tensor_view.cpp
    cpu_tracing.cpp
    kernel/eigen_thread_pool.cpp
    kernel/pad.cpp
    kernel/reduce_max.cpp
    kernel/reduce_sum.cpp
    kernel/reshape.cpp
    mkldnn_emitter.cpp
    mkldnn_invoke.cpp
    mkldnn_utils.cpp
    op/batch_dot.cpp
    op/batch_norm_relu.cpp
    op/group_conv.cpp
    op/conv_bias.cpp
    op/conv_relu.cpp
    op/convert_layout.cpp
    op/lstm.cpp
    op/matmul_bias.cpp
    op/max_pool_with_indices.cpp
    op/rnn.cpp
    op/sigmoid_mul.cpp
    op/sigmoid.cpp
    pass/cpu_assignment.cpp
    pass/cpu_concat_inputs.cpp
    pass/cpu_fusion.cpp
    pass/cpu_layout.cpp
    pass/cpu_post_layout_optimizations.cpp
    pass/cpu_rnn_fusion.cpp
    pass/cpu_mat_fusion.cpp
    pass/cpu_shuffle_folding.cpp
    pass/cpu_workspace_insertion.cpp
)

if (NGRAPH_TBB_ENABLE)
    include(${TBB_ROOT}/cmake/TBBBuild.cmake)
    tbb_build(TBB_ROOT ${TBB_ROOT} MAKE_ARGS tbb_build_dir=${CMAKE_CURRENT_BINARY_DIR}/tbb_build
                tbb_build_prefix=tbb CONFIG_DIR TBB_DIR)
    find_package(TBB REQUIRED tbb)
    if (NOT TBB_FOUND)
        message(FATAL_ERROR "TBB is needed by the CPU backend and was not found")
    else()
        message(STATUS "Found TBB and imported target ${TBB_IMPORTED_TARGETS}")
    endif()

    set_source_files_properties(cpu_external_function.cpp
        PROPERTIES COMPILE_DEFINITIONS "NGRAPH_TBB_ENABLE")

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tbb_build/tbb_release/
        DESTINATION ${NGRAPH_INSTALL_LIB}
        FILES_MATCHING REGEX "/libtbb${CMAKE_SHARED_LIBRARY_SUFFIX}(\\.[0-9]+)*$"
    )
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tbb_build/tbb_debug/
        DESTINATION ${NGRAPH_INSTALL_LIB}
        FILES_MATCHING REGEX "/libtbb_debug${CMAKE_SHARED_LIBRARY_SUFFIX}(\\.[0-9]+)*$"
    )
    add_library(libtbb INTERFACE)
    target_link_libraries(libtbb INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/tbb_build/tbb_release/libtbb${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
    target_include_directories(libtbb SYSTEM INTERFACE ${TBB_ROOT}/include)
endif()

if (NGRAPH_CPU_ENABLE)
    set(NGRAPH_CPU_DEBUGINFO_ENABLE 0 CACHE STRING "Enable debuginfo in the CPU backend")

    add_library(cpu_backend SHARED ${SRC})
    set_target_properties(cpu_backend PROPERTIES VERSION ${NGRAPH_VERSION} SOVERSION ${NGRAPH_API_VERSION})

    if(NGRAPH_DISTRIBUTED_ENABLE)
        find_package(MPI REQUIRED)
        add_definitions(-DNGRAPH_DISTRIBUTED)
        include_directories(SYSTEM ${MPI_C_INCLUDE_PATH} ${MPI_CXX_INCLUDE_PATH})
        link_directories(${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
        target_link_libraries(cpu_backend PRIVATE ${MPI_CXX_LIBRARIES})
    endif()

    target_link_libraries(cpu_backend PUBLIC ngraph codegen libmkldnn libeigen libjson libtbb)
    target_include_directories(cpu_backend SYSTEM PUBLIC libmkldnn)

    install(TARGETS cpu_backend LIBRARY DESTINATION ${NGRAPH_INSTALL_LIB})
endif()
