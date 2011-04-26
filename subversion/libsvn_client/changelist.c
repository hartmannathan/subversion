/*
 * changelist.c:  implementation of the 'changelist' command
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

/* ==================================================================== */



/*** Includes. ***/

#include "svn_client.h"
#include "svn_wc.h"
#include "svn_pools.h"
#include "svn_dirent_uri.h"
#include "svn_path.h"
#include "svn_hash.h"

#include "client.h"
#include "private/svn_wc_private.h"
#include "svn_private_config.h"




svn_error_t *
svn_client_add_to_changelist(const apr_array_header_t *paths,
                             const char *changelist,
                             svn_depth_t depth,
                             const apr_array_header_t *changelists,
                             svn_client_ctx_t *ctx,
                             apr_pool_t *pool)
{
  apr_pool_t *iterpool = svn_pool_create(pool);
  int i;

  if (changelist[0] == '\0')
    return svn_error_create(SVN_ERR_BAD_CHANGELIST_NAME, NULL,
                            _("Target changelist name must not be empty"));

  for (i = 0; i < paths->nelts; i++)
    {
      const char *path = APR_ARRAY_IDX(paths, i, const char *);

      if (svn_path_is_url(path))
        return svn_error_createf(SVN_ERR_ILLEGAL_TARGET, NULL,
                                 _("'%s' is not a local path"), path);
    }

  for (i = 0; i < paths->nelts; i++)
    {
      const char *path = APR_ARRAY_IDX(paths, i, const char *);
      const char *local_abspath;

      svn_pool_clear(iterpool);
      SVN_ERR(svn_dirent_get_absolute(&local_abspath, path, iterpool));

      SVN_ERR(svn_wc_set_changelist2(ctx->wc_ctx, local_abspath, changelist,
                                     depth, changelists,
                                     ctx->cancel_func, ctx->cancel_baton,
                                     ctx->notify_func2, ctx->notify_baton2,
                                     iterpool));
    }

  svn_pool_destroy(iterpool);
  return SVN_NO_ERROR;
}


svn_error_t *
svn_client_remove_from_changelists(const apr_array_header_t *paths,
                                   svn_depth_t depth,
                                   const apr_array_header_t *changelists,
                                   svn_client_ctx_t *ctx,
                                   apr_pool_t *pool)
{
  apr_pool_t *iterpool = svn_pool_create(pool);
  int i;

  for (i = 0; i < paths->nelts; i++)
    {
      const char *path = APR_ARRAY_IDX(paths, i, const char *);

      if (svn_path_is_url(path))
        return svn_error_createf(SVN_ERR_ILLEGAL_TARGET, NULL,
                                 _("'%s' is not a local path"), path);
    }

  for (i = 0; i < paths->nelts; i++)
    {
      const char *path = APR_ARRAY_IDX(paths, i, const char *);
      const char *local_abspath;

      svn_pool_clear(iterpool);
      SVN_ERR(svn_dirent_get_absolute(&local_abspath, path, iterpool));

      SVN_ERR(svn_wc_set_changelist2(ctx->wc_ctx, local_abspath, NULL,
                                     depth, changelists,
                                     ctx->cancel_func, ctx->cancel_baton,
                                     ctx->notify_func2, ctx->notify_baton2,
                                     iterpool));
    }

  svn_pool_destroy(iterpool);
  return SVN_NO_ERROR;
}



/* Entry-walker callback for svn_client_get_changelist() below. */
struct get_cl_fn_baton
{
  svn_changelist_receiver_t callback_func;
  void *callback_baton;
  svn_wc_context_t *wc_ctx;
  apr_pool_t *pool;
};


static svn_error_t *
get_node_changelist(const char *local_abspath,
                    svn_node_kind_t kind,
                    void *baton,
                    apr_pool_t *pool)
{
  struct get_cl_fn_baton *b = (struct get_cl_fn_baton *)baton;
  const char *changelist;

  SVN_ERR(svn_wc__node_get_changelist(&changelist, b->wc_ctx,
                                      local_abspath, pool, pool));

  if (((kind == svn_node_file) || (kind == svn_node_dir)))
    {

      /* ...then call the callback function. */
      SVN_ERR(b->callback_func(b->callback_baton, local_abspath,
                               changelist, pool));
    }

  return SVN_NO_ERROR;
}


svn_error_t *
svn_client_get_changelists(const char *path,
                           const apr_array_header_t *changelists,
                           svn_depth_t depth,
                           svn_changelist_receiver_t callback_func,
                           void *callback_baton,
                           svn_client_ctx_t *ctx,
                           apr_pool_t *pool)
{
  struct get_cl_fn_baton gnb;
  const char *local_abspath;

  SVN_ERR(svn_dirent_get_absolute(&local_abspath, path, pool));

  gnb.callback_func = callback_func;
  gnb.callback_baton = callback_baton;
  gnb.wc_ctx = ctx->wc_ctx;
  gnb.pool = pool;

  return svn_error_return(
    svn_wc__node_walk_children(ctx->wc_ctx, local_abspath, FALSE, changelists,
                               get_node_changelist, &gnb, depth,
                               ctx->cancel_func, ctx->cancel_baton, pool));
}
