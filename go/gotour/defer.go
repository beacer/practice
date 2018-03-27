package main

import "fmt"

func main() {
	// defers the exec of func until surrounding function returns.
	defer fmt.Println("world")

	fmt.Println("hello")

	for i := 0; i < 10; i++ {
		defer fmt.Println(i) // stacking defers, print done, 9, 8, ... 0
	}

	fmt.Println("done")
}
