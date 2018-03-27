package main

import "fmt"

type Vertex struct {
	X int
	Y int
}

var (
	v1 = Vertex{1, 2}
	v2 = Vertex{X: 1} // left Y as 0
	v3 = Vertex{}     // both X and Y are 0
	p  = &Vertex{1, 2}
)

func main() {
	fmt.Println(Vertex{1, 2})

	v := Vertex{1, 2}
	v.X = 4 // use dot to access struct field
	fmt.Println(v.X)

	p := &v
	p.X = 1e9 // use dot to access struct pointer's field
	fmt.Println(v)

	fmt.Println(v1, p, v2, v3)
}
