#include "Interface.hpp"
#include "test.hpp"

#define FILENAME "result.csv"

//#define STRESSTEST
//#define BASETEST
//#define DIFFTEST

int main() {
#ifdef STRESSTEST
    StressTest(FILENAME);
#endif

#ifdef BASETEST
    TreeBaseOperationsTest();
#endif

#ifdef DIFFTEST
    TreeDiffOperationsTest();
#endif

    Run();

    return 0;
}