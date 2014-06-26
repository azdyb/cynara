/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        load.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::StorageDeserializer
 */


#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <exceptions/BucketRecordCorruptedException.h>
#include <service/storage/StorageDeserializer.h>
#include <types/Policy.h>

#include "../../helpers.h"

#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

using namespace Cynara;

MATCHER_P(PolicyPtrEq, policy, "") {
    return std::tie(policy->key(), policy->result())
        == std::tie(arg->key(), arg->result());
}

class DeserializerFixture : public ::testing::Test {
public:
    virtual ~DeserializerFixture() = default;

    PolicyPtr createPolicy(const PolicyKey &pk, const PolicyResult &pr) {
        return std::make_shared<Policy>(pk, pr);
    }

    void checkCorruptedData(const std::string &data, const std::string &corruptedLine,
            size_t corruptedLineNumber) {
        std::istringstream ss(data);
        EXPECT_THROW(StorageDeserializer::loadPolicies(ss), BucketRecordCorruptedException);

        ss.seekg(0);
        try {
            StorageDeserializer::loadPolicies(ss);
        } catch (const BucketRecordCorruptedException &ex) {
            ASSERT_EQ(corruptedLine, ex.line());
            ASSERT_EQ(corruptedLineNumber, ex.lineNumber());
        }
    }
};

TEST_F(DeserializerFixture, load_empty) {
    using ::testing::IsEmpty;

    std::istringstream ss;

    auto policies = StorageDeserializer::loadPolicies(ss);
    ASSERT_THAT(policies, IsEmpty());
}

TEST_F(DeserializerFixture, load_1) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss("c;u;p;0;meta");

    auto policies = StorageDeserializer::loadPolicies(ss);
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyPtrEq(expectedPolicy)));
}

TEST_F(DeserializerFixture, load_1_allow) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss("c;u;p;0xFFFF;meta");

    auto policies = StorageDeserializer::loadPolicies(ss);
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::ALLOW, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyPtrEq(expectedPolicy)));
}

TEST_F(DeserializerFixture, load_1_no_meta_sep) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss("c;u;p;0;");

    auto policies = StorageDeserializer::loadPolicies(ss);
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyPtrEq(expectedPolicy)));
}

TEST_F(DeserializerFixture, load_1_no_meta_no_sep) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss("c;u;p;0");

    auto policies = StorageDeserializer::loadPolicies(ss);
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyPtrEq(expectedPolicy)));
}

TEST_F(DeserializerFixture, load_2) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss("c;u;p;0;meta\n"
                          "c;u;p;0;meta\n");

    auto policies = StorageDeserializer::loadPolicies(ss);
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyPtrEq(expectedPolicy),
            PolicyPtrEq(expectedPolicy)));
}

TEST_F(DeserializerFixture, load_mixed) {
    using ::testing::UnorderedElementsAre;
    using ::testing::UnorderedElementsAreArray;

    std::istringstream ss("c1;u1;p1;0;meta\n"
                          "c2;u2;p2;0xFFFF;meta2\n"
                          "c2;u2;p2;0xFFFF;\n"
                          "c1;u1;p3;0xFFFE;bucket\n");

    auto policies = StorageDeserializer::loadPolicies(ss);
    PolicyCollection expectedPolices = {
        createPolicy({ "c1", "u1", "p1" }, { PredefinedPolicyType::DENY, "meta" }),
        createPolicy({ "c2", "u2", "p2" }, { PredefinedPolicyType::ALLOW, "meta2" }),
        createPolicy({ "c2", "u2", "p2" }, { PredefinedPolicyType::ALLOW, "" }),
        createPolicy({ "c1", "u1", "p3" }, { PredefinedPolicyType::BUCKET, "bucket" })
    };

    // How to do it more elegantly?
    ASSERT_THAT(policies, UnorderedElementsAre(
        PolicyPtrEq(expectedPolices.at(0)),
        PolicyPtrEq(expectedPolices.at(1)),
        PolicyPtrEq(expectedPolices.at(2)),
        PolicyPtrEq(expectedPolices.at(3))
    ));
}

TEST_F(DeserializerFixture, load_no_client) {
    auto data = "u;p;0;meta";
    checkCorruptedData(data, data, 1);
}

TEST_F(DeserializerFixture, load_no_type) {
    auto data = "c;u;p;meta";
    checkCorruptedData(data, data, 1);
}

TEST_F(DeserializerFixture, load_all_missing) {
    auto data = ";;;";
    checkCorruptedData(data, data, 1);
}

TEST_F(DeserializerFixture, load_invalid_type) {
    auto data = "c;u;p;X";
    checkCorruptedData(data, data, 1);
}

TEST_F(DeserializerFixture, load_invalid_multiline) {
    auto data = "c1;u1;p1;0;meta\n"
                "c;u;p;X\n"
                "c1;u1;p1;0;meta\n";
    checkCorruptedData(data, "c;u;p;X", 2);
}
