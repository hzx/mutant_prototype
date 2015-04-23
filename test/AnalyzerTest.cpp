#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "Analyzer.h"


using std::string;
using std::vector;


class AnalyzerTest: public testing::Test {
public:
  AnalyzerTest();
  Analyzer analyzer;
  Module module;
  File* file;
};


AnalyzerTest::AnalyzerTest() {
  file = module.newFile();
  file->name = "test.mut";
}


TEST_F(AnalyzerTest, setTypes) {
  ASSERT_TRUE(false);
}
