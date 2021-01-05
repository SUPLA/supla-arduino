#include <gtest/gtest.h>
#include <supla/uptime.h>

TEST(UptimeTests, LastResetCauseSetAndGet) {
  Supla::Uptime uptime;
  EXPECT_EQ(uptime.getUptime(), 0);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  // setter should not work unless first resetConnectionUptime is called
  uptime.setConnectionLostCause(SUPLA_LASTCONNECTIONRESETCAUSE_WIFI_CONNECTION_LOST);
  EXPECT_EQ(uptime.getUptime(), 0);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  uptime.resetConnectionUptime();
  uptime.setConnectionLostCause(SUPLA_LASTCONNECTIONRESETCAUSE_WIFI_CONNECTION_LOST);
  EXPECT_EQ(uptime.getUptime(), 0);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_WIFI_CONNECTION_LOST);
}

TEST(UptimeTests, IterateShouldIncreaseUptimeCounters) {
  Supla::Uptime uptime;
  unsigned long millis = 0;

  EXPECT_EQ(uptime.getUptime(), 0);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  millis += 999;
  uptime.iterate(millis);
  EXPECT_EQ(uptime.getUptime(), 0);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  millis += 1500;
  uptime.iterate(millis);
  EXPECT_EQ(uptime.getUptime(), 2);
  EXPECT_EQ(uptime.getConnectionUptime(), 2);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  millis += 20000;
  uptime.iterate(millis);
  EXPECT_EQ(uptime.getUptime(), 22);
  EXPECT_EQ(uptime.getConnectionUptime(), 22);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  uptime.resetConnectionUptime();
  EXPECT_EQ(uptime.getUptime(), 22);
  EXPECT_EQ(uptime.getConnectionUptime(), 0);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);

  millis += 2500;
  uptime.iterate(millis);
  EXPECT_EQ(uptime.getUptime(), 24);
  EXPECT_EQ(uptime.getConnectionUptime(), 2);
  EXPECT_EQ(uptime.getLastResetCause(), SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN);
}

