package main

import "fmt"

func printSlice(s string, x []int) {
	fmt.Printf("%s len=%d cap=%d %v\n", s, len(x), cap(x), x)
}

func main() {
	a := make([]int, 5) // create a slice with both len and cap are 5
	printSlice("a", a)

	b := make([]int, 0, 5) // create a slice with len 0 and cap 5
	printSlice("b", b)

	c := b[:2] // extend the length from 0 to 2
	printSlice("c", c)

	d := c[2:5] // both len/cap are 3
	printSlice("d", d)
}
