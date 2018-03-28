package main

import "fmt"

// 闭包

func adder() func(int) int {
	sum := 0 // bound to returned functions each time.
	return func(x int) int {
		sum += x
		return sum
	}
}

func main() {
	pos, neg := adder(), adder()

	for i := 0; i < 10; i++ {
		fmt.Println(pos(i), neg(-2*i))
	}
}
