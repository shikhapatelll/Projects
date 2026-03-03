import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * FindShortestPath solves the dungeon by finding a shortest path from the start
 * chamber to the exit using an A*-style priority queue strategy.
 *
 * Rules:
 * - Walls cannot be entered.
 * - Dragon chambers cannot be part of the path.
 * - Any chamber adjacent to a dragon cannot be part of the path.
 *
 * Run from terminal (all files in same folder):
 *   javac *.java
 *   java FindShortestPath dungeon1.txt
 *
 * @author Shikha Patel
 */
public class FindShortestPath {

    private static boolean isUnsafe(Hexagon chamber) {
        if (chamber == null) return true;
        // Chamber itself has a dragon
        if (chamber.isDragon()) return true;

        // Adjacent to a dragon
        for (int i = 0; i < 6; i++) {
            try {
                Hexagon n = chamber.getNeighbour(i);
                if (n != null && n.isDragon()) return true;
            } catch (InvalidNeighbourIndexException e) {
                // won't happen since i is 0..5
            }
        }
        return false;
    }

    public static void main(String[] args) {
        try {
            if (args.length < 1) throw new Exception("No input file specified. Example: java FindShortestPath dungeon1.txt");

            String dungeonFileName = args[0];

            Dungeon d = new Dungeon(dungeonFileName);
            Hexagon start = d.getStart();
            Hexagon exit = d.getExit();

            if (start == null) throw new Exception("Dungeon file did not include a start chamber (S).");
            if (exit == null) throw new Exception("Dungeon file did not include an exit chamber (E).");

            // Priority = distanceToStart + estimatedDistanceToExit
            DLPriorityQueue<Hexagon> pq = new DLPriorityQueue<Hexagon>();

            // Initialize
            start.setDistanceToStart(0);
            pq.add(start, 0.0);
            start.markEnqueued();

            boolean found = false;

            while (!pq.isEmpty() && !found) {
                Hexagon current = pq.removeMin();
                current.markDequeued();

                if (current.isExit()) {
                    found = true;
                    break;
                }

                // If current is unsafe (dragon or adjacent to dragon), skip it
                if (isUnsafe(current)) {
                    continue;
                }

                // Explore neighbors
                for (int i = 0; i < 6; i++) {
                    Hexagon neighbour = null;
                    try {
                        neighbour = current.getNeighbour(i);
                    } catch (InvalidNeighbourIndexException e) {
                        continue;
                    }

                    if (neighbour == null) continue;
                    if (neighbour.isWall()) continue;
                    if (neighbour.isMarkedDequeued()) continue;

                    // Do not build paths through unsafe chambers (dragon / adjacent dragon)
                    if (isUnsafe(neighbour)) continue;

                    int D = 1 + current.getDistanceToStart();

                    boolean improved = false;
                    if (neighbour.getDistanceToStart() > D) {
                        neighbour.setDistanceToStart(D);
                        neighbour.setPredecessor(current);
                        improved = true;
                    }

                    double newPriority = neighbour.getDistanceToStart() + neighbour.getDistanceToExit(d);

                    if (neighbour.isMarkedEnqueued()) {
                        if (improved) {
                            try {
                                pq.updatePriority(neighbour, newPriority);
                            } catch (InvalidElementException e) {
                                // If somehow not present, add it again
                                pq.add(neighbour, newPriority);
                            }
                        }
                    } else {
                        pq.add(neighbour, newPriority);
                        neighbour.markEnqueued();
                    }
                }
            }

            if (found) {
                int length = d.getExit().getDistanceToStart() + 1; // chambers count
                System.out.println("Path of length " + length + " found");
                // Mark exit processed (optional): d.getExit().markExit();
            } else {
                System.out.println("No path found");
            }

        } catch (InvalidDungeonCharacterException e) {
            System.out.println("Invalid dungeon character in input file: " + e.getMessage());
        } catch (FileNotFoundException e) {
            System.out.println("Input file not found: " + e.getMessage());
        } catch (IOException e) {
            System.out.println("I/O error while reading the dungeon file: " + e.getMessage());
        } catch (EmptyPriorityQueueException e) {
            System.out.println("Priority queue error: tried to remove from an empty queue.");
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }
}
