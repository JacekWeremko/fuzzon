//============================================================================
// Name        : Branchness.cpp
// Author      : Jacek Weremko
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

#include <stdint.h>
#include <stddef.h>

bool func_a(char a) {
  if (a == 'a') {
    return true;
  }
  return false;
}

bool func_b(char b) {
  return b == 'b' ? true : false;
}

bool func_c(char c) {
  return c == 'c' ? true : false;
}

bool func_d(char d) {
  return d == 'd' ? true : false;
}

void crash_me() {
  volatile int* pc = 0;
  volatile int crash = *pc;
  return;
}

int test(const char* data, size_t size) {
  if (data == NULL) {
    return 0;
  }

  //  std::cout << "size :" << size << std::endl;
  int guard = 0;
  if (size < 8) {
    return 0;
  }

  //  std::cout << "start branchness" << std::endl;
  std::cout << std::string(data) << std::endl;
  if (func_a(data[0])) {
    guard++;
    if (func_b(data[1])) {
      guard++;
      if (func_c(data[2])) {
        guard++;
        if (func_d(data[3])) {
          guard++;
          crash_me();
        }
      }
    }
  }
  std::cout << "guard :" << guard << std::endl;

  // crash_me();

  if (func_a(data[4])) {
    guard++;
    if (func_b(data[5])) {
      guard++;
      if (func_c(data[6])) {
        guard++;
        if (func_d(data[7])) {
          guard++;
          //          crash_me();
        }
      }
    }
  }
  std::cout << "guard :" << guard << std::endl;
  return 0;
}

int main(int argc, char** argv) {
  //  std::cout << "argc :" << argc << std::endl;
  if (argc < 2) {
    return 0;
  }
  std::string argv1 = std::string(argv[1]);
  char data[9] = {0};
  size_t size = std::min(static_cast<int>(argv1.size()), 8);
  std::memcpy(&data[0], argv1.c_str(), size);
  data[8] = '\0';
  return test(&data[0], size);
}
