//Monomial class
// implementing the Comparable interface for Monomial objects

public class Monomial implements Comparable<Monomial> {

    // private integer instance variable
    private int coefficient; // coefficient to store the coefficient of the monomial
    private int degree; // degree to store the degree of the monomial


    // constructor for the Monomial class
    public Monomial(int coefficient, int degree) { // makes coefficient and degree as parameters and initializes the instance variables

        this.coefficient=coefficient;
        //  Assigning value of the parameter coefficient to instance variable coefficient

        this.degree=degree;
        //  Assigning value of the parameter degree to instance variable degree

    }

    // method is an accessor
    // it will return coefficient of monomial
    public int getCoefficient() {
        return coefficient;
        // returns value of coefficient instance variable

    }

    // method is an accessor
    // it will return degree of monomial
    public int getDegree() {
        return degree;
        // returns the value of the degree instance variable
    }


    // a compareTo method that accepts a Monomial as a parameter and returns an integer
    // It determines which Monomial has the higher degree
    @Override
    public int compareTo(Monomial m) { // method compares the degrees of two monomials for ordering, follows the contract of the Comparable interface

        return this.degree-m.degree; // Compares the degrees of the current monomial(this) and monomial passed as a parameter(m)
        //  result is positive if the current monomial has a higher degree
        // result is negative if it has a lower degree
        // result is zero if they have the same degree

    }

}