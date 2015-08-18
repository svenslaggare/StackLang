namespace std {
	class Point {
		Int x;
		Int y;
	}

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

		func asPoint(): Point {
			var point = new Point();
			point.x = chars.length;
			point.y = chars.length;
			return point;
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
	var point = str.asPoint();
	return point.x + point.y;
}