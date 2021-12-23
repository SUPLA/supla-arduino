/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <gtest/gtest.h>

#include <supla/condition.h>

TEST(OnEqualEqTests, OnLessEqConditionTests) {
  auto cond = OnLessEq(10);

  EXPECT_TRUE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(15));

  EXPECT_TRUE(cond->checkConditionFor(10));
  EXPECT_FALSE(cond->checkConditionFor(9.9999));

  // "On" conditions should fire actions only on transition to meet condition.
  EXPECT_FALSE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(5));

  // Going back above threshold value, should reset expectation and it should return
  // true on next call with met condition
  EXPECT_FALSE(cond->checkConditionFor(50));
  EXPECT_TRUE(cond->checkConditionFor(5));

}



