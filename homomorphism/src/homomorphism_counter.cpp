#include <homomorphism/calculation_remapper.h>
#include "homomorphism/homomorphism_counter.h"

long HomomorphismCounter::compute() {
    DPState res = computeRec(tdc_->getRoot());

    std::size_t count = 0;

    for (std::size_t c : *res.mappings)
    {
        count += c;
    }

    allocator_->Free(res.mappings, res.bag.size());

    return count;
}

// The state at each step is a pair of vectors
// The first vector contains the ordered bag of vertices for that node
// The second vector contains the count of homomorphisms for all possible mappings of said bag
DPState HomomorphismCounter::computeRec(const std::shared_ptr<NTDNode>& node) {
    switch (node->nodeType)
    {
        case INTRODUCE:
            return computeIntroduceRec(node->left, node->vertex);
        case FORGET:
            return computeForgetRec(node->left, node->vertex);
        case JOIN:
            return computeJoinRec(node->left, node->right);
        default:
            return { std::vector<std::size_t>(0), new std::vector<std::size_t> {1} };
    }
}

DPState HomomorphismCounter::computeIntroduceRec(const std::shared_ptr<NTDNode>& child, std::size_t x) {
    // Currently indices are 1-indexes for the tree decomposition and 0-indexes for the algorithm
    x--;

    DPState c = computeRec(child);

    std::vector<std::size_t> bag = c.bag;

    // TODO: Could be precomputed for cleaner code
    std::size_t pos;
    for(pos = 0; pos < bag.size(); pos++) {
        if(bag[pos] > x) {
            break;
        }
    }

    // Introduce the last vertex
    std::vector<std::size_t>* mapping = allocator_->Allocate(bag.size() + 1);
    introducer_->introduceLast(*c.mappings, *mapping, c.bag, h_, g_, n_, x);
    allocator_->Free(c.mappings, bag.size());

    // Remap vertex to correct position
    std::vector<std::size_t>* result = allocator_->Allocate(bag.size() + 1);
    mapper_->SetSizes(n_, bag.size() + 1);
    mapper_->Insert(*mapping, *result, pos);
    allocator_->Free(mapping, bag.size() + 1);

    bag.insert(bag.begin() + pos, x);

    return { bag, result };
}

// TODO: Handle case of forgetting only vertex
// Should also be handled properly for introduce nodes
DPState HomomorphismCounter::computeForgetRec(const std::shared_ptr<NTDNode>& child, std::size_t x) {
    x--;

    DPState c = computeRec(child);

    std::vector<std::size_t> bag = c.bag;

    // TODO: Could be precomputed for cleaner code
    std::size_t pos;
    for(pos = 0; pos < bag.size(); pos++) {
        if(bag[pos] == x) {
            break;
        }
    }

    // Remap vertex to last position
    std::vector<std::size_t>* mapping = allocator_->Allocate(bag.size());
    mapper_->SetSizes(n_, bag.size());
    mapper_->Extract(*c.mappings, *mapping, pos);
    allocator_->Free(c.mappings, bag.size());

    // Forget the last vertex
    std::vector<std::size_t>* result = allocator_->Allocate(bag.size() - 1);
    forgetter_->forget(*mapping, *result, bag.size(), 0);
    allocator_->Free(mapping, bag.size());

    bag.erase(bag.begin() + pos);

    return { bag, result };
}

DPState HomomorphismCounter::computeJoinRec(const std::shared_ptr<NTDNode>& child1, const std::shared_ptr<NTDNode>& child2) {
    // The bag of the results should be identical
    DPState c1 = computeRec(child1);
    DPState c2 = computeRec(child2);

    joiner_->join(*c1.mappings, *c2.mappings);
    allocator_->Free(c2.mappings, c2.bag.size());

    return { c1.bag, c1.mappings };
}
