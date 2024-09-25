/*
 * libsvn_ra_local/ra_init.h:  Exported function of libsvn_ra_local
 *
 * ====================================================================
 *    Licensed to the Apache Software Foundation (ASF) under one
 *    or more contributor license agreements.  See the NOTICE file
 *    distributed with this work for additional information
 *    regarding copyright ownership.  The ASF licenses this file
 *    to you under the Apache License, Version 2.0 (the
 *    "License"); you may not use this file except in compliance
 *    with the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing,
 *    software distributed under the License is distributed on an
 *    "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *    KIND, either express or implied.  See the License for the
 *    specific language governing permissions and limitations
 *    under the License.
 * ====================================================================
 */

#ifndef SVN_LIBSVN_RA_LOCAL_INIT
#define SVN_LIBSVN_RA_LOCAL_INIT

#include "../libsvn_ra/ra_loader.h"

svn_error_t *svn_ra_local__init(const svn_version_t *loader_version,
                                const svn_ra__vtable_t **vtable,
                                apr_pool_t *pool);

/** Initialize libsvn_ra_local.
 *
 * @deprecated Provided for backward compatibility with the 1.1 API. */
svn_error_t *svn_ra_local__deprecated_init(int abi_version,
                                           apr_pool_t *pool,
                                           apr_hash_t *hash);

#endif
