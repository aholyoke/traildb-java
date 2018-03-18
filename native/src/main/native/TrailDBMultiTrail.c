#include <traildb.h>
#include <string.h>
#include "traildb-java.h"
#include "traildb-java-static.h"

jfieldID FID_traildb_TrailDBMultiTrail_fields;

jfieldID FID_traildb_TrailDBTrail_fields;

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    init
 * Signature: ([Ltraildb/TrailDBTrail;)V
 */
JNIEXPORT void JNICALL Java_traildb_TrailDBMultiTrail_init(JNIEnv *env, jobject obj, jobjectArray trails) {
	jobject trail_obj;
	TrailDBMultiTrailFields *fields = malloc(sizeof(TrailDBMultiTrailFields));
	TrailDBTrailFields *trail_fields;

	tdb_multi_cursor *multi_cur;
	tdb_cursor **tgt_cursors;

	int num_cursors = (*env)->GetArrayLength(env, trails);

	tgt_cursors = malloc(num_cursors * sizeof(tdb_cursor *));

	for (int i = 0; i < num_cursors; i++) {
		trail_obj = (*env)->GetObjectArrayElement(env, trails, i);
		trail_fields = (TrailDBTrailFields *) (*env)->GetLongField(env, trail_obj, FID_traildb_TrailDBTrail_fields);
		tgt_cursors[i] = (tdb_cursor *) trail_fields->cur;
	}

	multi_cur = tdb_multi_cursor_new(tgt_cursors, num_cursors);

	// Initialize items to NULL because we haven't called next yet

	fields->multi_cur = (long) multi_cur;
	fields->items = 0L;

	(*env)->SetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields, (long) fields);
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    native_getItem
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_traildb_TrailDBMultiTrail_getItem(JNIEnv *env, jobject obj, jint index) {
	jclass exc;

	const tdb *db;
	const tdb_item *items;
	const char *value;
	char *tgt_value;
	uint64_t value_length;

	TrailDBMultiTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBMultiTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields);

	items = (tdb_item *) fields->items;

	if (items == 0L) {
		exc = (*env)->FindClass(env, "java/lang/IllegalStateException");
		if (exc == NULL) {
			/* Could not find the exception - We are in so much trouble right now */
			exit(1);
		}
		(*env)->ThrowNew(env, exc, "Cursor is not pointing at an event");
		return NULL;
	}

	if (index >= fields->num_items || index < 0) {
		exc = (*env)->FindClass(env, "java/lang/IndexOutOfBoundsException");
		if (exc == NULL) {
			/* Could not find the exception - We are in so much trouble right now */
			exit(1);
		}
		(*env)->ThrowNew(env, exc, "getItem out of bounds");
		return NULL;
	}

	db = (tdb *) fields->db;

	// Get the value of the item

	value = tdb_get_item_value(db, items[index], &value_length);

	// Convert buffer to null-terminated string

	tgt_value = malloc(value_length * sizeof(char) + 1);

	strncpy(tgt_value, value, value_length);

	tgt_value[value_length] = '\0';

	return (*env)->NewStringUTF(env, tgt_value);
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    getNumItems
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_traildb_TrailDBMultiTrail_getNumItems(JNIEnv *env, jobject obj) {
	jclass exc;

	TrailDBMultiTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBMultiTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields);

	if (fields->items == 0L) {
		exc = (*env)->FindClass(env, "java/lang/IllegalStateException");
		if (exc == NULL) {
			/* Could not find the exception - We are in so much trouble right now */
			exit(1);
		}
		(*env)->ThrowNew(env, exc, "Cursor is not pointing at an event");
		return 0;
	}

	return fields->num_items;
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    getTimestamp
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_traildb_TrailDBMultiTrail_getTimestamp(JNIEnv *env, jobject obj) {
	jclass exc;

	TrailDBMultiTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBMultiTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields);

	if (fields->items == 0L) {
		exc = (*env)->FindClass(env, "java/lang/IllegalStateException");
		if (exc == NULL) {
			/* Could not find the exception - We are in so much trouble right now */
			exit(1);
		}
		(*env)->ThrowNew(env, exc, "Cursor is not pointing at an event");
		return 0;
	}

	return fields->timestamp;
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_traildb_TrailDBMultiTrail_free(JNIEnv *env, jobject obj) {

}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    reset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_traildb_TrailDBMultiTrail_reset(JNIEnv *env, jobject obj) {

}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    next
 * Signature: ()Ltraildb/TrailDBMultiTrail;
 */
JNIEXPORT jobject JNICALL Java_traildb_TrailDBMultiTrail_next(JNIEnv *env, jobject obj) {
	tdb_multi_cursor *multi_cur;
	const tdb_multi_event *multi_event;
	const tdb_event *event;

	TrailDBMultiTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBMultiTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields);

	// Get cur pointer

	multi_cur = (tdb_multi_cursor *) fields->multi_cur;

	// Call multi cursor next

	multi_event = tdb_multi_cursor_next(multi_cur);

	if (multi_event == NULL) {
		fields->timestamp = 0L;
		fields->num_items = 0L;
		fields->items = 0L;

		return NULL;
	}

	event = multi_event->event;

	fields->timestamp = (long) event->timestamp;
	fields->num_items = (long) event->num_items;
	fields->items = (long) event->items;
	fields->db = (long) multi_event->db;
	fields->cursor_idx = (long) multi_event->cursor_idx;

	return obj;
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    nextBatch
 * Signature: (I)[Ltraildb/TrailDBMultiTrail;
 */
JNIEXPORT jobjectArray JNICALL Java_traildb_TrailDBMultiTrail_nextBatch(JNIEnv *env, jobject obj, jint max_events) {

}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    peek
 * Signature: ()Ltraildb/TrailDBMultiTrail;
 */
JNIEXPORT jobject JNICALL Java_traildb_TrailDBMultiTrail_peek(JNIEnv *env, jobject obj) {
	tdb_multi_cursor *multi_cur;
	const tdb_multi_event *multi_event;
	const tdb_event *event;

	TrailDBMultiTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBMultiTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBMultiTrail_fields);

	// Get cur pointer

	multi_cur = (tdb_multi_cursor *) fields->multi_cur;

	// Call multi cursor next

	multi_event = tdb_multi_cursor_peek(multi_cur);

	if (multi_event == NULL) {
		fields->timestamp = 0L;
		fields->num_items = 0L;
		fields->items = 0L;

		return NULL;
	}

	event = multi_event->event;

	fields->timestamp = (long) event->timestamp;
	fields->num_items = (long) event->num_items;
	fields->items = (long) event->items;
	fields->db = (long) multi_event->db;
	fields->cursor_idx = (long) multi_event->cursor_idx;

	return obj;
}

/*
 * Class:     traildb_TrailDBMultiTrail
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_traildb_TrailDBMultiTrail_initIDs(JNIEnv *env, jclass cls) {

	jclass traildb_TrailDBTrail = (*env)->FindClass(env, "traildb/TrailDBTrail");

	FID_traildb_TrailDBMultiTrail_fields = (*env)->GetFieldID(env, cls, "fields", "J");

	FID_traildb_TrailDBTrail_fields = (*env)->GetFieldID(env, traildb_TrailDBTrail, "fields", "J");

}
