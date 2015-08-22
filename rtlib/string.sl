namespace std {
	class RTString {
		private Char[] chars;

		RTString(Char[] inChars) {
			chars = new Char[inChars.length];

			for (var i = 0; i < chars.length; i += 1) {
				chars[i] = inChars[i];
			}
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

			for (var i = 0; i < other.length(); i += 1) {
				newChars[this.length() + i] = other.chars[i];
			}

			return new RTString(newChars);
		}

		func equals(RTString other): Bool {
			if (other == null) {
				return false;
			}

			if (this == other) {
				return true;
			}

			if (this.length() != other.length()) {
				return false;
			}

			for (var i = 0; i < this.length(); i += 1) {
				if (this.charAt(i) != other.charAt(i)) {
					return false;
				}
			}

			return true;
		}
	}

	func println(std::RTString str): Void {
		for (var i = 0; i < str.length(); i += 1) {
			std::printchar(str.charAt(i));
		}

		std::printchar('\n');
	}
}