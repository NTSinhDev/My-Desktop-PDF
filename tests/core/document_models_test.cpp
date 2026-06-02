#include <mdpdf/core/domain/document_models.hpp>

#include <gtest/gtest.h>

TEST(DocumentModelsTest, DocumentIdStableForSamePath) {
  const auto id1 = mdpdf::core::make_document_id("C:/docs/sample.pdf");
  const auto id2 = mdpdf::core::make_document_id("C:/docs/sample.pdf");
  EXPECT_EQ(id1.value, id2.value);
}
