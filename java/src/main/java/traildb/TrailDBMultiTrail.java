package traildb;

public class TrailDBMultiTrail {

	private long fields;

	public TrailDBMultiTrail(TrailDBTrail[] trails) {
		init(trails);
	}

	private native void init(TrailDBTrail[] trails);

	/**
	 * Get item i that trail cursor is currently pointing at
	 * @param i index of item to get
	 * @return The item value
	 */
	public native String getItem(int i);

	/**
	 * Get number of items the event has
	 * @return number of items
	 */
	public native int getNumItems();

	public native long getTimestamp();

	public native void free();

	public native void reset();

	public native TrailDBMultiTrail next();

	public native TrailDBMultiTrail[] nextBatch(int maxEvents);

	public native TrailDBMultiTrail peek();

	private static native void initIDs();

	static {
		System.loadLibrary("TraildbJavaNative");
		initIDs();
	}
}
