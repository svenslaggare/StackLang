class RTString {
	private Char[] chars;

	RTString(Char[] inChars) {
		chars = inChars;
	}

	func charAt(Int index): Char {
		return chars[index];
	}

	func length(): Int {
		return chars.length;
	}

	func concat(RTString other): RTString {
		var newLength = chars.length + other.chars.length;
		var newChars = new Char[newLength];
		return new RTString(newChars);
	}
}

func println(RTString str): Void {
	for (var i = 0; i < str.length(); i += 1) {
		std::printchar(str.charAt(i));
	}

	std::printchar('\n');
}

func main(): Int {
	var str = new RTString("test");
	println(str);
	return 0;
}