/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_coverage.h"

#include <sstream>
#include <cmath>
#include <limits>

#include "./utils/logger.h"

namespace fuzzon {

Coverage::Coverage(TrackMode mode)
    : mode_(mode),
      pc_total_(std::numeric_limits<uint32_t>::max()),
      pc_visited_(0),
      last_pc_(0) {
  flow_pcs_.fill(0);
  flow_data_.fill(0);
}

void Coverage::Reset() {
  pc_visited_ = 0;
  last_pc_ = 0;
  flow_pcs_.fill(0);
  flow_data_.fill(0);
  hash_pcs_.fill(0);
  hash_data_.fill(0);
}

void Coverage::Compress(CompreseMode comprese_mode) {
  if (comprese_mode == CompreseMode::Log2) {
    for (auto& elem : flow_pcs_) {
      if (elem == 0) {
        continue;
      }
      elem = std::log2(elem) + 1;
      pc_visited_++;
    }
    //
    //    //    for (auto& elem : flow_data_) {
    //    //      if (elem == 0) {
    //    //        continue;
    //    //      }
    //    //      elem = std::log2(elem) + 1;
    //    //    }
  }
}

void Coverage::ComputeHash() {
  MD5(reinterpret_cast<const unsigned char*>(flow_pcs_.data()),
      flow_pcs_.size() * sizeof(flow_pcs_[0]), hash_pcs_.data());

  MD5(reinterpret_cast<const unsigned char*>(flow_data_.data()),
      flow_data_.size() * sizeof(flow_data_[0]), hash_data_.data());
}

void Coverage::Merge(const Coverage& merge_me) {
  pc_total_ = merge_me.pc_total_;
  for (size_t idx = 0; idx < flow_pcs_.size(); idx++) {
    flow_pcs_[idx] += merge_me.flow_pcs_[idx];
  }
}

// Info getters
int Coverage::GetTotalPCCounter() const {
  return pc_total_;
}

int Coverage::GetVisitedPCCounter() {
  pc_visited_ = 0;
  for (size_t idx = 0; idx < flow_pcs_.size(); ++idx) {
    if (flow_pcs_[idx]) {
      pc_visited_ += 1;
    }
  }
  return pc_visited_;
}

bool Coverage::operator==(const Coverage& compare_with_me) const {
  auto pcs_match = hash_pcs_ == compare_with_me.hash_pcs_;
  auto data_match = hash_data_ == compare_with_me.hash_data_;
  return pcs_match && data_match;
}

bool Coverage::operator!=(const Coverage& compare_with_me) const {
  return !(*this == compare_with_me);
}

bool Coverage::IsPcFlowTheSame(const Coverage& compare_with_me) const {
  for (size_t idx = 0; idx < flow_pcs_.size(); idx++) {
    if (flow_pcs_[idx] != compare_with_me.flow_pcs_[idx]) {
      return false;
    }
  }
  return true;
}

bool Coverage::IsDataFlowTheSame(const Coverage& compare_with_me) const {
  for (size_t idx = 0; idx < flow_data_.size(); idx++) {
    if (flow_data_[idx] != compare_with_me.flow_data_[idx]) {
      return false;
    }
  }
  return true;
}

void Coverage::SetPCGuardsCount(size_t value) {
  LOG_DEBUG("SetPCGuardsCount : " + std::to_string(value));
  pc_total_ = value;
}

void Coverage::TracePC(uintptr_t pc) {
  uintptr_t current = 0;
  if (mode_ == TrackMode::Raw) {
    current = pc;
  } else {
    current = last_pc_ == 0 ? pc : (pc ^ last_pc_);
  }
  TracePC(current, pc);
}

void Coverage::TracePC(uint32_t idx, uintptr_t pc) {
  flow_pcs_[idx % flow_pcs_.size()]++;
  last_pc_ = pc >> 1;
}

void Coverage::PrintTrace() const {
  std::stringstream output;
  output << this;
  LOG_INFO("pc_flow : " + output.str());
  return;
}

} /* namespace fuzzon */
