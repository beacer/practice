package main

import (
	"flag"
	"fmt"
	"strings"
)

// <name, default, description>
// return flag @var's pointer
var n = flag.Bool("n", false, "omit trailing newline")
var sep = flag.String("s", " ", "separator")

func main() {
	flag.Parse()
	fmt.Print(strings.Join(flag.Args(), *sep))
	if !*n {
		fmt.Println()
	}
}
