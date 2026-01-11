class NewtonTest {

	private static final String DIVIDE = "divide by zero error";
	private static final String ITER = "maximum iteration exceeded";
	
	public static void main (String[] args) { 
		
		System.out.println( "Test 01 " + (test01() ? "passed" : "failed" ));
		System.out.println( "Test 02 " + (test02() ? "passed" : "failed" ));
		System.out.println( "Test 03 " + (test03() ? "passed" : "failed" ));
		System.out.println( "Test 04 " + (test04() ? "passed" : "failed" ));
		System.out.println( "Test 05 " + (test05() ? "passed" : "failed" ));
		System.out.println( "Test 06 " + (test06() ? "passed" : "failed" ));
		System.out.println( "Test 07 " + (test07() ? "passed" : "failed" ));
		System.out.println( "Test 08 " + (test08() ? "passed" : "failed" ));
		System.out.println( "Test 09 " + (test09() ? "passed" : "failed" ));
		System.out.println( "Test 10 " + (test10() ? "passed" : "failed" ));
		System.out.println( "Test 11 " + (test11() ? "passed" : "failed" ));
		System.out.println( "Test 12 " + (test12() ? "passed" : "failed" ));
		System.out.println( "Test 13 " + (test13() ? "passed" : "failed" ));
		System.out.println( "Test 14 " + (test14() ? "passed" : "failed" ));
		System.out.println( "Test 15 " + (test15() ? "passed" : "failed" ));

	}
	


	public static boolean test01() { 
		Polynomial p = new Polynomial();

		p.add(1,2);
		p.add(-1,1);
		p.add(+1,0);

		String out = p.toString();
		return out.equals("1*x^2 - 1*x^1 + 1*x^0");
	}



	public static boolean test02() {

		Polynomial p = new Polynomial();

		p.add(-1,1);
		p.add(3,3);
		p.add(-2,2);

		String out = p.toString();
		return out.equals("3*x^3 - 2*x^2 - 1*x^1");
	}

	public static boolean test03() {
		Polynomial p = new Polynomial();

		p.add(6,0);
		p.add(10,10);
		p.add(-5,5);
		p.add(3,3);
		p.add(1,1);

		String out = p.toString();
		return out.equals("10*x^10 - 5*x^5 + 3*x^3 + 1*x^1 + 6*x^0");

	}

	public static boolean test04() { 
		Polynomial p = new Polynomial();
		p.add(1,1);

		return Math.abs(p.eval(3)-3) < 1e-1;
	}

	public static boolean test05() { 
		Polynomial p = new Polynomial();
		p.add(9001,0);
		return Math.abs( p.eval(-111)-9001) < 1e-1;
	}

	public static boolean test06() { 
		Polynomial p = new Polynomial();
		p.add(1,2);
		p.add(2,1);
		p.add(3,0);

		return Math.abs(p.eval(0.5)-4.25) < 1e-1;
	}

	public static boolean test07() {
		Polynomial p = new Polynomial();

		p.add(1,3);

		Polynomial deriv = p.derivative();
		String derivString = deriv.toString();
		return derivString.equals( "3*x^2");

	}

	public static boolean test08() {
		Polynomial p = new Polynomial();

		p.add(6,0);
		p.add(10,10);
		p.add(-5,5);
		p.add(3,3);
		p.add(1,1);

		Polynomial deriv = p.derivative();
		String derivString = deriv.toString();

		return derivString.equals("100*x^9 - 25*x^4 + 9*x^2 + 1*x^0");
	}

	public static boolean test09() {
		Polynomial p = new Polynomial();
		p.add(1,2);
		p.add(-2,0);

		double result = 0;
		try {
			result = p.solve(1, 1e-10, 200);
			return Math.abs( result - Math.sqrt(2))< 1e-6;
		} catch (SolutionNotFound s) {
			return false; // exception triggered.
		}
	}

	public static boolean test10() {
		Polynomial p = new Polynomial();
		p.add(1,2);
		p.add(-2,0);

		double result = 0;
		try {
			result = p.solve(-1, 1e-10, 200);
			return Math.abs(result + Math.sqrt(2)) < 1e-6;
		} catch (SolutionNotFound s) {
			return false; // exception triggered.
		}
	}

	public static boolean test11() {
		Polynomial p = new Polynomial();
		p.add(16,7);
		p.add(-384,6);
		p.add(2512,5);
		p.add(-1188,4);
		p.add(-17549,3);
		p.add(21921,2);
		p.add(4230,1);
		p.add(-5400,0);

		double result = 0;
		try {
			result = p.solve(0, 1e-10, 200);
			return Math.abs( result- 1.5) < 1e-6;
		} catch (SolutionNotFound s) {
			return false; // exception triggered.
		}
	}

	public static boolean test12() {
		Polynomial p = new Polynomial();

		p.add(16,7);
		p.add(-384,6);
		p.add(2512,5);
		p.add(-1188,4);
		p.add(-17549,3);
		p.add(21921,2);
		p.add(4230,1);
		p.add(-5400,0);

		double result = 0;
		try {
			result = p.solve(3, 1e-10, 200);
			return Math.abs( result - 3.0) < 1e-6;
		} catch (SolutionNotFound s) {
			return false;
		}
	}

	public static boolean test13 () { 
		Polynomial p = new Polynomial();
		p.add(1,2);
		p.add(1,0);

		try {
			p.solve(0, 1, 1000);
			return false;
		} catch (SolutionNotFound snf) { 
		    return (snf.getMessage().equals(DIVIDE));
		}
	}

	public static boolean test14() { 
		Polynomial p = new Polynomial();

		p.add(16,7);
		p.add(-384,6);
		p.add(2512,5);
		p.add(-1188,4);
		p.add(-17549,3);
		p.add(21921,2);
		p.add(4230,1);
		p.add(-5400,0);

		double result = 0;
		try {
			result = p.solve(3, 1e-10, 200);
			double eval = p.eval(result);
			return Math.abs( eval) < 1e-6;
		} catch (SolutionNotFound s) {
			return false;
		}
	}

	public static boolean test15() { 

		// example from en.wikipedia.org/wiki/Newton%27s_method#Starting_point_enters_a_cycle
		Polynomial p = new Polynomial();
		p.add(1,3);
		p.add(-2,1);
		p.add(2,0);

		try {
			p.solve(0, 1e-12, 10000);
			return false;
		} catch (SolutionNotFound snf) { 
		    return (snf.getMessage().equals(ITER));
		}
	}
	
}