package main

import "fmt"

func printSlice(s []int) {
	// slice's
	// * length   - the number of elements it contains.
	// * capacity - the number of elements in the underlaying array,
	//              counting from the first elem in the slice.
	fmt.Println("len=%d cap=%d %v\n", len(s), cap(s), s)
}

func main() {
	s := []int{2, 3, 5, 7, 11, 13}
	printSlice(s)

	s = s[:0]
	printSlice(s)

	s = s[:4]
	printSlice(s)

	s = s[2:]
	printSlice(s)

	// nil slice
	var ns []int
	printSlice(ns)
	if ns == nil {
		fmt.Println("Nil!")
	}
}
