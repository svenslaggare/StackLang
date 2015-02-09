namespace core {
	func printArray(Int[] array): Void {
		for (Int i = 0; i < array.length; i += 1) {
			std::println(array[i]);
		}
	}
}

namespace std {
	namespace math {
		#func intMaxValue(): Int {
			#return 2147483647;
		#}

		#func intMinValue(): Int {
			#return -2147483648;
		#}

		func max(Int[] array): Int {
			var max = array[0];

			for (var i = 1; i < array.length; i += 1) {
				var current = array[i];

				if (current > max) {
					max = current;
				}
			}

			return max;
		}
	}
}

func main(): Int {
	var matrix = new Int[4, 4];

	#core::printArray(array);
	#std::println(std::math::max(array));
	return 0;
}