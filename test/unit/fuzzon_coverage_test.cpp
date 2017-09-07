/*
 * Copyright [2017] Jacek Weremko
 */

#include "../../shared/fuzzon_coverage.h"

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/mpl/list.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE(fuzzon_coverate_suit)

void VerifyOperatorDoubleEqualReturnTrueForTheSameDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    test.TracePC(i);
  }
  base.ComputeHash();
  test.ComputeHash();

  BOOST_TEST(base == test);
  return;
}

void VerifyOperatorDoubleEqualReturnFalseForTheDifferentDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    test.TracePC(i);
  }
  test.TracePC(6);

  base.ComputeHash();
  test.ComputeHash();

  BOOST_TEST(!(base == test));
  return;
}

void VerifyOperatorDoubleEqualReturnTrueForSimilarCompresedDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  base.SetPCLimit(20);
  test.SetPCLimit(20);
  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    base.TracePC(i);
    base.TracePC(i);
    base.TracePC(i);

    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
  }
  base.Compress(fuzzon::Coverage::Log2);
  test.Compress(fuzzon::Coverage::Log2);

  base.ComputeHash();
  test.ComputeHash();

  BOOST_TEST(base == test);
  return;
}

void VerifyIfIsTheSameReturnTrueForTheSameDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    test.TracePC(i);
  }

  BOOST_TEST(base.IsTheSame(test));
  return;
}

void VerifyIfIsTheSameReturnFalseForTheDifferentDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    test.TracePC(i);
  }
  test.TracePC(6);

  BOOST_TEST(!base.IsTheSame(test));
  return;
}

void VerifyIfIsTheSameReturnTrueForSimilarCompresedDataImpl() {
  fuzzon::Coverage base(fuzzon::Coverage::Raw);
  fuzzon::Coverage test(fuzzon::Coverage::Raw);

  for (size_t i = 0; i < 10; i++) {
    base.TracePC(i);
    base.TracePC(i);
    base.TracePC(i);
    base.TracePC(i);

    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
    test.TracePC(i);
  }
  base.Compress(fuzzon::Coverage::Log2);
  test.Compress(fuzzon::Coverage::Log2);

  BOOST_TEST(base.IsTheSame(test));
  return;
}

BOOST_AUTO_TEST_CASE(VerifyOperatorDoubleEqualReturnTrueForTheSameData) {
  VerifyOperatorDoubleEqualReturnTrueForTheSameDataImpl();
}
BOOST_AUTO_TEST_CASE(VerifyOperatorDoubleEqualReturnFalseForTheDifferentData) {
  VerifyOperatorDoubleEqualReturnFalseForTheDifferentDataImpl();
}
BOOST_AUTO_TEST_CASE(
    VerifyOperatorDoubleEqualReturnTrueForSimilarCompresedData) {
  VerifyOperatorDoubleEqualReturnTrueForSimilarCompresedDataImpl();
}

BOOST_AUTO_TEST_CASE(VerifyIfIsTheSameReturnTrueForTheSameData) {
  VerifyIfIsTheSameReturnTrueForTheSameDataImpl();
}
BOOST_AUTO_TEST_CASE(VerifyIfIsTheSameReturnFalseForTheDifferentData) {
  VerifyIfIsTheSameReturnFalseForTheDifferentDataImpl();
}
BOOST_AUTO_TEST_CASE(VerifyIfIsTheSameReturnTrueForSimilarCompresedData) {
  VerifyIfIsTheSameReturnTrueForSimilarCompresedDataImpl();
}

BOOST_AUTO_TEST_SUITE_END()
