package main

import "fmt"

const Pi = 3.14

const (
	Big = 1 << 100 // huge number
	Small = Big >> 99
)

func needInt(x int) int { return x*10 + 1 }
func needFloat(x float64) float64 { return x*0.1 }

func main() {
	const World = "世界" // unicode
	fmt.Println("Hello", World)
	fmt.Println("Happy", Pi, "Day")

	const Truth = true
	fmt.Println("Go rules?", Truth)

	fmt.Println(needInt(Small))
	//fmt.Println(needInt(Big)) /* overflow int */
	fmt.Println(needFloat(Small))
}
