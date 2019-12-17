#include "helpers.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;

TEST(CreatePeerId, CreatePeerId) {
    string id1 = r5::CreatePeerId();
    string id2 = r5::CreatePeerId();
    string id3 = r5::CreatePeerId();
    EXPECT_NE(id1, id2);
    EXPECT_NE(id1, id3);
    EXPECT_NE(id2, id3);
}
