/**
 * @copyright
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
 * @endcopyright
 *
 * @file org_apache_subversion_javahl_remote_RemoteFactory.cpp
 * @brief Implementation of the native methods in the Java class RemoteFactory
 */

#include "../include/org_apache_subversion_javahl_remote_RemoteFactory.h"

#include "JNIStackElement.h"
#include "JNIUtil.h"
#include "JNIStringHolder.h"

#include "RemoteSession.h"

#include "svn_private_config.h"

JNIEXPORT jobject JNICALL
Java_org_apache_subversion_javahl_remote_RemoteFactory_open(
    JNIEnv *env, jclass jclass, jstring jurl, jstring juuid,
    jstring jconfigDirectory, jstring jusername, jstring jpassword,
    jobject jprompter, jobject jprogress)
{
  //JNI macros need jthis but this is a static call
  jobject jthis = NULL;
  JNIEntry(Remotefactory, open);

  /*
   * Initialize ra layer if we have not done so yet
   */
  static bool initialized = false;
  if (!initialized)
    {
      SVN_JNI_ERR(svn_ra_initialize(JNIUtil::getPool()), NULL);
      initialized = true;
    }

  /*
   * Create RemoteSession C++ object and return its java wrapper to the caller
   */
  jobject jremoteSession = NULL;

  RemoteSession* session = new RemoteSession(
      &jremoteSession, jurl, juuid, jconfigDirectory,
      jusername, jpassword, jprompter, jprogress);
  if (JNIUtil::isJavaExceptionThrown())
    {
      delete session;
      return NULL;
    }

  return jremoteSession;
}
