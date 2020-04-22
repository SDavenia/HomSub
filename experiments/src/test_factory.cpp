//
//  test_factory.cpp
//  test
//
//  Created by Jonas Mortensen on 26/02/2020.
//

#include <stdio.h>
#include "experiments/test_factory.h"
#include "homomorphism/tamaki-2017.h"
#include "homomorphism/tree_decomposition.h"
#include "homomorphism/nice_tree_decomposition.h"
#include "homomorphism/adjacency_matrix_graph.h"
#include "homomorphism/main.h"
#include "homomorphism/forget_handler_combined.h"
#include "experiments/graph_generator.h"
#include "experiments/test_settings.h"
#include <memory>
#include "homomorphism/configuration_factory.h"
#include <future>
#include "homomorphism/calculation_remapper_extract_variants.h"
#include "homomorphism/calculation_remapper_insert_variants.h"
#include "homomorphism/calculation_remapper.h"
#include "homomorphism/iterator_remapper.h"
#include "homomorphism/traversal_homomorphism_counter.h"
#include "homomorphism/iterator_introduce_handler.h"
#include "homomorphism/introduce_handler_compute.h"
#include "homomorphism/introduce_handler_precomputed.h"
#include "homomorphism/tamaki_runner.h"

#define BEGIN_TEST(name) logger.NotifyTestStart(name);std::vector<int> durations(settings.GetRepetitions(),0);int duration = 0;auto start = std::chrono::steady_clock::now();auto stop = start;SubStep step;long exp = 0;int n = 1;

#define END_TEST logger.NotifyTestEnd(duration);

#define START_CLOCK start = std::chrono::steady_clock::now();

#define STOP_CLOCK stop = std::chrono::steady_clock::now();duration = milliSecondDifferene(start, stop);

#define ITERATIVE_START(init) n = init; while( ((float)duration/1000.0) < settings.GetPrTestTime()) {

#define ITERATIVE_END(incr) logger.NotifyTestIterative(n, "", duration);incr;}

#define REPEATED_CLOCK_START \
    for(int r = 0; r < settings.GetRepetitions(); r++) { \
        START_CLOCK;

#define REPEATED_CLOCK_END \
        STOP_CLOCK; \
        durations[r] = duration; \
    }

#define LOG_DURATIONS(args) \
for(int q = 0; q < durations.size(); q++) { \
    logger.log(args, durations[q]) \
}
    

#define STEPLOOP_START \
    for(int k = 2; k <= 5; k++) { \
        for(int logn = 3; logn <= 10; logn++) { \
            int n = 1 << logn; \
            int c = k * logn; \
            /* Skip too small or too large cases */ \
            if(15 > c || c > 28) { continue; } \
            size_t size = 1 << c;

#define STEPLOOP_END } }

std::function<void(TestSettings&, TestLogger&)> TestFactory::GetTest(int i) {
    auto tests = GetAllTests();
    if(i < tests.size()) {
        return tests[i];
    } else {
        return nullptr;
    }
}

std::vector<std::function<void(TestSettings&, TestLogger&)>> TestFactory::GetAllTests() {
    std::vector<std::function<void(TestSettings&, TestLogger&)>> tests
        {
            SquaresInGrid,
            BinaryTreeInBinaryTree,
            CliquesInClique,
            EdgesInPath,
            PathInRandomGraph,
            RandomPatternsInRandomGraph,
            ForgetLeastSignificant,
            ForgetMostSignificant,
            ForgetAny,
            ForgetCombined,
            IntroduceIterator,
            IntroduceCompute,
            IntroducePrecomputed,
            joinHandler,
            InsertClosedForm,
            ExtractClosedForm,
            InsertIterator,
            ExtractIterator,
            InsertClosedVariants,
            ExtractClosedVariants,
            MaxDegreeHomomorphismCount,
            CyclesInMaxDegreeRandom,
            StarsIsMaxDegreeKRandom,
            MemoryTest1
        };
    return tests;
}

int TestFactory::TestCount() {
    return 0;
}

void TestFactory::SquaresInGrid(TestSettings& settings, TestLogger& logger)
{
    BEGIN_TEST("SquareInNGrid");
    
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    GraphGenerator::CompleteGrid(h, 2, 2);
    
    std::shared_ptr<AdjacencyMatrixGraph> g = AdjacencyMatrixGraph::testGraph();
    
    for(int n = 1; n < 33; n = n * 2) {
        GraphGenerator::CompleteGrid(g, n, n);
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", n, 4, d);
        }
    }

    END_TEST;
}

void TestFactory::BinaryTreeInBinaryTree(TestSettings& settings, TestLogger& logger)
{
    BEGIN_TEST("BinaryTreeInBinaryTree");
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g  = AdjacencyMatrixGraph::testGraph();
    

    for(int k = 1; k < 4; k++) {
        GraphGenerator::CompleteBinaryTree(h, k);
        GraphGenerator::CompleteBinaryTree(g, k+3);
        
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", k+3, k, d);
        }
    }
    
    
    END_TEST;
}

