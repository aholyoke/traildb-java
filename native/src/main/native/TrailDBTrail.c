#include <traildb.h>
#include <string.h>
#include "traildb-java.h"
#include "traildb-java-static.h"

jfieldID FID_traildb_TrailDB_db;

jfieldID FID_traildb_TrailDBTrail_fields;

jfieldID FID_traildb_filters_TrailDBEventFilter_f;


JNIEXPORT void JNICALL Java_traildb_TrailDBTrail_init(JNIEnv * env, jobject obj, jobject tdb_obj, jlong trail_id) {
	const tdb *db;
	tdb_cursor *cur;

	TrailDBTrailFields *fields = malloc(sizeof(TrailDBTrailFields));

	// Retrieve db pointer

	db = (tdb *) (*env)->GetLongField(env, tdb_obj, FID_traildb_TrailDB_db);

	// Make new cursor

	cur = tdb_cursor_new(db);

	// Set trail id to initial trail

	tdb_get_trail(cur, trail_id);

	fields->cur = (long) cur;
	fields->db = (long) db;
	fields->items = 0L;

	(*env)->SetLongField(env, obj, FID_traildb_TrailDBTrail_fields, (long) fields);
}

JNIEXPORT jstring JNICALL Java_traildb_TrailDBTrail_getItem(JNIEnv *env, jobject obj, jint index) {
	jclass exc;

	const tdb *db;
	const tdb_item *items;
	const char *value;
	char *tgt_value;
	uint64_t value_length;

	TrailDBTrailFields *fields;

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Retrieve items pointer

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

	// Retrieve db pointer

	db = (tdb *) fields->db;

	// Get the value of the item

	value = tdb_get_item_value(db, items[index], &value_length);

	// Convert buffer to null-terminated string

	tgt_value = malloc(value_length * sizeof(char) + 1);

	strncpy(tgt_value, value, value_length);

	tgt_value[value_length] = '\0';

	return (*env)->NewStringUTF(env, tgt_value);
}

JNIEXPORT jint JNICALL Java_traildb_TrailDBTrail_getNumItems(JNIEnv *env, jobject obj) {
	jclass exc;

	TrailDBTrailFields *fields;

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

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

JNIEXPORT jlong JNICALL Java_traildb_TrailDBTrail_getTimestamp(JNIEnv *env, jobject obj) {
	jclass exc;

	TrailDBTrailFields *fields;

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

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

JNIEXPORT void JNICALL Java_traildb_TrailDBTrail_native_1getTrail(JNIEnv *env, jobject obj, jlong trail_id) {
	TrailDBTrailFields *fields;

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Get trail from cursor

	tdb_get_trail((tdb_cursor *) fields->cur, trail_id);

	// Initialize items to NULL because we haven't called next yet

	fields->items = 0L;
}

JNIEXPORT jlong JNICALL Java_traildb_TrailDBTrail_getTrailLength(JNIEnv *env, jobject obj) {
	TrailDBTrailFields *fields;

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Get trail length

	return tdb_get_trail_length((tdb_cursor *) fields->cur);
}

JNIEXPORT void JNICALL Java_traildb_TrailDBTrail_setEventFilter(JNIEnv *env, jobject obj, jobject filter) {
	jclass exc;

	TrailDBTrailFields *fields;
	struct tdb_event_filter *tgt_filter;
	tdb_error err;

	// Retrieve filter pointer

	tgt_filter = (struct tdb_event_filter *) (*env)->GetLongField(env, filter, FID_traildb_filters_TrailDBEventFilter_f);

	// Retrieve fields pointer

    fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Set the event filter on the cursor

	err = tdb_cursor_set_event_filter((tdb_cursor *) fields->cur, tgt_filter);

	if (err) {
		exc = (*env)->FindClass(env, "java/io/IOException");
		if (exc == NULL) {
			/* Could not find the exception - We are in so much trouble right now */
			exit(1);
		}
		(*env)->ThrowNew(env, exc, tdb_error_str(err));
		return;
	}
}

JNIEXPORT void JNICALL Java_traildb_TrailDBTrail_unsetEventFilter(JNIEnv *env, jobject obj) {
	TrailDBTrailFields *fields;

	// Retrieve fields pointer

	fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Unset event filter

	tdb_cursor_unset_event_filter((tdb_cursor *) fields->cur);

}

JNIEXPORT jobject JNICALL Java_traildb_TrailDBTrail_next(JNIEnv *env, jobject obj) {
	TrailDBTrailFields *fields;

	const tdb_event *event;

	// Retrieve fields pointer

	fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Get event

	event = tdb_cursor_next((tdb_cursor *) fields->cur);

	// Check if end of trail

	if (event == NULL) {
		fields->timestamp = 0L;
		fields->num_items = 0L;
		fields->items = 0L;

		return NULL;
	}

	fields->timestamp = (long) event->timestamp;
	fields->num_items = (long) event->num_items;
	fields->items = (long) event->items;

    // Return self for convenience

	return obj;
}

JNIEXPORT jobject JNICALL Java_traildb_TrailDBTrail_peek(JNIEnv *env, jobject obj) {
	TrailDBTrailFields *fields;

	const tdb_event *event;

	// Retrieve fields pointer

	fields = (TrailDBTrailFields *) (*env)->GetLongField(env, obj, FID_traildb_TrailDBTrail_fields);

	// Get event

	event = tdb_cursor_peek((tdb_cursor *) fields->cur);

	// Check if end of trail

	if (event == NULL) {
		fields->timestamp = 0L;
		fields->num_items = 0L;
		fields->items = 0L;

		return NULL;
	}

	fields->timestamp = (long) event->timestamp;
	fields->num_items = (long) event->num_items;
	fields->items = (long) event->items;

    // Return self for convenience

	return obj;
}

JNIEXPORT void JNICALL Java_traildb_TrailDBTrail_initIDs(JNIEnv *env, jclass cls) {

	jclass traildb_TrailDB = (*env)->FindClass(env, "traildb/TrailDB");

	jclass traildb_filters_TrailDBEventFilter = (*env)->FindClass(env, "traildb/filters/TrailDBEventFilter");

	FID_traildb_TrailDBTrail_fields = (*env)->GetFieldID(env, cls, "fields", "J");

	FID_traildb_filters_TrailDBEventFilter_f = (*env)->GetFieldID(env, traildb_filters_TrailDBEventFilter, "f", "J");

}
