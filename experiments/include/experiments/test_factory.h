//
//  test_factory.h
//  test
//
//  Created by Jonas Mortensen on 26/02/2020.
//

#ifndef EXPERIMENTS_TEST_FACTORY_H
#define EXPERIMENTS_TEST_FACTORY_H

#include "experiments/test_settings.h"
#include "experiments/test_logger.h"

#include <chrono>
#include <functional>
#include <memory>

#include "homomorphism/introduce_handler.h"

class TestFactory
{
public:
    static std::function<void(TestSettings&, TestLogger&)> GetTest(int i);
    static int TestCount();
    static std::vector<std::function<void(TestSettings&, TestLogger&)>> GetAllTests();
private:
    static void SquaresInGrid(TestSettings& settings, TestLogger& logger);
    static void BinaryTreeInBinaryTree(TestSettings& settings, TestLogger& logger);
    static void CliquesInClique(TestSettings& settings, TestLogger& logger);
    static void EdgesInPath(TestSettings& settings, TestLogger& logger);
    static void PathInRandomGraph(TestSettings& settings, TestLogger& logger);
    static void RandomPatternsInRandomGraph(TestSettings& settings, TestLogger& logger);
    static void ForgetLeastSignificant(TestSettings& settings, TestLogger& logger);
    static void ForgetMostSignificant(TestSettings& settings, TestLogger& logger);
    static void IntroduceIterator(TestSettings& settings, TestLogger& logger);
    static void IntroduceCompute(TestSettings& settings, TestLogger& logger);
    static void IntroduceComplete(TestSettings& settings, TestLogger& logger,
                    IntroduceHandler& ih, const std::string& handlername);
    static void IntroduceOneEdge(TestSettings& settings, TestLogger& logger,
                    IntroduceHandler& ih, const std::string& handlername);
    static void IntroduceConsistency(TestSettings& settings, TestLogger& logger,
                                 IntroduceHandler& ih, const std::string& handlername);
    static void joinHandler(TestSettings& settings, TestLogger& logger);
    static void InsertClosedForm(TestSettings& settings, TestLogger& logger);
    static void ExtractClosedForm(TestSettings& settings, TestLogger& logger);
    static void InsertIterator(TestSettings& settings, TestLogger& logger);
    static void ExtractIterator(TestSettings& settings, TestLogger& logger);
    static void MaxDegreeHomomorphismCount(TestSettings& settings, TestLogger& logger);
    static void CyclesInMaxDegreeRandom(TestSettings& settings, TestLogger& logger);
    static void StarsIsMaxDegreeKRandom(TestSettings& settings, TestLogger& logger);
    static int milliSecondDifferene(std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::time_point<std::chrono::steady_clock> stop);
    static float averageDuration(std::vector<int> durations);
    static void fillVector(std::vector<size_t> data);
};

#endif /* test_factory_h */
