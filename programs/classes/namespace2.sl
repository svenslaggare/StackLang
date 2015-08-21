func createCharArray(std::String str): Char[] {
	var chars = new Char[str.length()];

	for (var i = 0; i < str.length(); i += 1) {
		chars[i] = str.charAt(i);
	}

	return chars;
}

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
	}

	func println(std::RTString str): Void {
    	for (var i = 0; i < str.length(); i += 1) {
    		std::printchar(str.charAt(i));
    	}

    	std::printchar('\n');
    }
}

func main(): Int {
	var str = new std::RTString(createCharArray("Hello, World!"));
	std::println(str);
	return 0;
}