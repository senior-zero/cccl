/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef _LIBCUDACXX___MDSPAN_LAYOUT_RIGHT_HPP
#define _LIBCUDACXX___MDSPAN_LAYOUT_RIGHT_HPP

#include <cuda/std/detail/__config>

#if defined(_CCCL_IMPLICIT_SYSTEM_HEADER_GCC)
#  pragma GCC system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_CLANG)
#  pragma clang system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_MSVC)
#  pragma system_header
#endif // no system header

#include <cuda/std/__mdspan/extents.h>
#include <cuda/std/__mdspan/layout_stride.h>
#include <cuda/std/__mdspan/macros.h>
#include <cuda/std/__type_traits/fold.h>
#include <cuda/std/__type_traits/is_constructible.h>
#include <cuda/std/__type_traits/is_convertible.h>
#include <cuda/std/__type_traits/is_nothrow_constructible.h>
#include <cuda/std/__utility/integer_sequence.h>
#include <cuda/std/cstddef>

_LIBCUDACXX_BEGIN_NAMESPACE_STD

//==============================================================================
template <class _Extents>
class layout_right::mapping
{
public:
  using extents_type = _Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_right;

private:
  static_assert(__detail::__is_extents_v<extents_type>,
                "layout_right::mapping must be instantiated with a specialization of _CUDA_VSTD::extents.");

  template <class>
  friend class mapping;

  // i0+(i1 + E(1)*(i2 + E(2)*i3))
  template <size_t _r, size_t _Rank>
  struct __rank_count
  {};

  template <size_t _r, size_t _Rank, class _Ip, class... _Indices>
  _CCCL_HOST_DEVICE constexpr index_type
  __compute_offset(index_type __offset, __rank_count<_r, _Rank>, const _Ip& __i, _Indices... __idx) const
  {
    return __compute_offset(__offset * __extents.template __extent<_r>() + __i, __rank_count<_r + 1, _Rank>(), __idx...);
  }

  template <class _Ip, class... _Indices>
  _CCCL_HOST_DEVICE constexpr index_type
  __compute_offset(__rank_count<0, extents_type::rank()>, const _Ip& __i, _Indices... __idx) const
  {
    return __compute_offset(__i, __rank_count<1, extents_type::rank()>(), __idx...);
  }

  _CCCL_HOST_DEVICE constexpr index_type
  __compute_offset(size_t __offset, __rank_count<extents_type::rank(), extents_type::rank()>) const
  {
    return static_cast<index_type>(__offset);
  }

  _CCCL_HOST_DEVICE constexpr index_type __compute_offset(__rank_count<0, 0>) const
  {
    return 0;
  }

public:
  //--------------------------------------------------------------------------------

  _CCCL_HIDE_FROM_ABI constexpr mapping() noexcept               = default;
  _CCCL_HIDE_FROM_ABI constexpr mapping(mapping const&) noexcept = default;

  _CCCL_HOST_DEVICE constexpr mapping(extents_type const& __exts) noexcept
      : __extents(__exts)
  {}

  _CCCL_TEMPLATE(class _OtherExtents)
  _CCCL_REQUIRES(_CCCL_TRAIT(_CUDA_VSTD::is_constructible, extents_type, _OtherExtents))
  __MDSPAN_CONDITIONAL_EXPLICIT((!_CUDA_VSTD::is_convertible<_OtherExtents, extents_type>::value)) // needs two () due
                                                                                                   // to comma
  _LIBCUDACXX_HIDE_FROM_ABI constexpr mapping(
    mapping<_OtherExtents> const& __other) noexcept // NOLINT(google-explicit-constructor)
      : __extents(__other.extents())
  {
    /*
     * TODO: check precondition
     * __other.required_span_size() is a representable value of type index_type
     */
  }

  _CCCL_TEMPLATE(class _OtherExtents)
  _CCCL_REQUIRES(_CCCL_TRAIT(_CUDA_VSTD::is_constructible, extents_type, _OtherExtents)
                   _CCCL_AND(extents_type::rank() <= 1))
  __MDSPAN_CONDITIONAL_EXPLICIT((!_CUDA_VSTD::is_convertible<_OtherExtents, extents_type>::value)) // needs two () due
                                                                                                   // to comma
  _LIBCUDACXX_HIDE_FROM_ABI constexpr mapping(
    layout_left::mapping<_OtherExtents> const& __other) noexcept // NOLINT(google-explicit-constructor)
      : __extents(__other.extents())
  {
    /*
     * TODO: check precondition
     * __other.required_span_size() is a representable value of type index_type
     */
  }

