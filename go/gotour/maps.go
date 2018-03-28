package main

import "fmt"

type Vertex struct {
	Lat, Long float64
}

// map is keys to values (dict)
var m map[string]Vertex // decline a nil map

var m1 = map[string]Vertex{
	"hello": Vertex{1, 0},
	"world": Vertex{0, 1},
}

func main() {
	// create a map by make and assign to map @m
	m = make(map[string]Vertex) // create a map by make and assign to map @m
	m["Bell Labs"] = Vertex{
		40.68433, -74.39967,
	}

	fmt.Println(m["Bell Labs"])

	// cannot add keys to nil map!
	// m["hello"] = Vertex{1,0} lead panic!!

	// add new key to non-nil map works
	m1["google"] = Vertex{37.42202, -122.08408}

	fmt.Println(m1["hello"])
	fmt.Println(m1["world"])
	fmt.Println(m1["google"])

	fmt.Println(m)
	fmt.Println(m1)
}
