/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class site_ycsb_db_KvssdUnvme */

#ifndef _Included_site_ycsb_db_KvssdUnvme
#define _Included_site_ycsb_db_KvssdUnvme
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     site_ycsb_db_KvssdUnvme
 * Method:    init
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_init
  (JNIEnv *, jobject, jstring);

/*
 * Class:     site_ycsb_db_KvssdUnvme
 * Method:    destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_destroy
  (JNIEnv *, jobject);

/*
 * Class:     site_ycsb_db_KvssdUnvme
 * Method:    insert
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_insert
  (JNIEnv *, jobject, jint, jstring, jstring);

/*
 * Class:     site_ycsb_db_KvssdUnvme
 * Method:    scan
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_site_ycsb_db_KvssdUnvme_scan
  (JNIEnv *, jobject, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
