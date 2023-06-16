#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <numa.h>

#include "site_ycsb_db_KvssdUnvme.h"
#include "kv_apis.h"

static int      nr_device = 0;
static uint64_t arr_handle[NR_MAX_SSD] = {0,};

enum NVME_CMD_KV {
    NVME_CMD_KV_STORE = 0x81,
};

JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_init(JNIEnv *env, jobject obj, jstring config_path)
{
    int ret = kv_sdk_init(KV_SDK_INIT_FROM_JSON,
        (char *)env->GetStringUTFChars(config_path, 0));
    if (ret != KV_SUCCESS)
        return ret;
    return kv_get_device_handles(&nr_device, arr_handle);
}

JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_destroy(JNIEnv *env, jobject obj)
{
    return kv_sdk_finalize();
}

JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_insert(JNIEnv *env, jobject obj, jint dev_idx, jstring key, jstring value)
{
    kv_pair kv = {0,};
    kv.key.key = (char *)env->GetStringUTFChars(key, 0);
    kv.key.length = strlen((char *)kv.key.key);
    kv.value.value = (char *)env->GetStringUTFChars(value, 0);
    kv.value.length = strlen((char *)kv.value.value);

    if (dev_idx >= nr_device)
        return KV_ERR_INVALID_OPTION;
    return kv_store(arr_handle[dev_idx], &kv);
}

JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_scan(JNIEnv *env, jobject obj, jint dev_idx, jint prefix, jint to_read_count)
{
    const uint8_t keyspace = KV_KEYSPACE_IODATA;
    const uint32_t bitmask = 0xFFFFFFFF;
    uint32_t iter_handle, read_count;
    kv_iterate iter = {0,};
    int ret = 0;

    if (dev_idx >= nr_device)
        return KV_ERR_INVALID_OPTION;

    iter_handle = kv_iterate_open(arr_handle[dev_idx], keyspace, bitmask, prefix, KV_KEY_ITERATE);
    if (iter_handle > KV_MAX_ITERATE_HANDLE || iter_handle <= 0) {
        return KV_ERR_ITERATE_ERROR;
    }

    // 32KB.
    iter.kv.value.value = malloc(KV_ITERATE_READ_BUFFER_SIZE);
    while (to_read_count > 0) {
        iter.iterator = iter_handle;
        memset(&iter.kv.param, 0, sizeof(kv_param));
        iter.kv.value.length = KV_ITERATE_READ_BUFFER_SIZE;
        memset(iter.kv.value.value, 0, KV_ITERATE_READ_BUFFER_SIZE);

        ret = kv_iterate_read(arr_handle[dev_idx], &iter);
        if (ret == KV_ERR_ITERATE_READ_EOF) {
            ret = KV_SUCCESS;
            break;
        }
        read_count = *((uint32_t *)iter.kv.value.value);
        to_read_count -= read_count;
    }
    free(iter.kv.value.value);

    if (ret == KV_SUCCESS)
        ret = kv_iterate_close(arr_handle[dev_idx], iter.iterator);
    else
        kv_iterate_close(arr_handle[dev_idx], iter.iterator);

    return ret;
}

