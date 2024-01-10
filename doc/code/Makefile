all: multiply.hex sections/seccoes1.hex sections/seccoes2.hex

multiply.hex: multiply.s
	../../build/p16as multiply.s -s .data=0x4000 -s .text=0x1000

sections/seccoes1.hex: sections/seccoes1.s
	../../build/p16as sections/seccoes1.s

sections/seccoes2.hex: sections/seccoes2.s
	../../build/p16as sections/seccoes2.s --section .data=0x1000 --section .text=0x3000
