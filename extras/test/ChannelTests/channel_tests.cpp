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

#include <supla/channel.h>

TEST(ChannelTests, ChannelsAreAddedToStaticList) {
  EXPECT_EQ(Supla::Channel::begin(), nullptr); 
  EXPECT_EQ(Supla::Channel::begin(), Supla::Channel::last());
  EXPECT_EQ(Supla::Channel::size(), 0);

  {
    Supla::Channel firstChannel;
    EXPECT_EQ(Supla::Channel::size(), 1);

    Supla::Channel secondChannel;
    {
      Supla::Channel thirdChannel;
      EXPECT_EQ(Supla::Channel::size(), 3);
      EXPECT_EQ(Supla::Channel::begin(), &firstChannel);
      EXPECT_EQ(Supla::Channel::last(), &thirdChannel);
    }
    EXPECT_EQ(Supla::Channel::size(), 2);
    EXPECT_EQ(Supla::Channel::begin(), &firstChannel);
    EXPECT_EQ(Supla::Channel::last(), &secondChannel);
  }

  EXPECT_EQ(Supla::Channel::size(), 0);
  EXPECT_EQ(Supla::Channel::begin(), nullptr); 
  EXPECT_EQ(Supla::Channel::begin(), Supla::Channel::last());
}


TEST(ChannelTests, LastCommunicationTime) {
  EXPECT_EQ(Supla::Channel::size(), 0);
  EXPECT_EQ(Supla::Channel::lastCommunicationTimeMs, 0);
}

TEST(ChannelTests, ChannelMethods) {
  Supla::Channel first;
  Supla::Channel second;

  EXPECT_EQ(first.getChannelNumber(), 0);
  EXPECT_EQ(second.getChannelNumber(), 1);

  EXPECT_EQ(first.isExtended(), false);
  EXPECT_EQ(first.isUpdateReady(), false);
  EXPECT_EQ(first.getChannelType(), 0);
  EXPECT_EQ(first.getExtValue(), nullptr);

}
