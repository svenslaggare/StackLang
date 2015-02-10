namespace std {
	namespace math {
		func intMaxValue(): Int {
			return 2147483647;
		}

		func intMinValue(): Int {
			return -2147483647;
		}

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

		func max(Float[] array): Float {
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