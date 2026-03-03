# Dungeon Escape — Shortest Path Finder (Java)

A hexagon-tile dungeon escape visualizer that finds a **shortest safe path** from the entrance to the exit using a custom **Priority Queue (Doubly Linked List)**.

## What it does
- Loads a dungeon map from a `.txt` file (see `dungeon1.txt` … `dungeon7.txt`)
- Visualizes chambers as hex tiles (walls, start, exit, dragons)
- Runs a shortest-path search that **avoids**:
  - wall chambers
  - dragon chambers
  - any chamber adjacent to a dragon
- Uses an A*-style priority:  
  `priority = distanceToStart + estimatedDistanceToExit (Euclidean)`

## How to run (Terminal)
1. Put all files in the same folder (this repo does that)
2. Compile:
   ```bash
   javac *.java
   ```
3. Run with any sample dungeon:
   ```bash
   java FindShortestPath dungeon1.txt
   ```

You should see the dungeon window and the algorithm marking tiles as it explores.
When it reaches the exit, it prints:
`Path of length X found`  
or `No path found` if no safe path exists.

## Key files
- `DLinkedNode.java` — node for doubly linked list
- `DLPriorityQueue.java` — priority queue implementation (no Java PriorityQueue used)
- `FindShortestPath.java` — pathfinding algorithm + main entry point
- `Dungeon.java`, `Hexagon.java` — provided visual dungeon framework

## Skills shown
Java • Data Structures • Priority Queue • A* Search • Graph traversal • Debugging • Clean error handling

---
**Author:** Shikha Patel
