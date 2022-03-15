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
#include <supla/tools.h>

TEST(ToolsTests, isArrayEmptyTests) {
  char buf[10] = {};
  char buf2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  char buf3[10] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char buf4[1000] = {2};
  char buf5[1000] = {};

  EXPECT_TRUE(isArrayEmpty(buf, 10));
  buf[4] = 3;
  EXPECT_FALSE(isArrayEmpty(buf, 10));

  EXPECT_FALSE(isArrayEmpty(buf2, 10));
  EXPECT_TRUE(isArrayEmpty(buf2, 9));

  EXPECT_FALSE(isArrayEmpty(buf3, 10));
  EXPECT_FALSE(isArrayEmpty(buf3, 9));

  EXPECT_FALSE(isArrayEmpty(buf4, 1000));
  EXPECT_TRUE(isArrayEmpty(buf5, 1000));
}


