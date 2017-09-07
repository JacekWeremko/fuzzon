/*
 * Copyright [2017] Jacek Weremko
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/mpl/list.hpp>

#include "../../src/fuzzon_mutator.h"
#include "../../src/fuzzon_random.h"

int CalculateDifferentBitsCount(const std::vector<char>& base,
                                const std::vector<char>& check_me) {
  auto different_bits_count = 0;
  for (auto i = 0; i < std::min(base.size(), check_me.size()); ++i) {
    auto only_one_bit_should_be_set = base[i] ^ check_me[i];
    for (auto j = 0; j < 8; j++) {
      bool lowset_bit = (only_one_bit_should_be_set >> j) & 0x1;
      if (lowset_bit) {
        different_bits_count++;
      }
    }
  }
  return different_bits_count;
}

void TestIfMutatorBitFlipChangesOnlySingleBitImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {0b00011, 0b00101100, 0b00000, 0b01010};
  std::vector<char> mutate_me = test_data;

  mutator.FlipBit(mutate_me, 0, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 1);
  BOOST_TEST(mutate_me[0] == 0b00010);

  mutator.FlipBit(mutate_me, 2 * 8 + 4, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 1 + 1);
  BOOST_TEST(mutate_me[2] == 0b10000);

  mutator.FlipBit(mutate_me, 3 * 8 + 1, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 2 + 1);
  BOOST_TEST(mutate_me[3] == 0b01000);

  for (auto i = 1; i < 7; ++i) {
    mutator.FlipBit(mutate_me, 8 + i, 1);
  }
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 3 + 6);
  BOOST_TEST(0b1010010 == mutate_me[1]);
}

void TestIfMutatorBitFlipChangesMultipleBitsInRowImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {0b00011, 0b101100, 0b00000, 0b01010};
  std::vector<char> mutate_me = test_data;

  mutator.FlipBit(mutate_me, 0, 2);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 2);
  BOOST_TEST(mutate_me[0] == 0b00000);

  mutator.FlipBit(mutate_me, 2 * 8 + 4, 3);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == (2 + 3));
  BOOST_TEST(mutate_me[2] == 0b01110000);

  mutator.FlipBit(mutate_me, 3 * 8 + 1, 4);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == (5 + 4));
  BOOST_TEST(mutate_me[3] == 0b10100);
}

void TestIfMutatorByteFlipChangesOnlySingleByteImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {0b00011, 0b00101100, 0b00000, 0b01010};
  std::vector<char> mutate_me = test_data;

  mutator.FlipByte(mutate_me, 0, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 1 * 8);
  BOOST_TEST(mutate_me[0] == (char)0b11111100);

  mutator.FlipByte(mutate_me, 3, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 2 * 8);
  BOOST_TEST(mutate_me[3] == (char)0b11110101);

  mutator.FlipByte(mutate_me, 2, 1);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 3 * 8);
  BOOST_TEST(mutate_me[2] == (char)0b11111111);
}

void TestIfMutatorByteFlipChangesMultipleByteInRowImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {0b00011, 0b101100, 0b00000, 0b01010};
  std::vector<char> mutate_me = test_data;

  mutator.FlipByte(mutate_me, 0, 2);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == 2 * 8);
  BOOST_TEST(mutate_me[0] == (char)0b11111100);
  BOOST_TEST(mutate_me[1] == (char)0b11010011);

  mutator.FlipByte(mutate_me, 1, 3);
  BOOST_TEST(CalculateDifferentBitsCount(test_data, mutate_me) == (3) * 8);
  BOOST_TEST(mutate_me[0] == (char)0b11111100);
  BOOST_TEST(mutate_me[2] == (char)0b11111111);
  BOOST_TEST(mutate_me[3] == (char)0b11110101);
}

void TestMutatorSimpleArithmeticsCorrectnesImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {-2, 11, 40};
  std::vector<char> mutate_me = test_data;

  mutator.SimpleArithmetics(mutate_me, 0, 2);
  mutator.SimpleArithmetics(mutate_me, 2, -7);
  mutator.SimpleArithmetics(mutate_me, 1, 9);

  BOOST_TEST(mutate_me[0] == 0);
  BOOST_TEST(mutate_me[1] == 20);
  BOOST_TEST(mutate_me[2] == 33);
}

void TestMutatorKnownIntegersCorrectnesImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {-2, 11, 40};
  std::vector<char> mutate_me = test_data;

  mutator.KnownIntegers(mutate_me, 0, 2);
  mutator.KnownIntegers(mutate_me, 2, -7);
  mutator.KnownIntegers(mutate_me, 1, 9);

  BOOST_TEST(mutate_me[0] == 2);
  BOOST_TEST(mutate_me[1] == 9);
  BOOST_TEST(mutate_me[2] == -7);
}

void TestMutatorBlockInsertionCorrectnesImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {-2, 11, 40};
  const std::vector<char> insert_me = {1, 2, 3};

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockInsertion(mutate_me, 0, insert_me, 0, insert_me.size());

    std::vector<char> expectation = {1, 2, 3, -2, 11, 40};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockInsertion(mutate_me, 2, insert_me, 0, insert_me.size());

    std::vector<char> expectation = {-2, 11, 1, 2, 3, 40};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockInsertion(mutate_me, 2, insert_me, 1, 2);

    std::vector<char> expectation = {-2, 11, 2, 3, 40};
    BOOST_TEST(mutate_me == expectation);
  }
}

void TestMutatorBlockDeletionCorrectnesImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {-2, 11, 40, 1, 2, 3};

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockDeletion(mutate_me, 0, 1);

    std::vector<char> expectation = {11, 40, 1, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockDeletion(mutate_me, 1, 3);

    std::vector<char> expectation = {-2, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockDeletion(mutate_me, 3, 1);

    std::vector<char> expectation = {-2, 11, 40, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }
}

void TestMutatorBlockMemsetCorrectnesImpl() {
  fuzzon::Mutator mutator;

  const std::vector<char> test_data = {-2, 11, 40, 1, 2, 3};

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockMemset(mutate_me, 0, 1, 2);

    std::vector<char> expectation = {2, 11, 40, 1, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockMemset(mutate_me, 1, 3, -7);

    std::vector<char> expectation = {-2, -7, -7, -7, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }

  {
    std::vector<char> mutate_me = test_data;
    mutator.BlockMemset(mutate_me, 3, 1, 1);

    std::vector<char> expectation = {-2, 11, 40, 1, 2, 3};
    BOOST_TEST(mutate_me == expectation);
  }
}

void TestMutatorWhiteSpacePreservationImpl() {
  fuzzon::Mutator mutator(true);

  const std::vector<char> test_data = {'a', ' ', 11, '\r', '\n', 'b'};
  std::vector<char> mutate_me = test_data;

  BOOST_TEST(mutator.FlipByte(mutate_me, 0, 1));
  BOOST_TEST(!mutator.FlipByte(mutate_me, 1, 1));
  BOOST_TEST(!mutator.FlipByte(mutate_me, 2, 1));
  BOOST_TEST(!mutator.FlipByte(mutate_me, 3, 1));
  BOOST_TEST(!mutator.FlipByte(mutate_me, 4, 1));
  BOOST_TEST(mutator.FlipByte(mutate_me, 5, 1));
}

//
// void TestIfMutatorByteFlipChangesOnlySingleByteImpl() {
//  std::string input_alphabet(
//      "abcdefghijklmnopqrstuvwxyz"
//      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//      "1234567890");
//  fuzzon::Mutator mutator_(input_alphabet);
//
//  for (size_t iters = 0; iters < 10; iters++) {
//    uint8_t test_data[10] = {'A', 'B', 'c', 'd', '5', '6', '7', '8', '9',
//    '0'};
//    uint8_t mutate_me[10] = {'A', 'B', 'c', 'd', '5', '6', '7', '8', '9',
//    '0'};
//    mutator_.FlipByte(&mutate_me[0], sizeof(mutate_me));
//
//    int mutated_bytes = 0;
//    for (size_t i = 0; i < 10; ++i) {
//      if (test_data[i] != mutate_me[i]) {
//        mutated_bytes++;
//      }
//    }
//    BOOST_TEST(mutated_bytes == 1);
//  }
//}
//
// void TestIfMutatorMinMaxValueChangesOnlySingleByteImpl() {
//  std::string input_alphabet(
//      "abcdefghijklmnopqrstuvwxyz"
//      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//      "1234567890"
//      "!@#$%^&*()"
//      "`~-_=+[{]}\\|;:'\",<.>/? ");
//  fuzzon::Mutator mutator_(input_alphabet);
//
//  for (size_t iters = 0; iters < 10; iters++) {
//    uint8_t test_data[10] = {'A', 'B', 'c', 'd', '5', '6', '!', '^', ':',
//    ';'};
//    uint8_t mutate_me[10] = {'A', 'B', 'c', 'd', '5', '6', '!', '^', ':',
//    ';'};
//
//    mutator_.MinMaxValue(&mutate_me[0], sizeof(mutate_me));
//
//    int mutated_bytes = 0;
//    for (size_t i = 0; i < 10; ++i) {
//      if (test_data[i] != mutate_me[i]) {
//        mutated_bytes++;
//        BOOST_TEST((mutate_me[i] == 0 || mutate_me[i] == 255));
//      }
//    }
//    BOOST_TEST(mutated_bytes == 1);
//  }
//}
//
// void TestIfMutatorByteChangePreserveCharacterTypeImpl() {
//  std::string input_alphabet(
//      "abcdefghijklmnopqrstuvwxyz"
//      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//      "1234567890"
//      "!@#$%^&*()"
//      "`~-_=+[{]}\\|;:'\",<.>/? ");
//  fuzzon::Mutator mutator_(input_alphabet);
//
//  for (size_t iters = 0; iters < 10; iters++) {
//    uint8_t test_data[10] = {'A', 'B', 'c', 'd', '5', '6', '!', '^', ':',
//    ';'};
//    uint8_t mutate_me[10] = {'A', 'B', 'c', 'd', '5', '6', '!', '^', ':',
//    ';'};
//
//    mutator_.ChangeByte(&mutate_me[0], sizeof(mutate_me));
//
//    int mutated_bytes = 0;
//    for (size_t i = 0; i < 10; ++i) {
//      if (test_data[i] != mutate_me[i]) {
//        mutated_bytes++;
//        auto test_date_type =
//        fuzzon::Random::Get()->FindCharType(test_data[i]);
//        auto mutate_me_type =
//        fuzzon::Random::Get()->FindCharType(mutate_me[i]);
//
//        BOOST_TEST(test_date_type == mutate_me_type);
//      }
//    }
//    BOOST_TEST(mutated_bytes == 1);
//  }
//}

BOOST_AUTO_TEST_SUITE(fuzzon_mutator_suite)

BOOST_AUTO_TEST_CASE(TestIfMutatorBitFlipChangesOnlySingleBit) {
  TestIfMutatorBitFlipChangesOnlySingleBitImpl();
}
BOOST_AUTO_TEST_CASE(TestIfMutatorBitFlipChangesMultipleBitsInRow) {
  TestIfMutatorBitFlipChangesMultipleBitsInRowImpl();
}

BOOST_AUTO_TEST_CASE(TestIfMutatorByteFlipChangesOnlySingleByte) {
  TestIfMutatorByteFlipChangesOnlySingleByteImpl();
}
BOOST_AUTO_TEST_CASE(TestIfMutatorByteFlipChangesMultipleByteInRow) {
  TestIfMutatorByteFlipChangesMultipleByteInRowImpl();
}

BOOST_AUTO_TEST_CASE(TestMutatorSimpleArithmeticsCorrectnes) {
  TestMutatorSimpleArithmeticsCorrectnesImpl();
}
BOOST_AUTO_TEST_CASE(TestMutatorKnownIntegersCorrectnes) {
  TestMutatorKnownIntegersCorrectnesImpl();
}
BOOST_AUTO_TEST_CASE(TestMutatorBlockDeletionCorrectnes) {
  TestMutatorBlockDeletionCorrectnesImpl();
}
BOOST_AUTO_TEST_CASE(TestMutatorBlockMemsetCorrectnes) {
  TestMutatorBlockMemsetCorrectnesImpl();
}

BOOST_AUTO_TEST_CASE(TestMutatorWhiteSpacePreservation) {
  TestMutatorWhiteSpacePreservationImpl();
}

BOOST_AUTO_TEST_SUITE_END()
