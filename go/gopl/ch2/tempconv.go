package tempconv

import "fmt"

type Celsius float64
type Fahrenheit float64

const (
	AbsoluteZeroC Celsius = -273.15
	FreezingC     Celsius = 0
	BoilingC      Celsius = 100
)

// interface/method passed to type Celsius
func (c CelsiusA) String() string {
	return fmt.Sprintf("%g℉\n", c)
}

func (f Fahrenheit) String() string {
	return fmt.Sprintf("%g℉\n", f)
}
