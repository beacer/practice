// Dup1 prints the text of echo line that appears more than
// once in the standard input, preceded by it's count.
package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	counts := make(map[string]int)
	input := bufio.NewScanner(os.Stdin)

	/* input.Scan is an iterator
	 * echo time being invoked, it read a new line
	 * to input.Text and remove the tailing '\n'.
	 * it returns true if new line read or returns false */
	for input.Scan() {
		counts[input.Text()]++
	}

	// range works for map also, not only array
	// returns <key, value> pair each time.
	for line, n := range counts {
		if n > 1 {
			fmt.Printf("%d\t%s\n", n, line)
		}
	}
}