void TestFactory::CliquesInClique(TestSettings& settings, TestLogger& logger) {
    BEGIN_TEST("CliquesInLargerCliques");
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g = AdjacencyMatrixGraph::testGraph();
    
    for(int n = 1; n < 6; n++) {
        GraphGenerator::Clique(h, n);
        GraphGenerator::Clique(g, n*n);
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", n*n, n, d);
        }
    }

    END_TEST;
}

void TestFactory::EdgesInPath(TestSettings& settings, TestLogger& logger)
{
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g = AdjacencyMatrixGraph::testGraph();
    
    GraphGenerator::Path(h, 1);
    
    BEGIN_TEST("EdgesInPath");
    
    for(int n = 1; n < 4097; n = n*2) {
        GraphGenerator::Path(g, n);
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", n, 2, d);
        }
    }
    
    END_TEST;
}

void TestFactory::PathInRandomGraph(TestSettings& settings, TestLogger& logger)
{
    
    BEGIN_TEST("PathInRandomGraph");
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g = AdjacencyMatrixGraph::testGraph();
    
    for(int k = 1; k < 8; k++) {
        GraphGenerator::Path(h, k);
        GraphGenerator::EdgeProbabilityGraph(g, 1 << k, 0.5f);
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", 1 << k, k, d);
        }
    }
    
    END_TEST;
}

void TestFactory::RandomPatternsInRandomGraph(TestSettings &settings, TestLogger &logger)
{
    BEGIN_TEST("RandomPatternInRandomGraph");
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g = AdjacencyMatrixGraph::testGraph();
    
    for(int k = 1; k < 7; k++) {
        GraphGenerator::EdgeProbabilityGraph(h, k, 0.5f);
        GraphGenerator::EdgeProbabilityGraph(g, 1 << k, 0.5f);
        REPEATED_CLOCK_START;
        Main::subgraphsGraph(h, g);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", 1 << k, k, d);
        }
    }
    
    END_TEST;
}

