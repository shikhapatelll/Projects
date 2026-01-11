// SolutionNotFound class
// specifying that it extends the built-in Exception class and SolutionNotFound is a type of exception

public class SolutionNotFound extends Exception{

    // constructor for the SolutionNotFound class
    // takes a String parameter Message When an exception type SolutionNotFound is created
    public SolutionNotFound(String Message) { // constructor is called to initialize the exception with a specific error message

        super(Message);
        // calls the constructor of the superclass(Exception) with the provided Message
        // this sets the error message for the exception.

    }

}

/*
 this SolutionNotFound class is a simple exception class
 that will allow to throw exceptions with a specific error message when a solution is not found
 error message is provided when an instance of SolutionNotFound is created
 it is passed to the constructor of Exception
*/
