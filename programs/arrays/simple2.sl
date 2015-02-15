func newArray(Int length): Int[] {
	return new Int[length];
}

func main(): Int {
	Int[] array = newArray(4);
	array[0] = 4;
	return array[0];
}