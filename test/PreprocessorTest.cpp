#include "gmock/gmock.h"
#include "Preprocessor.h"


class PreprocessorTest: public testing::Test {
public:
};


TEST_F(PreprocessorTest, processIfdef) {
  ASSERT_TRUE(false);
}


TEST_F(PreprocessorTest, processIfndef) {
  ASSERT_TRUE(false);
}
