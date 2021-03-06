/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright (c) 2014-2016 Adam Getchell
///
/// Ensures that the S3 bulk action calculations are correct, and give
/// similar results for similar values.

/// @file S3ActionTest.cpp
/// @brief Tests for the S3 action functions
/// @author Adam Getchell
/// @bug <a href="http://clang-analyzer.llvm.org/scan-build.html">
/// scan-build</a>: No bugs found.

// clang-format off
#include <algorithm>
#include <vector>

#include "Utilities.h"
#include "S3Action.h"
#include "S3Triangulation.h"
#include "SimplicialManifold.h"
#include "gmock/gmock.h"
// clang-format on

class S3ActionTest : public ::testing::Test
{
 protected:
  S3ActionTest()
      : universe_{make_triangulation(6400, 7)}
      , attempted_moves_{}
      , N3_31_before{universe_.geometry->N3_31()}
      , N3_22_before{universe_.geometry->N3_22()}
      , N3_13_before{universe_.geometry->N3_13()}
      , timelike_edges_before{universe_.geometry->N1_TL()}
      , spacelike_edges_before{universe_.geometry->N1_SL()}
      , vertices_before{universe_.geometry->N0()}
  {
  }

  virtual void SetUp()
  {
    // Print ctor-initialized values
    std::cout << "(3,1) simplices: " << universe_.geometry->N3_31()
              << std::endl;
    std::cout << "(2,2) simplices: " << universe_.geometry->N3_22()
              << std::endl;
    std::cout << "(1,3) simplices: " << universe_.geometry->N3_13()
              << std::endl;
    std::cout << "Timelike edges: " << universe_.geometry->N1_TL() << std::endl;
    std::cout << "Spacelike edges: " << universe_.geometry->N1_SL()
              << std::endl;
    std::cout << "Vertices: " << universe_.geometry->N0() << std::endl;
  }

  /// @brief Simplicial manifold containing pointer to triangulation
  /// and geometric information.
  SimplicialManifold universe_;

  /// @brief A count of all attempted moves.
  Move_tracker attempted_moves_;

  /// @brief Initial number of (3,1) simplices
  std::intmax_t N3_31_before;

  /// @brief Initial number of (2,2) simplices
  std::intmax_t N3_22_before;

  /// @brief Initial number of (1,3) simplices
  std::intmax_t N3_13_before;

  /// @brief Initial number of timelike edges
  std::intmax_t timelike_edges_before;

  /// @brief Initial number of spacelike edges
  std::intmax_t spacelike_edges_before;

  /// @brief Initial number of vertices
  std::intmax_t vertices_before;

  /// @brief K value
  static constexpr long double K = static_cast<long double>(1.1);

  /// @brief Lambda value
  static constexpr auto Lambda = static_cast<long double>(0.1);
};

TEST_F(S3ActionTest, GetN3Values)
{
  ASSERT_EQ(universe_.triangulation->number_of_finite_cells(),
            N3_31_before + N3_22_before + N3_13_before)
      << "N3(3,1) + N3(2,2) + N3(1,3) should be total number of cells.";
}

TEST_F(S3ActionTest, GetN1Values)
{
  ASSERT_EQ(universe_.triangulation->number_of_finite_edges(),
            timelike_edges_before + spacelike_edges_before)
      << "timelike_edges_before + spacelike_edges_before should be total "
         "number of edges.";
}

TEST_F(S3ActionTest, CalculateAlphaMinus1BulkAction)
{
  auto Bulk_action = S3_bulk_action_alpha_minus_one(
      timelike_edges_before, universe_.geometry->N3_31_13(),
      universe_.geometry->N3_22(), K, Lambda);
  // Debugging
  //  std::cout << "timelike_edges_before " << timelike_edges_before <<
  //  std::endl;
  //  std::cout << "universe_.geometry->N3_31_13()"
  //            << universe_.geometry->N3_31_13() << std::endl;
  //  std::cout << "universe_.geometry->N3_22() " << universe_.geometry->N3_22()
  //            << std::endl;
  //  std::cout << "K " << K << std::endl;
  //  std::cout << "Lambda " << Lambda << std::endl;
  std::cout << "S3_bulk_action_alpha_minus_one() result is " << Bulk_action
            << std::endl;

  // Magic values from lots of tests
  EXPECT_TRUE(IsBetween<Gmpzf>(Bulk_action, 3000, 4500))
      << "S3_bulk_action_minus_one() out of expected range";
}

TEST_F(S3ActionTest, CalculateAlpha1BulkAction)
{
  auto Bulk_action = S3_bulk_action_alpha_one(
      timelike_edges_before, universe_.geometry->N3_31_13(),
      universe_.geometry->N3_22(), K, Lambda);
  std::cout << "S3_bulk_action_alpha_one() result is " << Bulk_action
            << std::endl;

  // Magic values from lots of tests
  EXPECT_TRUE(IsBetween<Gmpzf>(Bulk_action, 1000, 3000))
      << "S3_bulk_action_alpha_one() out of expected range.";
}

TEST_F(S3ActionTest, CalculateGeneralBulkAction)
{
  constexpr auto Alpha = static_cast<long double>(0.6);
  std::cout << "(Long double) Alpha = " << Alpha << std::endl;
  auto Bulk_action =
      S3_bulk_action(timelike_edges_before, universe_.geometry->N3_31_13(),
                     universe_.geometry->N3_22(), Alpha, K, Lambda);
  std::cout << "S3_bulk_action() result is " << Bulk_action << std::endl;

  // Magic value from lots of tests
  EXPECT_TRUE(IsBetween<Gmpzf>(Bulk_action, 1000, 4000))
      << "S3_bulk_action() out of expected range.";
}

TEST_F(S3ActionTest, GeneralBulkActionEquivalentToAlpha1BulkAction)
{
  constexpr auto tolerance = static_cast<long double>(0.05);
  constexpr auto Alpha     = static_cast<long double>(1.0);
  std::cout << "(Long double) Alpha = " << Alpha << std::endl;

  auto Bulk_action =
      S3_bulk_action(timelike_edges_before, universe_.geometry->N3_31_13(),
                     universe_.geometry->N3_22(), Alpha, K, Lambda);
  auto Bulk_action_one = S3_bulk_action_alpha_one(
      timelike_edges_before, universe_.geometry->N3_31_13(),
      universe_.geometry->N3_22(), K, Lambda);
  std::cout << "S3_bulk_action() result is " << Bulk_action << std::endl;
  std::cout << "S3_bulk_action_alpha_one() result is " << Bulk_action_one
            << std::endl;
  std::cout << (1.0 - tolerance) << std::endl;
  // BUG: For some reason this produces 0
  const auto min = Bulk_action_one * (1.0 - tolerance);
  std::cout << "(double) min = " << min << std::endl;
  std::cout << (1.0 + tolerance) << std::endl;
  const auto max = Bulk_action_one * (1.0 + tolerance);
  std::cout << "(double) max = " << max << std::endl;

  ASSERT_TRUE(IsBetween<double>(Bulk_action, min, max))
      << "General Bulk action does not match Bulk action for alpha=1.";
}
