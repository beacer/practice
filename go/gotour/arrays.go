package main

import "fmt"

func main() {
	var a [2]string // fixed length array, cannot be changed
	a[0] = "hello"
	a[1] = "world"
	fmt.Println(a[0], a[1])
	fmt.Println(a)

	primes := [6]int{2, 3, 5, 7, 11, 13}
	fmt.Println(primes)

	// array slices
	var s []int = primes[1:4] // from elem 1 (include) to 3 (exclude)
	fmt.Println(s)

	/* slices like references ! it do not store data,
	 * just describes a section of an underlying array. */
	names := [4]string{"John", "Paul", "George", "Rongo"}
	fmt.Println(names)

	x := names[0:2]
	y := names[1:3]
	fmt.Println(x, y)

	y[0] = "XXX"
	fmt.Println(x, y)
	fmt.Println(names)

	q := []int{2, 3, 5, 7, 11, 13} // slice literal
	fmt.Println(q)

	/* slice of slice */
	q = q[1:4]
	fmt.Println(q)
	q = q[:2] // omit the high bounds
	fmt.Println(q)
	q = q[1:] // omit the low bounds
	fmt.Println(q)

	// struct array slice
	sa := []struct {
		i int
		b bool
	}{
		{2, true},
		{3, false},
		{5, true},
		{7, true},
		{11, false},
		{13, true},
	}
	fmt.Println(sa)
}
