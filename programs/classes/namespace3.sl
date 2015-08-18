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

		func clone(): RTString {
			return new RTString(chars);
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
	var str = new std::RTString("Hello, World!");
	var str2 = str.clone();
	std::println(str2.clone());
	return 0;
}