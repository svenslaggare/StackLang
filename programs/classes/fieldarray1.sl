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

	func concat(TestString other): TestString {
		var newChars = new Char[chars.length + other.chars.length];

		for (var i = 0; i < this.chars.length; i += 1) {
			newChars[i] = chars[i];
		}

		for (var i = 0; i < other.chars.length; i += 1) {
			newChars[chars.length + i] = other.chars[i];
		}

		return new TestString(newChars);
	}
}

func println(TestString str): Void {
	for (var i = 0; i < str.length(); i += 1) {
		std::printchar(str.charAt(i));
	}

	std::printchar('\n');
}

func main(): Int {
	var newStr = new TestString(createCharArray("Hello")).concat(new TestString(createCharArray(", World!")));
	println(newStr);
	return 0;
}