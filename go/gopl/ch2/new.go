package main

import "fmt"

func newInt() *int {
	return new(int)
}

func main() {
	p := new(int) // create a anonymous int var and return it's pointer
	fmt.Println(*p)
	*p = 2
	fmt.Println(*p)

	p = newInt()
	q := newInt()

	fmt.Println(p == q)
}
