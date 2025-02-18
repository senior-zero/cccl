//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCUDACXX___TYPE_TRAITS_IS_SIGNED_INTEGER_H
#define _LIBCUDACXX___TYPE_TRAITS_IS_SIGNED_INTEGER_H

#include <cuda/std/detail/__config>

#if defined(_CCCL_IMPLICIT_SYSTEM_HEADER_GCC)
#  pragma GCC system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_CLANG)
#  pragma clang system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_MSVC)
#  pragma system_header
#endif // no system header

#include <cuda/std/__type_traits/integral_constant.h>

_LIBCUDACXX_BEGIN_NAMESPACE_STD

template <class _Tp>
struct __cccl_is_signed_integer : public false_type
{};
template <>
struct __cccl_is_signed_integer<signed char> : public true_type
{};
template <>
struct __cccl_is_signed_integer<signed short> : public true_type
{};
template <>
struct __cccl_is_signed_integer<signed int> : public true_type
{};
template <>
struct __cccl_is_signed_integer<signed long> : public true_type
{};
template <>
struct __cccl_is_signed_integer<signed long long> : public true_type
{};
#if _CCCL_HAS_INT128()
template <>
struct __cccl_is_signed_integer<__int128_t> : public true_type
{};
#endif // _CCCL_HAS_INT128()

_LIBCUDACXX_END_NAMESPACE_STD

#endif // _LIBCUDACXX___TYPE_TRAITS_IS_SIGNED_INTEGER_H
