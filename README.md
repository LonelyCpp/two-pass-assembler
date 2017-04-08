# Two-pass-assembler
A multi pass assembler implementation in c++

A direct implementation of the two pass algorithm for <i><b>compiler design</b></i> course provided by [VTU]( http://vtu.ac.in/), for CSE in semester 5.

An overview of the algorithm can be found here [Link](http://web.thu.edu.tw/ctyang/www/files/sp_chap2.pdf).
 
<h1>Sample Input :</h1> 

Label | Opcode | Operand
--- | --- | ---
TRANS | START | 1000
FIRST |	LDX |	ZERO
TLOOP |	TD |	INDEV
nop |	JEQ |	TLOOP
nop |	RD |	INDEV
nop |	STCH |	RECORD,X
nop |	TIX | 	TEN
nop |	JLT |	TLOOP
ZERO |	WORD |	0
TEN |	WORD |	10
INDEV |	BYTE |	X'F1'
RECORD |	RESB |	10
nop |	END |	FIRST

<h1>Output object code :</h1> 
<br/>
`H^0TRANS^001000^000026`
<br/>
`T^001000^1c^041015^e0101b^301003^d8101b^54901c^2c1018^381003^000000^000010^F1`
<br/>
`E^001000`
