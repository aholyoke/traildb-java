
typedef struct TrailDBMultiTrailFields {
	long timestamp;
	long num_items;
	long items;
	long db;
	long multi_cur;
	long cursor_idx;
} TrailDBMultiTrailFields;

typedef struct TrailDBTrailFields {
	long timestamp;
	long num_items;
	long items;
	long db;
	long cur;
} TrailDBTrailFields;
