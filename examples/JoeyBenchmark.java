import traildb.*;

import java.io.FileNotFoundException;

public class JoeyBenchmark {
	public static void main(String[] args) throws FileNotFoundException {
		TrailDB tdb = new TrailDB("wikipedia-history-small.tdb");
		TrailDBTrail trail = new TrailDBTrail(tdb, 0);
		long userEdits = 0;
		long ipEdits = 0;

		int ipIndex = tdb.fieldMap.get("ip");
		int userIndex = tdb.fieldMap.get("user");
		System.out.println("IP index: " + ipIndex);
		System.out.println("User index: " + userIndex);

		do {
			while (trail.next() != null) {
//				System.out.println("User: " + trail.getItem(userIndex));
//				System.out.println("IP: " + trail.getItem(ipIndex));
				if (!"".equals(trail.getItem(userIndex))) {
					userEdits++;
				} else if (!"".equals(trail.getItem(ipIndex))) {
					ipEdits++;
				}
			}
		} while (trail.nextTrail());

		System.out.println("User edits: " + userEdits);
		System.out.println("IP edits: " + ipEdits);
	}
}
