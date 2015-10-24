/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright (c) 2015 Adam Getchell
///
/// Performs the Metropolis-Hastings algorithm on the foliated Delaunay
/// triangulations.
/// For details see:
/// M. Creutz, and B. Freedman. “A Statistical Approach to Quantum Mechanics.”
/// Annals of Physics 132 (1981): 427–62.
/// http://thy.phy.bnl.gov/~creutz/mypubs/pub044.pdf

/// \done Initialization
/// \todo Implement 3D Metropolis algorithm
/// \todo Implement concurrency

/// @file MetropolisManager.h
/// @brief Perform Metropolis-Hasting algorithm on Delaunay Triangulations
/// @author Adam Getchell
/// @bug <a href="http://clang-analyzer.llvm.org/scan-build.html">
/// scan-build</a>: No bugs found.

#ifndef SRC_METROPOLIS_H_
#define SRC_METROPOLIS_H_

// CGAL headers
#include <CGAL/Gmpzf.h>
#include <CGAL/Gmpz.h>
#include <mpfr.h>

// CDT headers
#include "S3Triangulation.h"
#include "S3ErgodicMoves.h"
#include "Utilities.h"

// C++ headers
#include <vector>
#include <utility>
#include <tuple>

using Gmpzf = CGAL::Gmpzf;
using Gmpz = CGAL::Gmpz;
// using move_tuple = std::tuple<std::atomic<long int>,
                              // std::atomic<long int>,
                              // std::atomic<long int>>;
using move_tuple = std::tuple<unsigned long int, unsigned long int,
                              unsigned long int>;

extern unsigned PRECISION;

enum class move_type {TWO_THREE = 1,
                      THREE_TWO = 2,
                      TWO_SIX = 3};

// template <typename T1, typename T2>
// auto attempt_23_move(T1&& universe_ptr, T2&& simplex_types) noexcept
//                      -> decltype(universe_ptr) {
//   return universe_ptr;
// }  // attempt_23_move()

/// @class Metropolis
///
/// @brief Metropolis-Hastings algorithm functor
///
/// The Metropolis-Hastings algorithm is a Markov Chain Monte Carlo method.
/// The probability of making an ergodic (Pachner) move is:

class Metropolis {
 public:
  Metropolis(unsigned passes, unsigned output_every_n_passes)
    : passes_(passes), output_every_n_passes_(output_every_n_passes) {
#ifndef NDEBUG
    std::cout << "Ctor called." << std::endl;
#endif
  }

  template <typename T>
  auto operator()(T&& universe_ptr) -> decltype(universe_ptr) {
#ifndef NDEBUG
    std::cout << "operator() called." << std::endl;
#endif
    // Populate member data
    universe_ptr_ = std::move(universe_ptr);
    simplex_types_ = classify_simplices(universe_ptr_);
    edge_types_ = classify_edges(universe_ptr_);

    // Attempt each type of move to populate **attempted_moves_**
    universe_ptr_ = std::move(make_23_move(universe_ptr_,
                                           simplex_types_, attempted_moves_));


    return universe_ptr_;
  }
  /// Gets value of **passes_**.
  auto Passes() {return passes_;}
  /// Gets value of **output_every_n_passes_**.
  auto Output() {return output_every_n_passes_;}
  /// Gets the total number of attempted moves.
  auto TotalMoves() const {return std::get<0>(attempted_moves_) +
                                  std::get<1>(attempted_moves_) +
                                  std::get<2>(attempted_moves_);}
  /// Gets attempted (2,3) moves.
  auto TwoThreeMoves() const {return std::get<0>(attempted_moves_);}
  /// Gets the number of timelike edges.
  auto TimelikeEdges() const {return edge_types_.first;}
  /// Gets the number of (3,1) simplices.
  auto ThreeOne() const {return std::get<0>(simplex_types_);}
  /// Gets the number of (2,2) simplices.
  auto TwoTwo() const {return std::get<1>(simplex_types_);}
  /// Gets the number of (1,3) simplices.
  auto OneThree() const {return std::get<2>(simplex_types_);}
  /// Calculate the probability of making a move divided by the
  /// probability of its reverse.
  auto CalculateA1(move_type move) const {
    auto total_moves = this->TotalMoves();
    auto this_move = 0;
    switch (move) {
      case move_type::TWO_THREE:
        this_move = std::get<0>(attempted_moves_);
      case move_type::THREE_TWO:
        this_move = std::get<1>(attempted_moves_);
      case move_type::TWO_SIX:
        this_move = std::get<2>(attempted_moves_);
    }
    // Set precision for initialization and assignment functions
    mpfr_set_default_prec(PRECISION);

    // Initialize for MPFR
    mpfr_t r1, r2, a1;
    mpfr_inits2(PRECISION, r1, r2, a1, nullptr);

    mpfr_init_set_ui(r1, this_move, MPFR_RNDD);     // r1 = this_move
    mpfr_init_set_ui(r2, total_moves, MPFR_RNDD);   // r2 = total_moves

    // The result
    mpfr_div(a1, r1, r2, MPFR_RNDD);                // a1 = r1/r2

    // Debugging
    std::cout << "TotalMoves() = " << total_moves << std::endl;
    std::cout << "this_move = " << this_move << std::endl;
    std::cout << "A1 is " << mpfr_out_str(stdout, 10, 0, a1, MPFR_RNDD)
              << std::endl;

    // Convert mpfr_t total to Gmpzf result by using Gmpzf(double d)
    Gmpzf result = Gmpzf(mpfr_get_d(a1, MPFR_RNDD));

    // Free memory
    mpfr_clears(r1, r2, a1, nullptr);

    return result;
  }  // CalculateA1()

