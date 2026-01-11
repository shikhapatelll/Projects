
// class named OrderedLinkedList with a type parameter T
public class OrderedLinkedList<T extends Comparable> { // meaning type T must implement the Comparable interface, it has a compareTo method.

    // two private instance variables

    // to store the start of the linked list.
    private Node<T> head;

    // to keep track of the size(number of element) of the linked list
    private int size;

    // constructor of the class
    public OrderedLinkedList() {
        //  initializing an empty linked list by setting head to null and size to 0
        head=null;
        size=0;
    }

    public void insert(T value) {
        // this public method insert that takes a parameter of type T named value
        //  inserts an element into the linked list in an ordered manner

        Node<T> newNode = new Node<>(value, null);
        //Creates a new Node with the provided value and sets its next reference to null
        // This new node represents the element to be inserted

        // If the list is empty or the new element is larger than or equal to the head
        if (head == null || value.compareTo(head.getData()) >= 0) {
            // If the condition is true, new element should be inserted at the beginning of the list

            newNode.setNext(head); // newNode is set to point to the current head
            head = newNode; // head is updated to the newNode

        }

        // If the condition false
        // new element should be inserted somewhere other than the beginning of the list
        else {
            // initializing two pointers, current and previous
            Node<T> current = head; // current starts at the head
            Node<T> previous = null; // previous is initially set to null

            // This while loop iterates through the list as long as current is not null
            // and the value is smaller than the data of the current node.
            while (current != null && value.compareTo(current.getData()) < 0) {
                // In each iteration, previous is set to the current node and current moves to the next nod
                previous = current;
                current = current.getNext();

            }

            // newNode is linked to the current node when correct position is found
            newNode.setNext(current);

            //  If previous is not null, new element is not the largest in the list
            if (previous != null) {
                previous.setNext(newNode); // next reference of previous is set to the newNode
            }

            //  If previous is null, it means the new element is the larges
            else {
                head = newNode; // then head is updated to the newNode
            }
        }

        size++;
        // regardless of where the element was inserted
        // the size of the linked list is incremented
    }

    //  public method get that takes an integer parameter index and returns a value of type T
    public T get(int index) { // method ensures that the provided index is within bounds

        // if the provided index is less than 0 or greater than or equal to the size of the linked list
        if(index<0 || index>=size) {
            //  If true, it throws an IndexOutOfBoundsException
            throw new IndexOutOfBoundsException("Index out of bounds");
            // with an error message of "Index out of bounds"
        }

        Node<T> current=head;
        // loop iterates through the linked list
        // moving the current node to the node at the specified index
        for(int i=0; i<index; i++) {
            current = current.getNext();
            // stops when current reaches the desired index
        }

        // returns the data of the node at the specified index in the linked list
        return current.getData();
    }

    // retrieving the data at a specific index and returning the size of the linked list


    // public method getSize returns an integer representing the size of the linked list
    public int getSize() {
        return size;
        // returns the size of the linked list
    }

}