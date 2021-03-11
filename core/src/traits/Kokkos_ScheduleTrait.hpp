/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 3.0
//       Copyright (2020) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
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
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
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

#ifndef KOKKOS_KOKKOS_SCHEDULETRAIT_HPP
#define KOKKOS_KOKKOS_SCHEDULETRAIT_HPP

#include <Kokkos_Macros.hpp>
#include <Kokkos_Concepts.hpp>  // is_schedule_type, Schedule
#include <traits/Kokkos_PolicyTraitAdaptor.hpp>
#include <traits/Kokkos_Traits_fwd.hpp>

namespace Kokkos {

namespace Impl {

//==============================================================================
// <editor-fold desc="trait specification"> {{{1

template <class T>
struct show_extra_schedule_type_erroneously_given_to_execution_policy;
template <>
struct show_extra_schedule_type_erroneously_given_to_execution_policy<void> {};
struct ScheduleTrait : TraitSpecificationBase<ScheduleTrait> {
  // MSVC workaround for linearizing base classes (see Impl::linearize_bases)
  template <template <class> class GetBase, class... OtherTraits>
  struct base_traits : linearize_bases<GetBase, OtherTraits...> {
    static constexpr auto schedule_type_is_defaulted = true;

    using schedule_type = Schedule<Static>;
  };
  template <class Sched, class AnalyzeNextTrait>
  struct mixin_matching_trait : AnalyzeNextTrait {
    using base_t = AnalyzeNextTrait;
    using base_t::base_t;
    using schedule_type = Sched;
    static constexpr auto show_schedule_type_error_in_compilation_message =
        show_extra_schedule_type_erroneously_given_to_execution_policy<
            std::conditional_t<base_t::schedule_type_is_defaulted, void,
                               typename base_t::schedule_type>>{};
    static_assert(base_t::schedule_type_is_defaulted,
                  "Kokkos Error: More than one schedule type given. Search "
                  "compiler output for 'show_extra_schedule_type' to see the "
                  "type of the errant tag.");
    static constexpr bool schedule_type_is_defaulted = false;
  };
  // template <class T>
  // using trait_matches_specification = is_schedule_type<T>;
};

// </editor-fold> end trait specification }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="PolicyTraitMatcher specialization"> {{{1

template <class Sched>
struct PolicyTraitMatcher<ScheduleTrait, Schedule<Sched>> : std::true_type {};

// </editor-fold> end PolicyTraitMatcher specialization }}}1
//==============================================================================

}  // end namespace Impl

namespace Experimental {

//==============================================================================
// <editor-fold desc="User interface"> {{{1

template <class Policy, class ScheduleType>
constexpr auto require(Policy const& p, Kokkos::Schedule<ScheduleType>) {
  static_assert(Kokkos::is_execution_policy<Policy>::value, "");
  using new_policy_t = Kokkos::Impl::ScheduleTrait::policy_with_trait<
      Policy, Kokkos::Schedule<ScheduleType>>;
  return new_policy_t{p};
}

// </editor-fold> end User interface }}}1
//==============================================================================

}  // end namespace Experimental

}  // end namespace Kokkos

#endif  // KOKKOS_KOKKOS_SCHEDULETRAIT_HPP
