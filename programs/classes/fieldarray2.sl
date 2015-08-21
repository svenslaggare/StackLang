func createCharArray(std::String str): Char[] {
	var chars = new Char[str.length()];

	for (var i = 0; i < str.length(); i += 1) {
		chars[i] = str.charAt(i);
	}

	return chars;
}

class TestString {
	private Char[] chars;

	TestString(Char[] inChars) {
		chars = inChars;
	}

	func length(): Int {
    	return chars.length;
    }

    func charAt(Int index): Char {
   		return chars[index];
   	}

	func copyTo(TestString other): Void {
		other.chars = new Char[chars.length];

		for (var i = 0; i < other.chars.length; i += 1) {
			other.chars[i] = chars[i];
		}
	}
}

func println(TestString str): Void {
	for (var i = 0; i < str.length(); i += 1) {
		std::printchar(str.charAt(i));
	}

	std::printchar('\n');
}

func main(): Int {
	var str1 = new TestString(createCharArray("String 1"));
	var str2 = new TestString(createCharArray("String 2"));
	str1.copyTo(str2);
	println(str2);
	return 0;
}