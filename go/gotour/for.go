package main

import "fmt"

func main() {
	sum := 0
	for i := 0; i < 10; i++ {
		sum += i
	}

	fmt.Println(sum)

	tot := 1
	for ; tot < 1000; {
		tot += tot
	}
	fmt.Println(tot)

	sum = 1
	for sum < 1000 {  /* "for" is go's "while" */
		sum += sum
	}
	fmt.Println(sum)

	for { /* forever */
	}
}
