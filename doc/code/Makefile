all: multiply.hex sections/seccoes1.hex sections/seccoes2.hex

multiply.hex: multiply.s
	../../build/pas multiply.s -s .data=0x4000 -s .text=0x1000

sections/seccoes1.hex: sections/seccoes1.s
	../../build/pas sections/seccoes1.s

sections/seccoes2.hex: sections/seccoes2.s
	../../build/pas sections/seccoes2.s
