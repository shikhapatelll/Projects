
public class Polynomial {

    // private instance variable
    // monomials of type OrderedLinkedList<Monomial>, which stores the monomials of the polynomial
    private OrderedLinkedList<Monomial> monomials;

    // constructor of the class
    // initializes a new OrderedLinkedList of Monomial type to store the monomials
    // when a Polynomial object is created
    public Polynomial() {
        monomials = new OrderedLinkedList<>();
    }


    // method adds a new monomial to the polynomial
    // takes two parameters(coefficient and degree) and inserts a new Monomial into the OrderedLinkedList
    public void add(int coefficient, int degree) {
        monomials.insert(new Monomial(coefficient, degree));
    }


    // calculates the derivative of the polynomial
    public Polynomial derivative() {
        //  returns a new Polynomial object representing the derivative of the current polynomial
        Polynomial derivative = new Polynomial();
        // creates a new Polynomial object named derivative to store the result of the derivative

        // a loop that iterates over each monomial in the original polynomial
        for (int x = 0; x < monomials.getSize(); x++) {

            // retrieves the current monomial at the given index in the loop
            Monomial currentMonomial = monomials.get(x);

            // calculates the new coefficient for the derivative by multiplying the current coefficient with the current degree
            int newCoefficient = currentMonomial.getCoefficient() * currentMonomial.getDegree();
            int newDegree = currentMonomial.getDegree() - 1;
            // calculates the new degree for the derivative by subtracting 1 from the current degree

            if (newDegree >= 0) { // condition checks if the new degree is greater than or equal to 0
                derivative.add(newCoefficient, newDegree);
                // If condition is true
                // it adds the new monomial to the derivative polynomial
            }
        }

        return derivative;
        // returns the derivative polynomial
        // which will represents the derivative of the original polynomial
    }

    //  evaluates the polynomial for a given value of x
    public double eval(double x) { //  takes a parameter x and returns a double
        // representing the result of evaluating the polynomial for the given value of x
        double result = 0;// initializes a variable result to store the cumulative result of evaluating the polynomial

        //  a loop that iterates over each monomial in the polynomial
        for (int y = 0; y < monomials.getSize(); y++) {
            // retrieves the current monomial at the given index in the loop
            Monomial currentMonomial = monomials.get(y);

            result += currentMonomial.getCoefficient() * Math.pow(x, currentMonomial.getDegree());
            // calculates the contribution of the current monomial to the result by multiplying the monomial's coefficient with
        }

        return result;
        // returns result
        //representing the value of the polynomial when evaluated for the given value of x
    }

    // method that gives a string representation of all nonzero monomials in the polynomial
    public String toString() {
        // initializes a StringBuilder named result to construct the string representation of the polynomial
        StringBuilder result = new StringBuilder();
        for (int x = 0; x < monomials.getSize(); x++) { // loop that iterates over each monomial in the polynomial

            //retrieves the current monomial at the given index in the loop
            Monomial currentMonomial = monomials.get(x);

            int coefficient = currentMonomial.getCoefficient(); // gets the coefficient of the current monomial
            int degree = currentMonomial.getDegree(); // gets the degree of the current monomial

            // checks if the result is empty
            // it's the first monomial being processed
            if (result.length() == 0) {

                result.append(Math.abs(coefficient)); // appends the absolute value of the coefficient to the result
                result.append("*x^").append(degree); // appends the term representing the monomial to the result
            }

            // executed for monomials after the first one
            else {

                if (coefficient < 0) { // to see if the coefficient is negative
                    result.append(" - "); // appends a minus sig
                    result.append(Math.abs(coefficient)); // A=appends the absolute value of the coefficient
                }

                if (coefficient > 0) { // to see if the coefficient is positive
                    result.append(" + "); //  appends a plus sign
                    result.append(Math.abs(coefficient)); // appends the absolute value of the coefficient

                }

                result.append("*x^").append(degree);
                //  Appends the term representing the monomial to the result
            }

        }

        return result.toString();
        //  returns the constructed string representation of the polynomial
    }


    // method takes three parameters(two doubles and an integer)
    // returns a double
    public double solve(double initial, double tolerance, int maxIterations)
            throws SolutionNotFound {
        double previous = initial; //  initializing the variable previous with the initial guess for the solution
        double current; // it will be used to store the updated guess for the solution
        // Declaring variable current

        //  loop for the Newton's Method iterations
        for (int iteration = 0; iteration < maxIterations; iteration++) { // it will continue until the maximum number of iterations is reached

            Polynomial derivative = derivative(); // calculates the derivative of the current polynomial
            double derivativeValue = derivative.eval(previous); // evaluates the derivative at the current guess

            // Checking if the derivative is zero
            if (derivativeValue == 0) { // result in a division by zero in Newton's Method
                // if so throws an exception with the message "divide by zero error"
                throw new SolutionNotFound("divide by zero error");
            }

            // it will update the guess for the solution using the Newton's Method formula
            current = previous - (eval(previous) / derivativeValue);

            // it will check if the difference between the current and previous guesses is within the specified tolerance
            if (Math.abs(current - previous) < tolerance) {
                return current;
                // Returns the current if the tolerance is met
            }

            // it will update the previous variable for the next iteration
            previous = current;

        }

        //  Throws an exception
        //  if the maximum number of iterations is exceeded without reaching the desired tolerance
        throw new SolutionNotFound("maximum iteration exceeded");
    }

}