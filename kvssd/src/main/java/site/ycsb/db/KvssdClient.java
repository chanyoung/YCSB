/**
 * Copyright (c) 2016 YCSB contributors. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License. You
 * may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License. See accompanying
 * LICENSE file.
 */

package site.ycsb.db;

import site.ycsb.ByteIterator;
import site.ycsb.DB;
import site.ycsb.DBException;
import site.ycsb.Status;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.Vector;

/**
 * YCSB binding for kvssd.
 */
public class KvssdClient extends DB {

  public static final String CONFIG_PATH_PROPERTY = "kvssd.path";
  public static final String CONFIG_PATH_PROPERTY_DEFAULT = "/etc/unvme_config.json";

  public static final String KEY_LEN_PROPERTY = "kvssd.keylen";
  public static final String KEY_LEN_PROPERTY_DEFAULT = "32";

  private boolean isInited = false;

  private int keyLen;
  private int valLen;

  private KvssdUnvme unvme;

  public void init() throws DBException {
    Properties props = getProperties();

    String confPath = props.getProperty(CONFIG_PATH_PROPERTY);
    if (confPath == null) {
      confPath = CONFIG_PATH_PROPERTY_DEFAULT;
    }

    String keyLenProp = props.getProperty(KEY_LEN_PROPERTY);
    if (keyLenProp == null) {
      keyLenProp = KEY_LEN_PROPERTY_DEFAULT;
    }
    keyLen = Integer.parseInt(keyLenProp);

    unvme = new KvssdUnvme();
    int ret = unvme.init(confPath);
    if (ret < 0) {
      throw new DBException("failed to initialize device");
    }

    isInited = true;
  }

  public void cleanup() throws DBException {
    if (isInited) {
      unvme.destroy();
      isInited = false;
    }
  }

  private String fixedSizeKey(String key) {
    // Drop the common ycsb prefix "user".
    key = key.substring(4, key.length());
    while (key.length() < keyLen) {
      key = key + key;
    }
    if (key.length() > keyLen) {
      key = key.substring(0, keyLen);
    }
    return key;
  }

  private Status store(String table, String key, Map<String, ByteIterator> values) {
    StringBuilder val = new StringBuilder();
    for (final Entry<String, ByteIterator> e : values.entrySet()) {
      val.append(e.getValue().toString());
    }
    key = fixedSizeKey(key);

    int ret = unvme.insert(0, key, val.toString());
    if (ret == 0) {
      return Status.OK;
    } else {
      return Status.ERROR;
    }
  }

  @Override
  public Status read(String table, String key, Set<String> fields, Map<String, ByteIterator> result) {
    return Status.NOT_IMPLEMENTED;
  }

  @Override
  public Status insert(String table, String key, Map<String, ByteIterator> values) {
    return store(table, key, values);
  }

  @Override
  public Status delete(String table, String key) {
    return Status.NOT_IMPLEMENTED;
  }

  @Override
  public Status update(String table, String key, Map<String, ByteIterator> values) {
    return store(table, key, values);
  }

  @Override
  public Status scan(String table, String startkey, int recordcount, Set<String> fields,
      Vector<HashMap<String, ByteIterator>> result) {
    startkey = fixedSizeKey(startkey);

    int prefix = 0;
    int prefixLen = startkey.length() > 4 ? 4 : startkey.length();
    for (int i = 0; i < prefixLen; i++) {
      prefix |= ((0xFF & startkey.charAt(i)) << ((3-i) * 8));
    }

    int ret = unvme.scan(0, prefix, recordcount);
    if (ret == 0) {
      return Status.OK;
    } else {
      return Status.ERROR;
    }
  }
}