  _CCCL_TEMPLATE(class _OtherExtents)
  _CCCL_REQUIRES(_CCCL_TRAIT(_CUDA_VSTD::is_constructible, extents_type, _OtherExtents))
  __MDSPAN_CONDITIONAL_EXPLICIT((extents_type::rank() > 0))
  _LIBCUDACXX_HIDE_FROM_ABI constexpr mapping(
    layout_stride::mapping<_OtherExtents> const& __other) // NOLINT(google-explicit-constructor)
      : __extents(__other.extents())
  {
    /*
     * TODO: check precondition
     * __other.required_span_size() is a representable value of type index_type
     */
    NV_IF_TARGET(NV_IS_HOST, (size_t __stride = 1; for (rank_type __r = __extents.rank(); __r > 0; __r--) {
                   _LIBCUDACXX_THROW_RUNTIME_ERROR(__stride == static_cast<size_t>(__other.stride(__r - 1)),
                                                   "Assigning layout_stride to layout_right with invalid strides.");
                   __stride *= __extents.extent(__r - 1);
                 }))
  }

  _CCCL_HIDE_FROM_ABI __MDSPAN_CONSTEXPR_14_DEFAULTED mapping& operator=(mapping const&) noexcept = default;

  _LIBCUDACXX_HIDE_FROM_ABI constexpr const extents_type& extents() const noexcept
  {
    return __extents;
  }

  _LIBCUDACXX_HIDE_FROM_ABI constexpr index_type required_span_size() const noexcept
  {
    index_type __value = 1;
    for (rank_type __r = 0; __r != extents_type::rank(); ++__r)
    {
      __value *= __extents.extent(__r);
    }
    return __value;
  }

  //--------------------------------------------------------------------------------

  _CCCL_TEMPLATE(class... _Indices)
  _CCCL_REQUIRES((sizeof...(_Indices) == extents_type::rank())
                   _CCCL_AND __fold_and_v<_CCCL_TRAIT(_CUDA_VSTD::is_convertible, _Indices, index_type)...> //
                     _CCCL_AND __fold_and_v<_CCCL_TRAIT(_CUDA_VSTD::is_nothrow_constructible, index_type, _Indices)...>)
  _CCCL_HOST_DEVICE constexpr index_type operator()(_Indices... __idxs) const noexcept
  {
    return __compute_offset(__rank_count<0, extents_type::rank()>(), static_cast<index_type>(__idxs)...);
  }

  _LIBCUDACXX_HIDE_FROM_ABI static constexpr bool is_always_unique() noexcept
  {
    return true;
  }
  _LIBCUDACXX_HIDE_FROM_ABI static constexpr bool is_always_exhaustive() noexcept
  {
    return true;
  }
  _LIBCUDACXX_HIDE_FROM_ABI static constexpr bool is_always_strided() noexcept
  {
    return true;
  }
  _LIBCUDACXX_HIDE_FROM_ABI constexpr bool is_unique() const noexcept
  {
    return true;
  }
  _LIBCUDACXX_HIDE_FROM_ABI constexpr bool is_exhaustive() const noexcept
  {
    return true;
  }
  _LIBCUDACXX_HIDE_FROM_ABI constexpr bool is_strided() const noexcept
  {
    return true;
  }

  _CCCL_TEMPLATE(class _Ext = _Extents)
  _CCCL_REQUIRES((_Ext::rank() > 0))
  _LIBCUDACXX_HIDE_FROM_ABI constexpr index_type stride(rank_type __i) const noexcept
  {
    index_type __value = 1;
    for (rank_type __r = extents_type::rank() - 1; __r > __i; __r--)
    {
      __value *= __extents.extent(__r);
    }
    return __value;
  }

  template <class _OtherExtents>
  _LIBCUDACXX_HIDE_FROM_ABI friend constexpr bool
  operator==(mapping const& __lhs, mapping<_OtherExtents> const& __rhs) noexcept
  {
    return __lhs.extents() == __rhs.extents();
  }

  // In C++ 20 the not equal exists if equal is found
#if !__MDSPAN_HAS_CXX_20
  template <class _OtherExtents>
  _LIBCUDACXX_HIDE_FROM_ABI friend constexpr bool
  operator!=(mapping const& __lhs, mapping<_OtherExtents> const& __rhs) noexcept
  {
    return __lhs.extents() != __rhs.extents();
  }
#endif // !__MDSPAN_HAS_CXX_20

  // Not really public, but currently needed to implement fully constexpr usable submdspan:
  template <size_t _Np, class _SizeType, size_t... _Ep, size_t... _Idx>
  _CCCL_HOST_DEVICE constexpr index_type
  __get_stride(_CUDA_VSTD::extents<_SizeType, _Ep...>, _CUDA_VSTD::integer_sequence<size_t, _Idx...>) const
  {
    return __MDSPAN_FOLD_TIMES_RIGHT((_Idx > _Np ? __extents.template __extent<_Idx>() : 1), 1);
  }
  template <size_t _Np>
  _CCCL_HOST_DEVICE constexpr index_type __stride() const noexcept
  {
    return __get_stride<_Np>(__extents, _CUDA_VSTD::make_index_sequence<extents_type::rank()>());
  }

private:
  _CCCL_NO_UNIQUE_ADDRESS extents_type __extents{};
};

_LIBCUDACXX_END_NAMESPACE_STD

#endif // _LIBCUDACXX___MDSPAN_LAYOUT_RIGHT_HPP
