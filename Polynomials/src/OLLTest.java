
public class OLLTest {

	public static void main (String[] args) { 

		
		System.out.println( "OLL Test 01 " + (test01() ? "passed" : "failed" ));
		System.out.println( "OLL Test 02 " + (test02() ? "passed" : "failed" ));
		System.out.println( "OLL Test 03 " + (test03() ? "passed" : "failed" ));
		System.out.println( "OLL Test 04 " + (test04() ? "passed" : "failed" ));
		System.out.println( "OLL Test 05 " + (test05() ? "passed" : "failed" ));

	}


	public static boolean test01() { 
		OrderedLinkedList<Integer> oll = new OrderedLinkedList<>();
		
		if (oll.getSize() != 0) {
			return false;
		}
		
		try {
			oll.get(0);
			return false;
		} catch (IndexOutOfBoundsException e) {
			return true;
		}

	}



	public static boolean test02() { 
		OrderedLinkedList<Integer> oll = new OrderedLinkedList<>();
		int x = (int) (Math.random()*100);
		oll.insert(x);

		return (oll.getSize() == 1) && (oll.get(0) == x);
	}

	public static boolean test03() { 
		OrderedLinkedList<Integer> oll = new OrderedLinkedList<>();
		oll.insert(5);

		try {
			oll.get(1);
			return false;
		} catch (IndexOutOfBoundsException e) {
			return true;
		}

	}

	public static boolean test04() { 
		OrderedLinkedList<Integer> oll = new OrderedLinkedList<>();

		int x = (int) (Math.random()*100);
		int y = (int) (Math.random()*100);
		int[] all = new int[] {x,y};

		oll.insert(x);
		oll.insert(y);

		if (oll.getSize()!= 2) {
			return false; 
		} else {
			int first = oll.get(0);
			int second = oll.get(1);

			return (first >= second) && (first == all[0] || first == all[1]) && (second == all[0] || second == all[1]);
		}

	}


	public static boolean test05() { 

		OrderedLinkedList<Integer> oll = new OrderedLinkedList<>();

		for (int i = 99; i >=0; i--) { 
			oll.insert(i);
		}

		boolean result = true;
		for (int i = 0; i< 100; i++) { 
			result = result && (i == 100-oll.get(i)-1);
		}
		return result;


	}

}

