package main

import "fmt"

func main() {
	i := 42 // int
	f := 3.142 //float64
	g := 0.867 + 0.5i // complex128
	fmt.Printf("%v is of type %T\n", i, i)
	fmt.Printf("%v is of type %T\n", f, f)
	fmt.Printf("%v is of type %T\n", g, g)
}
