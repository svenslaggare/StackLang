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

		for (var i = 0; i < this.length(); i += 1) {
			newChars[i] = chars[i];
		}

		var thisLength = chars.length;
		for (var i = 0; i < other.length(); i += 1) {
			var char = other.chars[i];
			newChars[thisLength + i] = char;
		}

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
	println(new RTString("Hello").concat(new RTString(", World!")));
	return 0;
}