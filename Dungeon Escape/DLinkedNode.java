/**
 * DLinkedNode represents a node in a doubly linked list used by DLPriorityQueue.
 *
 * @author Shikha Patel
 * (Based on CS1027 assignment spec)
 */
public class DLinkedNode<T> {
    private T dataItem;
    private double priority;
    private DLinkedNode<T> next;
    private DLinkedNode<T> prev;

    /** Creates a node storing the given data item and priority. */
    public DLinkedNode(T data, double prio) {
        this.dataItem = data;
        this.priority = prio;
        this.next = null;
        this.prev = null;
    }

    /** Creates an empty node, with null dataItem and zero priority. */
    public DLinkedNode() {
        this(null, 0.0);
    }

    public double getPriority() { return priority; }
    public T getDataItem() { return dataItem; }
    public DLinkedNode<T> getNext() { return next; }
    public DLinkedNode<T> getPrev() { return prev; }

    public void setPriority(double prio) { this.priority = prio; }
    public void setDataItem(T data) { this.dataItem = data; }
    public void setNext(DLinkedNode<T> n) { this.next = n; }
    public void setPrev(DLinkedNode<T> p) { this.prev = p; }
}
