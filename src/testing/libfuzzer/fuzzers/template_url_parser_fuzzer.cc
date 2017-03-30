// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <stdint.h>

#include <random>
#include <string>

#include "components/search_engines/search_terms_data.h"
#include "components/search_engines/template_url.h"
#include "components/search_engines/template_url_parser.h"

class PseudoRandomFilter : public TemplateURLParser::ParameterFilter {
 public:
  PseudoRandomFilter(uint32_t seed) : generator_(seed), pool_(0, 1) {}
  ~PseudoRandomFilter() override = default;

  bool KeepParameter(const std::string&, const std::string&) override {
    return pool_(generator_);
  }

 private:
  std::mt19937 generator_;
  std::uniform_int_distribution<uint8_t> pool_;
};

struct FuzzerFixedParams {
  uint32_t seed_;
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < sizeof(FuzzerFixedParams)) {
    return 0;
  }
  const FuzzerFixedParams* params =
      reinterpret_cast<const FuzzerFixedParams*>(data);
  size -= sizeof(FuzzerFixedParams);
  const char* char_data = reinterpret_cast<const char*>(params + 1);
  PseudoRandomFilter filter(params->seed_);
  TemplateURLParser::Parse(SearchTermsData(), char_data, size, &filter);
  return 0;
}