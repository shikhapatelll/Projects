/**
 * DLPriorityQueue implements a priority queue using a doubly linked list.
 * Items are kept sorted in non-decreasing order of priority (min at front).
 *
 * @author Shikha Patel
 * (Based on CS1027 assignment spec)
 */
public class DLPriorityQueue<T> implements PriorityQueueADT<T> {

    private DLinkedNode<T> front;
    private DLinkedNode<T> rear;
    private int count;

    /** Creates an empty priority queue. */
    public DLPriorityQueue() {
        front = null;
        rear = null;
        count = 0;
    }

    /** Returns the number of data items in the priority queue. */
    public int size() {
        return count;
    }

    /** Returns true if the priority queue is empty. */
    public boolean isEmpty() {
        return count == 0;
    }

    /** Returns rear node (used by the provided tester). */
    public DLinkedNode<T> getRear() {
        return rear;
    }

    /**
     * Adds a data item with the given priority, keeping the list sorted in non-decreasing
     * order of priority (smallest at front, largest at rear).
     */
    public void add(T dataItem, double priority) {
        DLinkedNode<T> node = new DLinkedNode<T>(dataItem, priority);

        if (isEmpty()) {
            front = node;
            rear = node;
            count = 1;
            return;
        }

        // Insert at front
        if (priority < front.getPriority()) {
            node.setNext(front);
            front.setPrev(node);
            front = node;
            count++;
            return;
        }

        // Insert at rear
        if (priority >= rear.getPriority()) {
            rear.setNext(node);
            node.setPrev(rear);
            rear = node;
            count++;
            return;
        }

        // Insert in the middle: after the last node with priority <= new priority (stable)
        DLinkedNode<T> current = front;
        while (current != null && current.getPriority() <= priority) {
            current = current.getNext();
        }
        // current is the first node with priority > new priority, so insert before current
        DLinkedNode<T> prev = current.getPrev();
        prev.setNext(node);
        node.setPrev(prev);
        node.setNext(current);
        current.setPrev(node);
        count++;
    }

    /** Removes and returns the data item with smallest priority (front of the list). */
    public T removeMin() throws EmptyPriorityQueueException {
        if (isEmpty()) {
            throw new EmptyPriorityQueueException("Priority queue is empty");
        }

        T data = front.getDataItem();

        if (count == 1) {
            front = null;
            rear = null;
            count = 0;
            return data;
        }

        front = front.getNext();
        front.setPrev(null);
        count--;
        return data;
    }

    /**
     * Updates the priority of the given data item to newPriority. After updating, the
     * node may be moved so that the linked list remains sorted.
     */
    public void updatePriority(T dataItem, double newPriority) throws InvalidElementException {
        if (isEmpty()) {
            throw new InvalidElementException("Cannot update priority: element not found (queue is empty)");
        }

        // Find the node
        DLinkedNode<T> current = front;
        while (current != null) {
            T curData = current.getDataItem();
            if (curData == dataItem || (curData != null && curData.equals(dataItem))) {
                break;
            }
            current = current.getNext();
        }

        if (current == null) {
            throw new InvalidElementException("Element not found in priority queue");
        }

        // If priority doesn't change, nothing to do
        if (current.getPriority() == newPriority) {
            return;
        }

        // Detach current from the list
        DLinkedNode<T> prev = current.getPrev();
        DLinkedNode<T> next = current.getNext();

        if (prev != null) prev.setNext(next);
        else front = next;

        if (next != null) next.setPrev(prev);
        else rear = prev;

        // Reinsert the same node with the new priority
        current.setPriority(newPriority);
        current.setNext(null);
        current.setPrev(null);

        // If list became empty after removal
        if (front == null) {
            front = current;
            rear = current;
            return;
        }

        // Insert at front
        if (newPriority < front.getPriority()) {
            current.setNext(front);
            front.setPrev(current);
            front = current;
            return;
        }

        // Insert at rear
        if (newPriority >= rear.getPriority()) {
            rear.setNext(current);
            current.setPrev(rear);
            rear = current;
            return;
        }

        // Insert in the middle (stable)
        DLinkedNode<T> scan = front;
        while (scan != null && scan.getPriority() <= newPriority) {
            scan = scan.getNext();
        }
        DLinkedNode<T> scanPrev = scan.getPrev();
        scanPrev.setNext(current);
        current.setPrev(scanPrev);
        current.setNext(scan);
        scan.setPrev(current);
    }

    /**
     * Returns a String representation of the priority queue by scanning from
     * front to rear and concatenating each data item's toString().
     */
    public String toString() {
        StringBuilder sb = new StringBuilder();
        DLinkedNode<T> current = front;
        while (current != null) {
            if (current.getDataItem() != null) {
                sb.append(current.getDataItem().toString());
            }
            current = current.getNext();
        }
        return sb.toString();
    }
}
