// Echo2 prints its command line argument
package main

import (
	"fmt"
	"os"
)

/*
1. range 类似pythond的'inerator'（迭代器）,每次调用返回元素的index和元素
2. go的变量声明后必须使用，所以使用_空白标识符表示不使用的变量
3. 使用迭代器的性能比Echo1里面，每个循环执行len(os.Args)高！
4. 使用这种隐式遍历下标，比显式使用下标array[i]更不容易出错。
5. s变量每次都会指向新生成的字符串字面值(literal)，老的字符串会垃圾收集系统收集。
6. 每次生成新字符串字面值也很低效（反复分配内存），使用jion函数解决
*/
func main() {
	s, sep := "", ""

	for _, arg := range os.Args[1:] {
		s += sep + arg
		sep = " "
	}
	fmt.Println(s)
}
