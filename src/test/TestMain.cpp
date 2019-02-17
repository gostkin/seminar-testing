#include "StdAfxTest.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  const int code = RUN_ALL_TESTS();
  printf("[==========]\r\n\r\nTests execution completed with code: %d \r\n\r\n", code);
  return code;
}