void TestFactory::ForgetLeastSignificant(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ForgetLeastSignificant")

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

        ForgetHandlerLast handler(n, k);

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size / n);
        fillVector(vec2);
        REPEATED_CLOCK_START;
        handler.forget(vec1, vec2, k, k - 1);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("",n, k, d);
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ForgetMostSignificant(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ForgetMostSignificant")

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

            ForgetHandlerFirst handler(n, k);

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size / n);
            fillVector(vec2);
            REPEATED_CLOCK_START;
                handler.forget(vec1, vec2, k, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("",n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ForgetAny(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ForgetAny")

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

            ForgetHandlerAny handler(n, k);

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size / n);
            fillVector(vec2);
            for(size_t idx = 0; idx < k; idx++) {
                REPEATED_CLOCK_START;
                    handler.forget(vec1, vec2, k, idx);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("",n, k, idx, d);
                }
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ForgetCombined(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ForgetCombined")

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

            ForgetHandlerCombined handler(n, k);

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size / n);
            fillVector(vec2);
            for(size_t idx = 0; idx < k; idx++) {
                REPEATED_CLOCK_START;
                    handler.forget(vec1, vec2, k, idx);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("",n, k, idx, d);
                }
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceIterator(TestSettings &settings, TestLogger &logger) {
    IteratorIntroduceHandler ih;
    IntroduceComplete(settings, logger, ih, "Iterator");
    IntroduceOneEdge(settings, logger, ih, "Iterator");
    IntroduceConsistency(settings, logger, ih, "Iterator");
}

void TestFactory::IntroduceCompute(TestSettings &settings, TestLogger &logger) {
    IntroduceHandlerCompute ih;
    IntroduceComplete(settings, logger, ih, "Compute");
    IntroduceOneEdge(settings, logger, ih, "Compute");
    IntroduceConsistency(settings, logger, ih, "Compute");
}

void TestFactory::IntroducePrecomputed(TestSettings &settings, TestLogger &logger) {
    IntroduceCompletePrecomputed(settings, logger);
    IntroduceOneEdgePrecomputed(settings, logger);
    IntroduceConsistencyPrecomputed(settings, logger);
}

void TestFactory::IntroduceComplete(TestSettings &settings, TestLogger &logger,
        IntroduceHandler& ih, const std::string& handlername) {
    BEGIN_TEST("IntroduceComplete" + handlername)

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            GraphGenerator::Clique(h, k);
            GraphGenerator::Clique(g, n);
            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceOneEdge(TestSettings &settings, TestLogger &logger,
        IntroduceHandler& ih, const std::string& handlername) {
    BEGIN_TEST("IntroduceOneEdge" + handlername)

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            h->clear(k);
            h->addEdge(0, 1);
            GraphGenerator::Clique(g, n);
            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceConsistency(TestSettings &settings, TestLogger &logger,
                                    IntroduceHandler& ih, const std::string& handlername) {
    BEGIN_TEST("IntroduceConsistency" + handlername)

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            GraphGenerator::Clique(h, k);
            GraphGenerator::EdgeProbabilityGraph(g, n, 0.5);
            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceCompletePrecomputed(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("IntroduceCompletePrecomputed")

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            GraphGenerator::Clique(h, k);
            GraphGenerator::Clique(g, n);

            auto pre = EdgeConsistencyPrecomputation::Initialize(g, k - 1);
            IntroduceHandlerPrecomputed ih(pre);

            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceOneEdgePrecomputed(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("IntroduceOneEdgePrecomputed")

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            h->clear(k);
            h->addEdge(0, 1);
            GraphGenerator::Clique(g, n);

            auto pre = EdgeConsistencyPrecomputation::Initialize(g, 1);
            IntroduceHandlerPrecomputed ih(pre);

            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::IntroduceConsistencyPrecomputed(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("IntroduceConsistencyPrecomputed")

    std::vector<size_t> vec1, vec2, bag;

    auto h = AdjacencyMatrixGraph::testGraph(), g = AdjacencyMatrixGraph::testGraph();

    STEPLOOP_START

            bag.resize(k - 1);
            for(int i = 0; i < bag.size(); i++) {
                bag[i] = i + 1;
            }
            vec1.resize(size / n);
            fillVector(vec1);
            vec2.resize(size);
            GraphGenerator::Clique(h, k);
            GraphGenerator::EdgeProbabilityGraph(g, n, 0.5);

            auto pre = EdgeConsistencyPrecomputation::Initialize(g, k - 1);
            IntroduceHandlerPrecomputed ih(pre);

            REPEATED_CLOCK_START;
                ih.introduceLast(vec1, vec2, bag, h, g, n, 0);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, d);
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::joinHandler(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("JoinHandler")

    JoinHandler jh;

    std::vector<size_t> vec1, vec1Copy, vec2;

    STEPLOOP_START

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size);
        fillVector(vec2);
        vec1Copy = vec1;
        REPEATED_CLOCK_START;
        jh.join(vec1Copy, vec2);
        REPEATED_CLOCK_END;
        for(int d : durations) {
            logger.Log("", n, k, d);
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::InsertClosedForm(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("InsertClosedForm")

    CalculationRemapper remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size);
        remapper.SetSizes(n, k);
        for(int pos = 0; pos < k - 1; pos++) {
            REPEATED_CLOCK_START;
            remapper.Insert(vec1, vec2, pos);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, pos, d);
            }
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ExtractClosedForm(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ExtractClosedForm")

    CalculationRemapper remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size);
        remapper.SetSizes(n, k);
        for(int pos = 0; pos < k - 1; pos++) {
            REPEATED_CLOCK_START;
            remapper.Extract(vec1, vec2, pos);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, pos, d);
            }
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::InsertIterator(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("InsertIterator")

    IteratorRemapper remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size);
        remapper.SetSizes(n, k);
        for(int pos = 0; pos < k - 1; pos++) {
            REPEATED_CLOCK_START;
            remapper.Insert(vec1, vec2, pos);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, pos, d);
            }
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ExtractIterator(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ExtractIterator")

    IteratorRemapper remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

        vec1.resize(size);
        fillVector(vec1);
        vec2.resize(size);
        remapper.SetSizes(n, k);
        for(int pos = 0; pos < k - 1; pos++) {
            REPEATED_CLOCK_START;
            remapper.Extract(vec1, vec2, pos);
            REPEATED_CLOCK_END;
            for(int d : durations) {
                logger.Log("", n, k, pos, d);
            }
        }

    STEPLOOP_END

    END_TEST
}

void TestFactory::MaxDegreeHomomorphismCount(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("MaxDegreeHomomorphismCount");
    
    std::shared_ptr<EdgeSetGraph> h = std::make_shared<EdgeSetGraph>(0);
    GraphGenerator::Cycle(h, 4);
    
    std::shared_ptr<EdgeSetGraph> g = std::make_shared<EdgeSetGraph>(0);
    
    for(int n = 2; n < 65; n = n*2) {
        GraphGenerator::CompleteGrid(g, n, n);
        REPEATED_CLOCK_START;
        TraversalHomomorphismCounter::Count(h, g);
        REPEATED_CLOCK_END;
        
        for(int i = 0; i < durations.size(); i++) {
            logger.Log("SquareInGrid", n*n, 4, durations[i]);
        }
    }
    
    END_TEST;
}

void TestFactory::CyclesInMaxDegreeRandom(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("CyclesInMaxDegree5Random");
    std::shared_ptr<EdgeSetGraph> setH = std::make_shared<EdgeSetGraph>(0);
    std::shared_ptr<EdgeSetGraph> setG = std::make_shared<EdgeSetGraph>(0);
    
    std::shared_ptr<Graph> adjH = std::make_shared<AdjacencyMatrixGraph>(0);
    std::shared_ptr<Graph> adjG = std::make_shared<AdjacencyMatrixGraph>(0);
    
    TamakiRunner tr;
    
    
    
    for(int n = 8; n < 4097; n = n*2 ) {
        for(int k = 3; k < 6; k++) {
            GraphGenerator::Cycle(setH, k);
            GraphGenerator::MaxDegreeRandomGraph(setG, n, 5);
            
            REPEATED_CLOCK_START
            TraversalHomomorphismCounter::Count(setH, setG);
            REPEATED_CLOCK_END;
            
            for(int i = 0; i < durations.size(); i++) {
                logger.Log("maxDegree",n, k, durations[i]);
            }
            
            if( (n > 1023 && k > 3) || n > 1024) {
                continue;
            }
            
            GraphGenerator::Cycle(adjH, k);
            GraphGenerator::FromGraph(adjG, setG);

            std::shared_ptr<TreeDecomposition> td = tr.decompose(adjH);
            std::shared_ptr<NiceTreeDecomposition> ntd = NiceTreeDecomposition::FromTd(td);
            
            HomomorphismSettings setting = ConfigurationFactory::defaultSettings(n, k);

            REPEATED_CLOCK_START
            HomomorphismCounter(adjH, adjG, ntd, setting).compute();
            REPEATED_CLOCK_END;
            
            for(int i = 0; i < durations.size(); i++) {
                logger.Log("treeWidth",n, k, durations[i]);
            }
        }
    }
    
    END_TEST;
}

void TestFactory::StarsIsMaxDegreeKRandom(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("StarsIsMaxDegreeKRandom");
    std::shared_ptr<EdgeSetGraph> setH = std::make_shared<EdgeSetGraph>(0);
    std::shared_ptr<EdgeSetGraph> setG = std::make_shared<EdgeSetGraph>(0);
    
    std::shared_ptr<Graph> adjH = std::make_shared<AdjacencyMatrixGraph>(0);
    std::shared_ptr<Graph> adjG = std::make_shared<AdjacencyMatrixGraph>(0);
    
    TamakiRunner tr;

    for(int n = 8; n < 4097; n = n*2 ) {
        for(int k = 3; k < 6; k++) {
            GraphGenerator::Star(setH, k);
            GraphGenerator::MaxDegreeRandomGraph(setG, n, k);
            
            REPEATED_CLOCK_START
            TraversalHomomorphismCounter::Count(setH, setG);
            REPEATED_CLOCK_END;
            
            for(int i = 0; i < durations.size(); i++) {
                logger.Log("maxDegree",n, k, durations[i]);
            }
            
            GraphGenerator::Star(adjH, k);
            
            GraphGenerator::FromGraph(adjG, setG);

            std::shared_ptr<TreeDecomposition> td = tr.decompose(adjH);
            std::shared_ptr<NiceTreeDecomposition> ntd = NiceTreeDecomposition::FromTd(td);
            
            HomomorphismSettings setting = ConfigurationFactory::defaultSettings(n, k);

            REPEATED_CLOCK_START
            HomomorphismCounter(adjH, adjG, ntd, setting).compute();
            REPEATED_CLOCK_END;
            
            for(int i = 0; i < durations.size(); i++) {
                logger.Log("treeWidth",n, k, durations[i]);
            }
        }
    }
    
    END_TEST;
}

int TestFactory::milliSecondDifferene(std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::time_point<std::chrono::steady_clock> stop) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
}

void TestFactory::fillVector(std::vector<size_t> data) {
    for(auto & entry : data) {
        entry = rand();
    }
}

float TestFactory::averageDuration(std::vector<int> durations)
{
    float sum = 0;
    for(int i = 0 ; i <durations.size(); i++) {
        sum += durations[i];
    }
    return sum / durations.size();
}

void TestFactory::InsertClosedVariants(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("InsertClosedVariants")

    CalculationRemapperInsertVarients remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertXYZ(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("XYZ", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertXZY(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("XZY", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertYXZ(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("YXZ", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertYZX(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("YZX", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertZXY(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("ZXY", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.InsertZYX(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("ZYX", n, k, pos, d);
                }
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::ExtractClosedVariants(TestSettings &settings, TestLogger &logger) {
    BEGIN_TEST("ExtractClosedVariants")

    CalculationRemapperExtractVarients remapper;

    std::vector<size_t> vec1, vec2;

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractXYZ(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("XYZ", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractXZY(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("XZY", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractYXZ(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("YXZ", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractYZX(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("YZX", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractZXY(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("ZXY", n, k, pos, d);
                }
            }

    STEPLOOP_END

    STEPLOOP_START

            vec1.resize(size);
            fillVector(vec1);
            vec2.resize(size);
            remapper.SetSizes(n, k);
            for(int pos = 0; pos < k - 1; pos++) {
                REPEATED_CLOCK_START;
                    remapper.ExtractZYX(vec1, vec2, pos);
                REPEATED_CLOCK_END;
                for(int d : durations) {
                    logger.Log("ZYX", n, k, pos, d);
                }
            }

    STEPLOOP_END

    END_TEST
}

void TestFactory::MemoryTest1(TestSettings &settings, TestLogger &logger) {
    std::shared_ptr<AdjacencyMatrixGraph> h = AdjacencyMatrixGraph::testGraph();
    std::shared_ptr<AdjacencyMatrixGraph> g  = AdjacencyMatrixGraph::testGraph();
    
    GraphGenerator::Clique(h, 4);
    GraphGenerator::EdgeProbabilityGraph(g, 128, 0.02);

    Main::subgraphsGraph(h, g);
    
    
    GraphGenerator::Clique(h, 4);
    GraphGenerator::EdgeProbabilityGraph(g, 128, 0.02);

    Main::subgraphsGraph(h, g);
}
