/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
/**
 * @file        src/cyad/BaseAdminApiWrapper.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Wrapper around cynara-admin API (base)
 */

#ifndef SRC_CYAD_BASEADMINAPIWRAPPER_H_
#define SRC_CYAD_BASEADMINAPIWRAPPER_H_

struct cynara_admin;
struct cynara_admin_policy;

namespace Cynara {

class BaseAdminApiWrapper {
public:
    virtual ~BaseAdminApiWrapper() {};
    virtual int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin) = 0;
    virtual int cynara_admin_finish(struct cynara_admin *p_cynara_admin) = 0;
    virtual int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                                          const struct cynara_admin_policy *const *policies) = 0;
    virtual int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket,
                                        int operation, const char *extra) = 0;
    virtual int cynara_admin_check(struct cynara_admin *p_cynara_admin,
                                   const char *start_bucket, const int recursive,
                                   const char *client, const char *user, const char *privilege,
                                   int *result, char **result_extra) = 0;
    virtual int cynara_admin_list_policies(struct cynara_admin *p_cynara_admin, const char *bucket,
                                           const char *client, const char *user,
                                           const char *privilege,
                                           struct cynara_admin_policy ***policies) = 0;
    virtual int cynara_admin_erase(struct cynara_admin *p_cynara_admin, const char *start_bucket,
                                   int recursive, const char *client, const char *user,
                                   const char *privilege) = 0;
    virtual int cynara_admin_list_policies_descriptions(struct cynara_admin *p_cynara_admin,
                                              struct cynara_admin_policy_descr ***descriptions) = 0;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_BASEADMINAPIWRAPPER_H_ */