  template <typename T1, typename T2, typename T3>
  auto attempt_23_move(T1&& universe_ptr,
                       T2&& simplex_types,
                       T3&& attempted_moves)
                       noexcept -> decltype(universe_ptr) {
    // Calculate probability
    auto a1 = CalculateA1(move_type::TWO_THREE);
    // Make move if random number < probability
    return universe_ptr;
  }  // attempt_23_move()


 private:
  Delaunay universe;
  std::unique_ptr<decltype(universe)>
    universe_ptr_ = std::make_unique<decltype(universe)>(universe);
  ///< Pointer to the Delaunay triangulation.
  unsigned passes_;  ///< Number of passes of ergodic moves on triangulation.
  unsigned output_every_n_passes_;  ///< How often to print/write output.
  move_tuple attempted_moves_;
  ///< Attempted (2,3), (3,2), and (2,6) moves.
  std::tuple<std::vector<Cell_handle>,
             std::vector<Cell_handle>,
             std::vector<Cell_handle>> simplex_types_;
  ///< (3,1), (2,2) and (1,3) simplices.
  std::pair<std::vector<Edge_tuple>, unsigned> edge_types_;
  ///< Timelike and spacelike edges.
};




/// @brief Apply the Metropolis-Hastings algorithm
///
/// The Metropolis-Hastings algorithm is a Markov Chain Monte Carlo method.
/// The probability of making an ergodic (Pachner) move is:
///
/// @param[in] universe_ptr A std::unique_ptr to the Delaunay triangulation
/// @param[in] number_of_passes The number of passes made with MCMC, where a
/// pass is defined as a number of attempted moves equal to the current number
/// of simplices.
/// @param[in] output_every_n_passes Prints/saves to file the current
/// Delaunay triangulation every n passes.
/// @returns universe_ptr A std::unique_ptr to the Delaunay triangulation after
/// the move has been made
// template <typename T>
// auto metropolis(T&& universe_ptr, unsigned number_of_passes,
//                 unsigned output_every_n_passes) noexcept
//                 -> decltype(universe_ptr) {
//   std::cout << "Starting ..." << std::endl;
//
//   auto simplex_types = classify_simplices(universe_ptr);
//   auto edge_types = edge_types = classify_edges(universe_ptr);
//
//
//   auto attempted_moves_per_pass = universe_ptr->number_of_finite_cells();
//   // First, attempt a move of each type
//   // attempt_23_move();
//   ++std::get<0>(attempted_moves);
//   // attempt_32_move();
//   ++std::get<1>(attempted_moves);
//   // attempt_26_move();
//   ++std::get<2>(attempted_moves);
//   while (std::get<0>(attempted_moves) +
//          std::get<1>(attempted_moves) +
//          std::get<2>(attempted_moves) < attempted_moves_per_pass) {
//     // Fix this function to use attempt[i]/total attempts
//     auto move = generate_random_unsigned(1, 3);
//     std::cout << "Move #" << move << std::endl;
//
//     switch (move) {
//       case (move_type::TWO_THREE):
//         std::cout << "Move 1 (2,3) picked" << std::endl;
//         // attempt_23_move()
//         ++std::get<0>(attempted_moves);
//         break;
//       case (move_type::THREE_TWO):
//         std::cout << "Move 2 (3,2) picked" << std::endl;
//         // attempt_32_move()
//         ++std::get<1>(attempted_moves);
//         break;
//       case (move_type::TWO_SIX):
//         std::cout << "Move 3 (2,6) picked" << std::endl;
//         // attempt_26_move()
//         ++std::get<2>(attempted_moves);
//         break;
//       default:
//         std::cout << "Oops!" << std::endl;
//         break;
//     }
//   }
//   return universe_ptr;
// }  // metropolis()
// auto metropolis =
//   std::make_unique<decltype(universe)>(Metropolis(universe));
// std::unique_ptr<Metropolis> metropolis = std::make_unique<Metropolis>(universe);
// Metropolis metropolis(universe);

// // Main loop of program
// for (auto i = 0; i < passes; ++i) {
//   // Initialize data and data structures needed for ergodic moves
//   // each pass.
//   // make_23_move(&SphericalUniverse, &two_two) does the (2,3) move
//   // two_two is populated via classify_3_simplices()
//
//   // Get timelike edges V2 for make_32_move(&SphericalUniverse, &V2)
//   std::vector<Edge_tuple> V2;
//   auto N1_SL = static_cast<unsigned>(0);
//   get_timelike_edges(SphericalUniverse, &V2, &N1_SL);
//
//   auto moves_this_pass = SphericalUniverse.number_of_finite_cells();
//
//   std::cout << "Pass #" << i+1 << " is "
//             << moves_this_pass
//             << " attempted moves." << std::endl;
//
//   for (auto j = 0; j < moves_this_pass; ++j) {
//     // Metropolis algorithm to select moves goes here
//   }
// }

#endif  // SRC_METROPOLIS_H_
