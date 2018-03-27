package main

import "fmt"

var c, python, java bool /* init to zero */
var i, j int = 1, 2 /* initializer */

func main() {
	var cpp, perl, shell = true, false, "no!" /* omit type, with initializer */
	k := 3 /* short var declaration inside func, type is omit */
	fmt.Println(c, python, java)
	fmt.Println(i, j, k)
	fmt.Println(cpp, perl, shell)
}
