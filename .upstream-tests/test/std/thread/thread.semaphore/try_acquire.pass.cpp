//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: libcpp-has-no-threads
// UNSUPPORTED: pre-sm-60

// <cuda/std/semaphore>

#include <cuda/std/semaphore>
#include <cuda/std/thread>

#include "test_macros.h"

int main(int, char**)
{
  cuda::std::counting_semaphore s(1);

  assert(s.try_acquire());
  s.release();
  assert(s.try_acquire());
  s.release(2);
  cuda::std::thread t([&](){
    assert(s.try_acquire());
  });
  t.join();
  assert(s.try_acquire());

  return 0;
}
