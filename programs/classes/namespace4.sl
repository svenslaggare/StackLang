namespace std {
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
			var newChars = new Char[chars.length + other.chars.length];

			for (var i = 0; i < this.chars.length; i += 1) {
				newChars[i] = chars[i];
			}

			for (var i = 0; i < other.chars.length; i += 1) {
				newChars[chars.length + i] = other.chars[i];
			}

			return new RTString(newChars);
		}
	}

	func println(std::RTString str): Void {
    	for (var i = 0; i < str.length(); i += 1) {
    		std::printchar(str.charAt(i));
    	}

    	std::printchar('\n');
    }
}

func main(): Int {
	std::println(new std::RTString("Hello").concat(new std::RTString(", World!")));
	return 0;
}