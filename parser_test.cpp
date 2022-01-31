#include "catch.hpp"

// put your parser tests here
#include "lexer.hpp"
#include "VMModel.hpp"
#include "ASMData.hpp"

#include <string>
#include <sstream>
#include <map>

TEST_CASE("test_bad_register_access", "[parser]") {
	{
		ASMData reg = findReg("notARegister");
		REQUIRE(reg.getName() == "");
	}
}

TEST_CASE("test_pass_unix_0", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word 1024             # int var1 = 1024

var2:	.half 12               # short var2 = 12
	
var3:	.byte 0                # char var3 = 0

var4:	.byte 1, 2, 3, 4, 5, 6, 7, 8  # var4 = {1,2,3,4,5,6,7,8}

var5:	.space 512             # reserve 512 bytes

var6:	.ascii "hello"

var7:	.asciiz "goodbye"

	.text
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_1", "[parser]") {

	{
		std::string input = R"(	# a test for constants
	.data
	LENGTH = 1024
arr:	.space LENGTH
	
	.text
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}
TEST_CASE("test_pass_unix_2", "[parser]") {

	{
		std::string input = R"(	# a test for basic data movement
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, 45
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_3", "[parser]") {

	{
		std::string input = R"(	# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0, 2
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_4", "[parser]") {

	{
		std::string input = R"(	# test of basic logical instructions
	.data
	TRUE = 1
	FALSE = 0

test1:	.word TRUE
test2:	.word FALSE
	
	.text
main:
	lw	$t0, test1
	lw	$t1, test2
	
	and	$t2, $t0, $t1
	and	$t2, $t0, TRUE
	nor	$t2, $t0, $t1
	nor	$t2, $t0, TRUE
	not	$t2, $t0
	not	$t2, $t0
	or	$t2, $t0, $t1
	or	$t2, $t0, TRUE
	xor	$t2, $t0, $t1
	xor	$t2, $t0, TRUE
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_5", "[parser]") {

	{
		std::string input = R"(	# test of the basic control instructions
	.data
x:	.word 1
y:	.word 2

	.text
main:
	lw $t0, x
	lw $t1, y
	
	beq $t0, $t0, next1
next1:

	bne $t0, $t1, next2
next2:

	blt $t0, $t1, next3
next3:

	ble $t0, $t0, next4
next4:

	bgt $t1, $t0, next5
next5:

	bge $t0, $t0, next6
next6:
	
end:
	j	end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_6", "[parser]") {

	{
		std::string input = R"(	# Example program to compute the sum of squares from Jorgensen [2016]

	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mul $t3, $t1, $t1
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_7", "[parser]") {

	{
		std::string input = R"(	# a test of all register names
	.data

	.text
main:
	sw $1, $0
	sw $at, $zero
	sw $2, $0
	sw $v0, $zero
	sw $3, $0
	sw $v1, $zero
	sw $4, $0
	sw $a0, $zero
	sw $5, $0
	sw $a1, $zero
	sw $6, $0
	sw $a2, $zero
	sw $7, $0
	sw $a3, $zero
	sw $8, $0
	sw $t0, $zero
	sw $9, $0
	sw $t1, $zero
	sw $10, $0
	sw $t2, $zero
	sw $11, $0
	sw $t3, $zero
	sw $12, $0
	sw $t4, $zero
	sw $13, $0
	sw $t5, $zero
	sw $14, $0
	sw $t6, $zero
	sw $15, $0
	sw $t7, $zero
	sw $16, $0
	sw $s0, $zero
	sw $17, $0
	sw $s1, $zero
	sw $18, $0
	sw $s2, $zero
	sw $19, $0
	sw $s3, $zero
	sw $20, $0
	sw $s4, $zero
	sw $21, $0
	sw $s5, $zero
	sw $22, $0
	sw $s6, $zero
	sw $23, $0
	sw $s7, $zero
	sw $24, $0
	sw $t8, $zero
	sw $25, $0
	sw $t9, $zero
	sw $26, $0
	sw $k0, $zero
	sw $27, $0
	sw $k1, $zero
	sw $28, $0
	sw $gp, $zero
	sw $29, $0
	sw $sp, $zero
	sw $30, $0
	sw $fp, $zero
	sw $31, $0
	sw $ra, $zero

end:
	j end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_unix_8", "[parser]") {

	{
		std::string input = R"(	# a test of address modes
	.data
str:	.ascii	"hello"

	.text
main:
	lb $t8, str
	la $t0, str
	lb $t1, ($t0)
	lb $t2, 1($t0)
	lb $t3, 2($t0)
	lb $t4, 3($t0)
	lb $t5, 4($t0)
	
end:	nop
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_0", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word 1024             # int var1 = 1024

var2:	.half 12               # short var2 = 12
	
var3:	.byte 0                # char var3 = 0

var4:	.byte 1, 2, 3, 4, 5, 6, 7, 8  # var4 = {1,2,3,4,5,6,7,8}

var5:	.space 512             # reserve 512 bytes

var6:	.ascii "hello"

var7:	.asciiz "goodbye"

	.text
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_1", "[parser]") {

	{
		std::string input = R"(	# a test for constants
	.data
	LENGTH = 1024
arr:	.space LENGTH
	
	.text
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_2", "[parser]") {

	{
		std::string input = R"(	# a test for basic data movement
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, 45
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_3", "[parser]") {

	{
		std::string input = R"(	# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0, 2
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_4", "[parser]") {

	{
		std::string input = R"(	# test of basic logical instructions
	.data
	TRUE = 1
	FALSE = 0

test1:	.word TRUE
test2:	.word FALSE
	
	.text
main:
	lw	$t0, test1
	lw	$t1, test2
	
	and	$t2, $t0, $t1
	and	$t2, $t0, TRUE
	nor	$t2, $t0, $t1
	nor	$t2, $t0, TRUE
	not	$t2, $t0
	not	$t2, $t0
	or	$t2, $t0, $t1
	or	$t2, $t0, TRUE
	xor	$t2, $t0, $t1
	xor	$t2, $t0, TRUE
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_5", "[parser]") {

	{
		std::string input = R"(	# test of the basic control instructions
	.data
x:	.word 1
y:	.word 2

	.text
main:
	lw $t0, x
	lw $t1, y
	
	beq $t0, $t0, next1
next1:

	bne $t0, $t1, next2
next2:

	blt $t0, $t1, next3
next3:

	ble $t0, $t0, next4
next4:

	bgt $t1, $t0, next5
next5:

	bge $t0, $t0, next6
next6:
	
end:
	j	end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_6", "[parser]") {

	{
		std::string input = R"(	# Example program to compute the sum of squares from Jorgensen [2016]

	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mul $t3, $t1, $t1
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_7", "[parser]") {

	{
		std::string input = R"(	# a test of all register names
	.data

	.text
main:
	sw $1, $0
	sw $at, $zero
	sw $2, $0
	sw $v0, $zero
	sw $3, $0
	sw $v1, $zero
	sw $4, $0
	sw $a0, $zero
	sw $5, $0
	sw $a1, $zero
	sw $6, $0
	sw $a2, $zero
	sw $7, $0
	sw $a3, $zero
	sw $8, $0
	sw $t0, $zero
	sw $9, $0
	sw $t1, $zero
	sw $10, $0
	sw $t2, $zero
	sw $11, $0
	sw $t3, $zero
	sw $12, $0
	sw $t4, $zero
	sw $13, $0
	sw $t5, $zero
	sw $14, $0
	sw $t6, $zero
	sw $15, $0
	sw $t7, $zero
	sw $16, $0
	sw $s0, $zero
	sw $17, $0
	sw $s1, $zero
	sw $18, $0
	sw $s2, $zero
	sw $19, $0
	sw $s3, $zero
	sw $20, $0
	sw $s4, $zero
	sw $21, $0
	sw $s5, $zero
	sw $22, $0
	sw $s6, $zero
	sw $23, $0
	sw $s7, $zero
	sw $24, $0
	sw $t8, $zero
	sw $25, $0
	sw $t9, $zero
	sw $26, $0
	sw $k0, $zero
	sw $27, $0
	sw $k1, $zero
	sw $28, $0
	sw $gp, $zero
	sw $29, $0
	sw $sp, $zero
	sw $30, $0
	sw $fp, $zero
	sw $31, $0
	sw $ra, $zero

end:
	j end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_pass_win_8", "[parser]") {

	{
		std::string input = R"(	# a test of address modes
	.data
str:	.ascii	"hello"

	.text
main:
	lb $t8, str
	la $t0, str
	lb $t1, ($t0)
	lb $t2, 1($t0)
	lb $t3, 2($t0)
	lb $t4, 3($t0)
	lb $t5, 4($t0)
	
end:	nop
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(!vm.status);
	}
}

TEST_CASE("test_fail_unix_0", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word 1024             # int var1 = 1024

var2:	.half 12               # short var2 = 12
	
var3:	.byte 0                # char var3 = 0

var4:	.byte 1, 2, 3, 4, 5, 6, 7,   # PARSE ERROR

var5:	.space 512             # reserve 512 bytes

var6:	.ascii "hello"

var7:	.asciiz "goodbye"

	.text
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_1", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word              # PARSE ERROR
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_2", "[parser]") {

	{
		std::string input = R"(	.data
var:	.word 0
	.word 1
	.word 2

	.text
	
	# lexer error line 10 
main:	la $t0, var 
	lw $s1, ($t0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_3", "[parser]") {

	{
		std::string input = R"(	# a test for basic data movement
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, $t1 # Parse Error
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_4", "[parser]") {

	{
		std::string input = R"(	# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0	# parse error
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_5", "[parser]") {

	{
		std::string input = R"(	# test of basic logical instructions
	.data
	TRUE = 1
	FALSE = 0

test1:	.word TRUE
test2:	.word FALSE
	
	.text
main:
	lw	$t0, test1
	lw	$t1, test2
	
	and	$t2, $t0, $t1
	and	$t2, $t0, TRUE
	nor	$t2, $t0, $t1
	nor	$t2, $t0, TRUE, FALSE # parse error
	not	$t2, $t0
	not	$t2, $t0
	or	$t2, $t0, $t1
	or	$t2, $t0, TRUE
	xor	$t2, $t0, $t1
	xor	$t2, $t0, TRUE
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_6", "[parser]") {

	{
		std::string input = R"(	# test of the basic control instructions
	.data
x:	.word 1
y:	.word 2

	.text
main:
	lw $t0, x
	lw $t1, y
	
	beq $t0, $t0, next1
next1:

	bne $t0, $t1, next2
next2:

	blt $t0, $t1, next3
next3:

	ble $t0, $t0, next4
next4:

	bgt $t1, next5      # parse error
next5:

	bge $t0, $t0, next6
next6:
	
end:
	j end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_unix_7", "[parser]") {

	{
		std::string input = R"(	# Example program to compute the sum of squares from Jorgensen [2016]

	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mul $t3, $t1, $t1
	add $t2, $t2, $32	# parse error
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_0", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word 1024             # int var1 = 1024

var2:	.half 12               # short var2 = 12
	
var3:	.byte 0                # char var3 = 0

var4:	.byte 1, 2, 3, 4, 5, 6, 7,   # PARSE ERROR

var5:	.space 512             # reserve 512 bytes

var6:	.ascii "hello"

var7:	.asciiz "goodbye"

	.text
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_1", "[parser]") {

	{
		std::string input = R"(	# A test file of data declarations only
	.data
var1:	.word              # PARSE ERROR
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_2", "[parser]") {

	{
		std::string input = R"(	.data
var:	.word 0
	.word 1
	.word 2

	.text
	
	# lexer error line 10 
main:	la $t0, var 
	lw $s1, ($t0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_3", "[parser]") {

	{
		std::string input = R"(	# a test for basic data movement
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, $t1 # Parse Error
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_4", "[parser]") {

	{
		std::string input = R"(	# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0	# parse error
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_5", "[parser]") {

	{
		std::string input = R"(	# test of basic logical instructions
	.data
	TRUE = 1
	FALSE = 0

test1:	.word TRUE
test2:	.word FALSE
	
	.text
main:
	lw	$t0, test1
	lw	$t1, test2
	
	and	$t2, $t0, $t1
	and	$t2, $t0, TRUE
	nor	$t2, $t0, $t1
	nor	$t2, $t0, TRUE, FALSE # parse error
	not	$t2, $t0
	not	$t2, $t0
	or	$t2, $t0, $t1
	or	$t2, $t0, TRUE
	xor	$t2, $t0, $t1
	xor	$t2, $t0, TRUE
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_6", "[parser]") {

	{
		std::string input = R"(	# test of the basic control instructions
	.data
x:	.word 1
y:	.word 2

	.text
main:
	lw $t0, x
	lw $t1, y
	
	beq $t0, $t0, next1
next1:

	bne $t0, $t1, next2
next2:

	blt $t0, $t1, next3
next3:

	ble $t0, $t0, next4
next4:

	bgt $t1, next5      # parse error
next5:

	bge $t0, $t0, next6
next6:
	
end:
	j end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}

TEST_CASE("test_fail_win_7", "[parser]") {

	{
		std::string input = R"(	# Example program to compute the sum of squares from Jorgensen [2016]

	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mul $t3, $t1, $t1
	add $t2, $t2, $32	# parse error
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		REQUIRE(vm.status == 2);
	}
}