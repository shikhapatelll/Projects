// class named Node with a type parameter T

public class Node<T> {

    // private instance variables
    private T data; // data stores the data of type 'T'.

    private Node<T> next; // next is a reference to the next Node object in the linked list, and its type is Node<T>
    // To allow the creation of a linked list where each node points to the next node.

   // constructor of the Node class
    // takes two parameters data of type T and next of type Node<T>
    public Node(T data, Node<T> next) {
        this.data = data;
        this.next = next;
        // whenever new Node object is created,
        // this constructor initializes data and next instance variables with the values passed as parameters
    }

    // method is an accessor(getter) for the data variable
    public T getData() {
        return data;
        // allowing to retrieve the value of the data variable from outside the class
    }

    // method is an accessor for the next variable
    public Node<T> getNext() {
        return next;
        // allowing to retrieve the next Node in the linked list
    }

    //  method is a mutator(setter) for the next variable
    public void setNext(Node<T> next) {
        this.next = next;
        //  allowing to set the next Node in the linked list
        // basically takes a parameter of type Node<T> and sets the next variable to the provided value
    }

}


