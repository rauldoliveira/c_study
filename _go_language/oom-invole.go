package main

import "fmt"

func main() {
	buf := make([]byte, 10000000)
	for i := 0; i < 100; i++ {

		buf = append(buf, buf...)
		fmt.Println("len: ", len(buf))
	}
}