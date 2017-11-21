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

#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#include "memory_visualize.hpp"
#include "ngraph/descriptor/tensor.hpp"
#include "ngraph/function.hpp"
#include "ngraph/node.hpp"
#include "ngraph/util.hpp"

using namespace std;
using namespace ngraph;
using namespace ngraph::descriptor;

pass::MemoryVisualize::MemoryVisualize(const string& filename)
    : m_filename{filename}
{
}

bool pass::MemoryVisualize::run_on_module(vector<shared_ptr<ngraph::Function>>& functions)
{
    ofstream file(m_filename);
    {
        for (shared_ptr<Function> f : functions)
        {
            list<shared_ptr<Node>> nodes = f->get_ordered_ops();
            file << "<!DOCTYPE html>\n<html>\n";
            file << "<head>\n";
            file << "    <style>\n";
            file << "        th, td {\n";
            file << "            border-bottom: 1px solid #ddd;\n";
            file << "            width: 200px;\n";
            file << "        }\n";
            file << "        table, td, th {\n";
            // file << "            border: 1px solid #ddd;\n";
            // file << "            text-align: left;\n";
            file << "        }\n";
            file << "        table {\n";
            file << "            border-collapse: collapse;\n";
            // file << "            width: 100%;\n";
            file << "        }\n";
            // file << "        tr:hover {background-color: #f5f5f5}\n";
            file << "        tr:nth-child(even) {background-color: #f2f2f2}\n";
            file << "    </style>\n";
            file << "</head>\n";

            file << "<body>\n";
            unordered_set<descriptor::Tensor*> tensors;
            size_t temp_max_size = 0;
            for (shared_ptr<Node> node : nodes)
            {
                tensors.insert(node->liveness_live_list.begin(), node->liveness_live_list.end());
            }
            for (descriptor::Tensor* tensor : tensors)
            {
                if (tensor->is_persistent() == false)
                {
                    temp_max_size += tensor->size();
                }
            }

            // file << "<table>\n";
            // file << "<tr><td>Persistent Memory Footprint</td><td align=\"right\">";
            // file << computation_decl.exop_block.persistent_size() << "</td></tr>\n";
            // file << "<tr><td>Temporary Memory Footprint</td><td align=\"right\">";
            // file << computation_decl.exop_block.memory_footprint() << "</td></tr>\n";
            // file << "<tr><td>Max temporary Memory Footprint</td><td align=\"right\">";
            // file << temp_max_size << "</td></tr>\n";
            // file << "</table>\n";

            file << "<hr>\n";
            draw_tensor_weight(file, nodes);
            // file << "<hr>\n";
            // draw_op_influence(file);
            file << "<hr>\n";
            draw_histogram(file, nodes);
            // file << "<hr>\n";
            file << "</body>\n</html>\n";
        }
    }
    return false;
}

shared_ptr<Node> pass::MemoryVisualize::find_largest_op(const list<shared_ptr<Node>>& nodes)
{
    shared_ptr<Node> largest_op = nullptr;
    size_t largest_size = 0;
    for (shared_ptr<Node> exop : nodes)
    {
        size_t size = 0;
        for (const Tensor* tensor : exop->liveness_live_list)
        {
            size += tensor->size();
        }
        if (size > largest_size)
        {
            largest_size = size;
            largest_op = exop;
        }
    }
    return largest_op;
}

void pass::MemoryVisualize::draw_tensor_weight(ostream& file, const list<shared_ptr<Node>>& nodes)
{
    shared_ptr<Node> largest_op = find_largest_op(nodes);

    if (largest_op)
    {
        unordered_set<Tensor*> largest_live;
        for (Tensor* tensor : largest_op->liveness_live_list)
        {
            largest_live.insert(tensor);
        }

        unordered_map<const Tensor*, size_t> age_list;
        vector<const Tensor*> tensor_set;
        unordered_map<const Tensor*, shared_ptr<Node>> generator_op;
        file << "<table>\n";
        file << "    <tr>";
        file << "<th align=\"left\">tensor</th>";
        file << "<th align=\"right\">size</th>";
        file << "<th align=\"right\">age</th>";
        file << "<th align=\"right\">generator weight</th>";
        file << "</tr>\n";
        size_t i = 0;
        for (shared_ptr<Node> exop : nodes)
        {
            for (const Tensor* tensor : exop->liveness_new_list)
            {
                age_list[tensor] = i;
                generator_op[tensor] = exop;
            }
            for (const Tensor* tensor : exop->liveness_free_list)
            {
                size_t start = age_list[tensor];
                age_list[tensor] = (i - start);
                tensor_set.push_back(tensor);
            }
            i++;
        }
        sort(tensor_set.begin(), tensor_set.end(), [](const Tensor* t1, const Tensor* t2) {
            return t1->size() < t2->size();
        });
        for (const Tensor* tensor : tensor_set)
        {
            int generator_weight = compute_op_weight(generator_op[tensor]);
            if (contains(largest_live, tensor))
            {
                file << "    <tr style=\"background-color: #f0c0f0\">";
            }
            else
            {
                file << "    <tr>";
            }
            file << "<td>" << tensor->get_name() << "</td>";
            file << "<td align=\"right\">" << tensor->size() << "</td>";
            file << "<td align=\"right\">" << age_list[tensor] << "</td>";
            file << "<td align=\"right\">" << generator_weight << "/td>";
            file << "</tr>\n";
        }

        file << "</table>\n";
    }
}

