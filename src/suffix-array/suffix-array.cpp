#include <cassert>
#include <cstring>

#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

#include <hacks/suffix-tree.hpp>
#include <hacks/suffix-array.hpp>

static const size_t npos = size_t (-1);

suffix_array_t
make_suffix_array (const suffix_tree_t& t) {
    suffix_array_t suffix_array { t.text };

    //
    // Stack of path length from root:
    //
    stack< size_t > overlap_stack;
    overlap_stack.emplace (0);

    //
    // State of node and edge traversal:
    //
    stack< pair< size_t, size_t > > state;

    size_t i = 0, j = 0, overlap = 0;

    while (1) {
        while (j < t.nodes [i].edges.size ()) {

            const auto& node = t.nodes [i];
            const auto& edge = t.edges [node.edges [j].second];

            assert (t.text.size () > edge.pos);

            if (j) {
                overlap = overlap_stack.top ();
                assert (edge.pos >= overlap);
            }

            //
            // Actual starting position of the substring that contains this
            // edge is overlap characters behind:
            //
            const auto pos = edge.pos - overlap_stack.top ();

            if (edge.end) {
                //
                // Visit children of intermediate edges:
                //
                state.emplace (i, j);

                i = edge.end;
                j = 0;

                overlap_stack.emplace (overlap_stack.top () + edge.len);

                continue;
            }
            else {
                auto first = t.text.begin (), last = first;
                advance (first, pos);

                auto len = overlap_stack.top ();

                if (edge.end)
                    len += edge.len;
                else
                    len += t.text.size () - edge.pos;

                advance (last, pos + len);

                suffix_array.array.emplace_back (string_view_t { first, last });
                suffix_array.lcp.emplace_back (overlap);
            }

            ++j;
        }

        if (state.empty ())
            break;

        tie (i, j) = state.top ();
        state.pop ();

        ++j;

        overlap_stack.pop ();
    }

    suffix_array.lcp [0] = npos;

    return suffix_array;
}