#include <mdpdf/infrastructure/cache/lru_page_cache.hpp>

#include <gtest/gtest.h>

using namespace mdpdf;

TEST(LruPageCacheTest, EvictsOldestWhenOverCapacity) {
  infra::LruPageCache cache(2);
  infra::CacheKey k1{1, 0, 1000, 1000};
  infra::CacheKey k2{1, 1, 1000, 1000};
  infra::CacheKey k3{1, 2, 1000, 1000};

  core::PageBitmap bmp;
  bmp.width = 10;
  bmp.height = 10;
  bmp.stride = 40;
  bmp.bgra.assign(400, 0);

  cache.put(k1, bmp);
  cache.put(k2, bmp);
  EXPECT_TRUE(cache.get(k1).has_value());
  cache.put(k3, bmp);
  EXPECT_FALSE(cache.get(k1).has_value());
  EXPECT_TRUE(cache.get(k2).has_value());
  EXPECT_TRUE(cache.get(k3).has_value());
}