void pass::MemoryVisualize::draw_histogram(ostream& file, const list<shared_ptr<Node>>& nodes)
{
    size_t stroke_width = 14;
    size_t text_offset = 4;
    size_t offset = 200;
    size_t width = 1000;
    size_t scale = width - offset;
    size_t line_spacing = stroke_width * 1.5;
    size_t line_count = 0;
    for (shared_ptr<Node> node : nodes)
    {
        (void)node;
        line_count += 1;
    }
    size_t height = line_count * line_spacing + stroke_width;
    size_t memory_footprint = max<size_t>(1, MemoryVisualize::memory_footprint(nodes));

    file << "<svg viewBox=\"0 0 " << width << " " << height << "\">\n";
    size_t y = 0;
    for (shared_ptr<Node> node : nodes)
    {
        float usage = float(MemoryVisualize::memory_usage(node));
        float footprint = float(MemoryVisualize::memory_footprint(node));
        y += line_spacing;
        size_t x1 = offset;
        size_t x2 = ((usage / memory_footprint) * scale) + offset;
        file << "<text x=\"" << 0 << "\" y=\"" << y + text_offset << "\" fill=\""
             << "black"
             << "\">" << node->get_name() << "</text>\n";
        file << "<line x1=\"" << x1 << "\" y1=\"" << y << "\" x2=\"" << x2 << "\" y2=\"" << y
             << "\"";
        file << " style=\"stroke:forestgreen;stroke-width:" << stroke_width << "\" />\n";
        x1 = x2;
        x2 = ((footprint / memory_footprint) * scale) + offset;
        file << "<line x1=\"" << x1 << "\" y1=\"" << y << "\" x2=\"" << x2 << "\" y2=\"" << y
             << "\"";
        file << " style=\"stroke:firebrick;stroke-width:" << stroke_width << "\" />\n";
    }
    file << "</svg>\n";
}

void pass::MemoryVisualize::draw_op_influence(ostream& file, const list<shared_ptr<Node>>& nodes)
{
    file << "<table>\n";
    file << "    <tr>";
    file << "<th align=\"left\">op</th>";
    file << "<th align=\"right\">influence</th>";
    file << "</tr>\n";
    for (shared_ptr<Node> exop : nodes)
    {
        int weight = compute_op_weight(exop);
        file << "    <tr>";
        file << "<td>" << exop->get_name() << "</td>";
        file << "<td align=\"right\">" << weight << "</td>";
        file << "</tr>\n";
    }
}

int pass::MemoryVisualize::compute_op_weight(const shared_ptr<Node> exop)
{
    int mass = 0;
    // for input_decl in exop.input_decls:
    //     tensor = input_decl.source_output_decl.tensor
    //     if tensor.is_persistent is False:
    //         mass += tensor->size()
    // for output_decl in exop.output_decls:
    //     tensor = output_decl.tensor
    //     if tensor.is_persistent is False:
    //         mass -= tensor->size()
    for (const Tensor* tensor : exop->liveness_new_list)
    {
        if (tensor->is_persistent() == false)
        {
            mass += tensor->size();
        }
    }
    for (const Tensor* tensor : exop->liveness_free_list)
    {
        if (tensor->is_persistent() == false)
        {
            mass -= tensor->size();
        }
    }
    return mass;
}

size_t pass::MemoryVisualize::memory_usage(shared_ptr<Node> node)
{
    return 0;
}

size_t pass::MemoryVisualize::memory_footprint(shared_ptr<Node> node)
{
    return 0;
}

size_t pass::MemoryVisualize::memory_footprint(const std::list<shared_ptr<Node>>& nodes)
{
    return 0;
}