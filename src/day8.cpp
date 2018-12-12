#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>

struct tree_node_t {
    std::vector<tree_node_t> children;
    std::vector<int> metadata;

    void parse(std::ifstream &input_stream) {
        int num_children, num_metadata;
        input_stream >> num_children >> num_metadata;
        for (int i = 0; i < num_children; i++) {
            tree_node_t child;
            child.parse(input_stream);
            children.push_back(child);
        }
        for (int i = 0; i < num_metadata; i++) {
            int metadata_value;
            input_stream >> metadata_value;
            metadata.push_back(metadata_value);
        }
    }

    void visit(std::function<void(const tree_node_t &)> visitor) const {
        visitor(*this);
        for (auto &child : children) {
            child.visit(visitor);
        }
    }

    int compute_value() {
        if (children.empty()) {
            int sum = 0;
            for (auto &mval : metadata) {
                sum += mval;
            }
            return sum;
        } else {
            // Sum of the values of the children specified by the metadata (child indices)
            // If child index is out of bounds, skip
            int sum = 0;
            for (auto &cidx : metadata) {
                if (cidx < 1 || cidx > children.size()) {
                    continue;
                } else {
                    sum += children[cidx - 1].compute_value();
                }
            }
            return sum;
        }
    }
};

void read_day8_data(tree_node_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    outdata.parse(input_stream);
}

int sum_metadata(const tree_node_t& root) {
    int sum = 0;
    root.visit([&sum](const tree_node_t &node) {
        for (auto &metadata : node.metadata) {
            sum += metadata;
        }
    });
    return sum;
}

namespace day8 {

    void problem1() {
        std::cout << "Day 8 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 8
        
        tree_node_t test1;
        read_day8_data(test1, "data/day8/problem1/test1.txt");
        assert(sum_metadata(test1) == 138);

        tree_node_t input;
        read_day8_data(input, "data/day8/problem1/input.txt");
        std::cout << "Result : " << sum_metadata(input) << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 8 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 8

        tree_node_t test1;
        read_day8_data(test1, "data/day8/problem2/test1.txt");
        assert(test1.compute_value() == 66);

        tree_node_t input;
        read_day8_data(input, "data/day8/problem2/input.txt");
        std::cout << "Result : " << input.compute_value() << std::endl;

        #endif
    }

}