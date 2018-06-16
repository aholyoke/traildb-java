package traildb;

import traildb.filters.TrailDBEventFilter;

public class TrailDBTrail {
	private long fields;

	private long currentTrail;

	private long numTrails;

	public TrailDBTrail(TrailDB tdb, long trailId) {
		currentTrail = trailId;
		numTrails = tdb.numTrails();
		init(tdb, trailId);
	}

	private native void init(TrailDB tdb, long trailId);

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

	public String[] getItems() {
		int numItems = getNumItems();
		String[] output = new String[(int) numItems];
		for (long i = 0; i < numItems; i++) {
			output[(int) i] = getItem((int) i);
		}
		return output;
	}

	/**
	 * Set the cursor to a new trailId
	 * @param trailId
	 */
	public void getTrail(long trailId) {
		currentTrail = trailId;
		native_getTrail(trailId);
	}

	private native void native_getTrail(long trailId);

	/**
	 * Set the cursor to the next trail.
	 * This is useful for processing all trails
	 * eg.
	 * do {
	 *   <process trail>
	 * } while (trail.nextTrail());
	 *
	 * @return succeded in getting another trail
	 */
	public boolean nextTrail() {
		currentTrail++;
		if (currentTrail >= numTrails) {
			return false;
		}
		native_getTrail(currentTrail);
		return true;
	}

	/**
	 * Get the length of the trail. This exhausts the trail
	 * @return
	 */
	public native long getTrailLength();

	public native void setEventFilter(TrailDBEventFilter filter);

	public native void unsetEventFilter();

	public native TrailDBTrail next();

	public native TrailDBTrail peek();

	private static native void initIDs();

	static {
		System.loadLibrary("TraildbJavaNative");
		initIDs();
	}
}
