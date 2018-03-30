package main

import (
	"fmt"
	"math"
)

type Vertex struct {
	X, Y float64
}

/* go does not have class, you can define methods on types */

/* a method is a function with a special "receiver" argument,
 * here receiver "(v Vertex)" defined between keyword "func" and function name "Abs"
 * Now method Abs has a receiver of type Vertex named v */

/* method Abs's receiver is Vertex v */
func (v Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

/* regular function */
func Abs2(v Vertex) float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func main() {
	v := Vertex{3, 4}
	fmt.Println(v.Abs()) // call struct Vertex's method Abs
	fmt.Println(Abs2(v)) // regular function call with argument
}
