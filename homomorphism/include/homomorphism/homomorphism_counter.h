#ifndef HOMOMORPHISM_HOMOMORPHISM_COUNTER_H
#define HOMOMORPHISM_HOMOMORPHISM_COUNTER_H

#include <memory>

#include "homomorphism/dpstate.h"
#include "homomorphism/nice_tree_decomposition.h"
#include "homomorphism/vector_allocator.h"
#include "homomorphism/join_handler.h"
#include "homomorphism/forget_handler.h"
#include "homomorphism/introduce_handler_least.h"
#include "homomorphism/remapper.h"
#include "homomorphism/homomorphism_counter_interface.h"

struct HomomorphismSettings {
    std::shared_ptr<Remapper> mapper;
    std::shared_ptr<ForgetHandler> forget;
    std::shared_ptr<IntroduceHandlerLeast> introduce;
    std::shared_ptr<JoinHandler> join;
    std::shared_ptr<VectorAllocator> alloc;
};

// TODO: Extended tree decomposition could precompute some values. Such as index for forget and introduce nodes
class HomomorphismCounter : public HomomorphismCounterInterface
{
public:
    HomomorphismCounter(std::shared_ptr<Graph>& h, std::shared_ptr<Graph>& g,
                          std::shared_ptr<NiceTreeDecomposition>& tree, HomomorphismSettings& settings) :
                          h_(h), g_(g), tdc_(tree), n_(g->vertCount()),
                          mapper_(settings.mapper), forgetter_(settings.forget), introducer_(settings.introduce),
                          joiner_(settings.join), allocator_(settings.alloc) {};

    long compute() override;
private:
    std::shared_ptr<Graph> h_, g_;
    std::shared_ptr<NiceTreeDecomposition> tdc_;
    std::size_t n_;
    std::shared_ptr<Remapper> mapper_;
    std::shared_ptr<ForgetHandler> forgetter_;
    std::shared_ptr<IntroduceHandlerLeast> introducer_;
    std::shared_ptr<JoinHandler> joiner_;
    std::shared_ptr<VectorAllocator> allocator_;

    DPState computeRec(const std::shared_ptr<NTDNode>& node);
    DPState computeIntroduceRec(const std::shared_ptr<NTDNode>& child, std::size_t x);
    DPState computeForgetRec(const std::shared_ptr<NTDNode>& child, std::size_t x);
    DPState computeJoinRec(const std::shared_ptr<NTDNode>& child1, const std::shared_ptr<NTDNode>& child2);
};

#endif
