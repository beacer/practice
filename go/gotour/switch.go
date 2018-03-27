package main

import (
	"fmt"
	"runtime"
	"time"
)

func pow(i int) int {
	return i*i
}

func main() {
	fmt.Print("Go runs on ")
	switch os := runtime.GOOS; os {  // swich with short-statement
	case "darwin": // no need constant, no need int
		fmt.Println("OS X.")	 // no break needed
	case "linux":
		fmt.Println("Linux.")
	default:
		fmt.Printf("%s.", os)
	}

	/* switch with funcion invoking,
	 * and switch stops one of the case is true. */
	switch i := 4; i {
	case 0:
		fmt.Println("Zero")
	case pow(2):
	}

	fmt.Println("When's Saturday?")
	today := time.Now().Weekday()
	switch time.Saturday {
	case today + 0:
		fmt.Println("Today.");
	case today + 1:
		fmt.Println("Tomorrow.");
	case today + 2:
		fmt.Println("In two days.");
	case today + 3:
		fmt.Println("In three days.");
	case today + 4:
		fmt.Println("In four days.");
	default:
		fmt.Println("Too far away.");
	}

	t := time.Now()
	switch { // swith with no condition
	case t.Hour() < 12: // replace if .. elif .. elif .. else
		fmt.Println("Good morning!")
	case t.Hour() < 17:
		fmt.Println("Good afternoon!")
	default:
		fmt.Println("Good evening!")
	}
}
