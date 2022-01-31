#include "catch.hpp"

// put your parser tests here
#include "VMController.hpp"

#include <string>
#include <sstream>
#include <map>

string registers[32*2+3] = { "zero", "0", "at", "1", "v0", "2", "v1", "3", "a0", "4",
"a1", "5", "a2", "6", "a3", "7", "t0", "8", "t1", "9",
"t2", "10", "t3", "11", "t4", "12", "t5", "13", "t6", "14",
"t7", "15", "s0", "16", "s1", "17", "s2", "18", "s3", "19",
"s4", "20", "s5", "21", "s6", "22", "s7", "23", "t8", "24",
"t9", "25", "k0", "26", "k1", "27", "gp", "28", "sp", "29",
"fp", "30", "ra", "31", "pc", "hi", "lo" };


TEST_CASE("bad_file", "[MVCText]")
{
	{
		VMModel vm("badfile");
		REQUIRE(vm.status);
	}
}

TEST_CASE("test00", "[MVCText]") {

	{
		std::string input = R"(# simple infinite loop
	.data
	.text
main:	j main
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("foo").find("Error") == 0);
		REQUIRE(controller.parseInput("print $32").find("Error") == 0);
		REQUIRE(controller.parseInput("status") == "");
		for (size_t i = 0; i < registers->length(); i++)
		{
			REQUIRE(controller.parseInput("print $" + registers[i]) == "0x00000000");
		}
		REQUIRE(controller.parseInput("print 0x0000000f") == "0x00000000");
		REQUIRE(controller.parseInput("print $pc") == "0x00000000");
		REQUIRE(controller.parseInput("step") == "0x00000000");
		REQUIRE(controller.parseInput("status") == "");
		REQUIRE(controller.parseInput("print $pc") == "0x00000000");
		REQUIRE(controller.parseInput("quit") == "quit");
	}
}

TEST_CASE("test01", "[MVCText]") {

	{
		std::string input = R"(        .data
        .space 8
var1:   .word 1
var2:   .word -2

        .text
main:
     	la $t0, var1

	lw $t1, 0
	lw $t2, $t0
	lw $t3, 4($t0)
	lw $t4, 4(var1)
	lw $t5, var2

	lh $t1, 0
	lh $t2, $t0
	lh $t3, 4($t0)
	lh $t4, 4(var1)
	lh $t5, var2

	lb $t1, 0
	lb $t2, $t0
	lb $t3, 4($t0)
	lb $t4, 4(var1)
	lb $t5, var2
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("status") == "");
		REQUIRE(controller.parseInput("print $pc") == "0x00000000");
		REQUIRE(controller.parseInput("print 0x00000008") == "0x01");
		REQUIRE(controller.parseInput("print 0x00000009") == "0x00");
		REQUIRE(controller.parseInput("print 0x0000000a") == "0x00");
		REQUIRE(controller.parseInput("print 0x0000000b") == "0x00");
		REQUIRE(controller.parseInput("print 0x0000000c") == "0xfe");
		REQUIRE(controller.parseInput("print 0x0000000d") == "0xff");
		REQUIRE(controller.parseInput("print 0x0000000e") == "0xff");
		REQUIRE(controller.parseInput("print 0x0000000f") == "0xff");

		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("print $t0") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("print $t1") == "0x00000000");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("print $t2") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("print $t3") == "0xfffffffe");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("print $t4") == "0xfffffffe");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("print $t5") == "0xfffffffe");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
	}
}

TEST_CASE("test02", "[MVCText]") {

	{
		std::string input = R"(        .data
r1:     .space 4
r2:     .space 12
r3:     .space 4
var:    .word 7

        .text
main:
        la $t0, r2
     	lw $t1, var

	sw $t1, 0
	sw $t1, $t0
	sw $t1, 4($t0)
	sw $t1, 8(r2)
	sw $t1, r3

	sh $t1, 0
	sh $t1, $t0
	sh $t1, 4($t0)
	sh $t1, 8(r2)
	sh $t1, r3

	sb $t1, 0
	sb $t1, $t0
	sb $t1, 4($t0)
	sb $t1, 8(r2)
	sb $t1, r3
	sb $t1, r3

	mul $t1, $s2, $s3
	mul $t1, $s2, 5
	mulo $t1, $s2, $s3
	mulo $t1, $s2, 5
	mulou $t1, $s2, $s3
	mulou $t1, $s2, 5

	lw $s4, 8
	rem $s2, $s4, $s4
	remu $s2, $s4, $s4
	rem $s2, $s4, 4
	remu $s2, $s4, 4
	abs $s2, $s4
	neg $s2, $s4
	negu $s2, $s4
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("status") == "");
		REQUIRE(controller.parseInput("print $pc") == "0x00000000");

		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("print $t0") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("print $t1") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("print 0x00000000") == "0x07");
		REQUIRE(controller.parseInput("print 0x00000001") == "0x00");
		REQUIRE(controller.parseInput("print 0x00000002") == "0x00");
		REQUIRE(controller.parseInput("print 0x00000003") == "0x00");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("print 0x00000004") == "0x07");
		REQUIRE(controller.parseInput("print 0x00000005") == "0x00");
		REQUIRE(controller.parseInput("print 0x00000006") == "0x00");
		REQUIRE(controller.parseInput("print 0x00000007") == "0x00");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("print 0x00000008") == "0x07");
		REQUIRE(controller.parseInput("print 0x00000009") == "0x00");
		REQUIRE(controller.parseInput("print 0x0000000a") == "0x00");
		REQUIRE(controller.parseInput("print 0x0000000b") == "0x00");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");

		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
		REQUIRE(controller.parseInput("step") == "0x0000000b");
		REQUIRE(controller.parseInput("step") == "0x0000000c");
		REQUIRE(controller.parseInput("step") == "0x0000000d");
		REQUIRE(controller.parseInput("step") == "0x0000000e");
		REQUIRE(controller.parseInput("step") == "0x0000000f");
		REQUIRE(controller.parseInput("step") == "0x00000010");
		REQUIRE(controller.parseInput("step") == "0x00000011");
		REQUIRE(controller.parseInput("step") == "0x00000012");
		REQUIRE(controller.parseInput("step") == "0x00000013");
		REQUIRE(controller.parseInput("step") == "0x00000014");
		REQUIRE(controller.parseInput("step") == "0x00000015");
		REQUIRE(controller.parseInput("step") == "0x00000016");
		REQUIRE(controller.parseInput("step") == "0x00000017");
		REQUIRE(controller.parseInput("step") == "0x00000018");
		REQUIRE(controller.parseInput("step") == "0x00000019");
		REQUIRE(controller.parseInput("step") == "0x0000001a");
		REQUIRE(controller.parseInput("step") == "0x0000001b");
		REQUIRE(controller.parseInput("step") == "0x0000001c");
		REQUIRE(controller.parseInput("step") == "0x0000001d");
	}
}

TEST_CASE("test03", "[MVCText]") {

	{
		std::string input = R"(	          .data
	  A = 0
	  B = 1
	  C = 2
	  D = 4
var1: .word 1
	          .text
	  main:
	          li $t0, 100
	          li $t1, A
	          li $t2, B
	          li $t3, C
	          li $t4, D
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		controller.parseInput("print $t0");
		controller.parseInput("print &0x00000000");
		controller.parseInput("status");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
		controller.parseInput("step");
	}
}

TEST_CASE("test04", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = -1234

        .text
main:
        li $t0, VALUE
        move $t1, $t0
        move $t2, $t1
        move $t3, $t2
        move $t4, $t3
        move $t5, $t4
        move $t6, $t5
        move $t7, $t6
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
	}
}

TEST_CASE("test05", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = -1
var:    .word 1
        .text
main:
        lw $t0, var
        add $t1, $t0, VALUE
        add $t2, $t1, $t0
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
	}
}

TEST_CASE("test06", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 12
var:    .word 31
        .text
main:
        lw $t0, var
        addu $t1, $t0, VALUE # 31+12=43
        addu $t2, $t1, $t0 # 43+31=74
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
	}
}

TEST_CASE("test07", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        lw $t0, var1
        lw $t1, var2
        lw $t2, var3
        sub $t3, $t0, VALUE # 1-2 = -1
        sub $t4, $t1, $t0 # 12-1 = 11
        sub $t5, $t2, VALUE # -1-2 = -3
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
	}
}

TEST_CASE("test08", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        lw $t0, var1
        lw $t1, var2
        lw $t2, var3
        subu $t3, $t0, VALUE # 1-2 = -1 = 4294967295
        subu $t4, $t1, $t0 # 12-1 = 11
        subu $t5, $t2, VALUE # -1-2 = -3 = 4294967293
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
	}
}

TEST_CASE("test09", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        mult $t0, $t1 # -2*2 = -4 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        mult $t0, $t1 # 1073741824*-4 = -4294967296 (overflow)
        mflo $t8
        mfhi $t9

		mult $t0, $t1 # 2*4 = 8
		multu $t0, $t1 # 2*4 = 8
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
		REQUIRE(controller.parseInput("step") == "0x0000000b");
		REQUIRE(controller.parseInput("step") == "0x0000000c");
		REQUIRE(controller.parseInput("step") == "0x0000000d");
		REQUIRE(controller.parseInput("step") == "0x0000000e");
		REQUIRE(controller.parseInput("step") == "0x0000000f");
		REQUIRE(controller.parseInput("step") == "0x00000010");
		REQUIRE(controller.parseInput("step") == "0x00000011");
		REQUIRE(controller.parseInput("step") == "0x00000012");
		REQUIRE(controller.parseInput("step") == "0x00000013");
		REQUIRE(controller.parseInput("step") == "0x00000014");
		REQUIRE(controller.parseInput("step") == "0x00000015");
		REQUIRE(controller.parseInput("step") == "0x00000016");
	}
}

TEST_CASE("test10", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 4
var1:   .word 2
var2:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
	}
}

TEST_CASE("test11", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        divu $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        lw $s4, var1
        div $t0, $t1 # -2/2 = -1 rem 0 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        div $t0, $t1 # 1073741824/4 = 268435456 rem 0
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        div $t0, $t1 # 1073741824/-4 = -268435456 rem 0 
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0
        div $t0, $0, $s4
        div $t0, $0, 8
        divu $t0, $0, $s4
        divu $t0, $0, 8
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
		REQUIRE(controller.parseInput("step") == "0x0000000b");
		REQUIRE(controller.parseInput("step") == "0x0000000c");
		REQUIRE(controller.parseInput("step") == "0x0000000d");
		REQUIRE(controller.parseInput("step") == "0x0000000e");
		REQUIRE(controller.parseInput("step") == "0x0000000f");
		REQUIRE(controller.parseInput("step") == "0x00000010");
		REQUIRE(controller.parseInput("step") == "0x00000011");
		REQUIRE(controller.parseInput("step") == "0x00000012");
		REQUIRE(controller.parseInput("step") == "0x00000013");
		REQUIRE(controller.parseInput("step") == "0x00000014");
		REQUIRE(controller.parseInput("step") == "0x00000015");
		REQUIRE(controller.parseInput("step") == "0x00000016");
		REQUIRE(controller.parseInput("step") == "0x00000017");
		REQUIRE(controller.parseInput("step") == "0x00000018");
		REQUIRE(controller.parseInput("step") == "0x00000019");
		REQUIRE(controller.parseInput("step") == "0x0000001a");
	}
}

TEST_CASE("test12", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 4
var1:   .word 2
var2:   .word 1073741825 # = 2^30+1
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        div $t0, $t1 # 1073741825/4 = 268435456 rem 1
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
		REQUIRE(controller.parseInput("step") == "0x0000000b");
	}
}

TEST_CASE("test13", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        and $t2, $t0, $t1  
        and $t2, $t0, VALUE
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
	}
}

TEST_CASE("test14", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        nor $t2, $t0, $t1  
        nor $t2, $t0, VALUE
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
	}
}

TEST_CASE("test15", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        or $t2, $t0, $t1  
        or $t2, $t0, VALUE
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
	}
}

TEST_CASE("test16", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        xor $t2, $t0, $t1  
        xor $t2, $t0, VALUE
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
	}
}

TEST_CASE("test17", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        not $t2, $t0
        not $t2, $t1
        not $t2, VALUE
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
	}
}

TEST_CASE("test18", "[MVCText]") {

	{
		std::string input = R"(        .data
        .text
main:
        nop
        j next
        nop
next:
        nop
        j main
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000003");
	}
}

TEST_CASE("test19", "[MVCText]") {

	{
		std::string input = R"(        .data
var0:   .word 0
var1:   .word 1
var2:   .word 2
var3:   .word 3
        .text
main:
        lw $t0, var0
        lw $t1, var1
        lw $t2, var2
        lw $t3, var3

        beq $t0, $t1, check1
        beq $t0, $t0, check1
        nop
check1:
        bne $t0, $t0, check2
        bne $t0, $t1, check2
        nop
check2:
        bgt $t0, $t0, check3
        bgt $t3, $t1, check3
        nop
check3:
        bge $t0, $t1, check4
        bge $t3, $t2, check4
        nop
check4:
        blt $t3, $t1, check5
        blt $t1, $t3, check5
        nop
check5:
        ble $t3, $t1, check6
        ble $t3, $t3, check6
        nop
check6:
        nop
        j check6
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
	}
}

TEST_CASE("test20", "[MVCText]") {

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
	mult $t1, $t1
        mflo $t3
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		for (int i = 0; i < 54; i++)
		{
			controller.parseInput("step");
		}
	}
}

TEST_CASE("custom_test", "[MVCText]") {

	{
		std::string input = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
		lh $s1, var2
		lb $s1, var2
		move $s1, $t1
		mfhi $s1
		mthi $s1
		mflo $s1
		mtlo $s1
        not $t2, $t0
        not $t2, $t1
        not $t2, VALUE

		
)";
		std::istringstream iss(input);
		VMModel vm(iss);
		VMView gui;
		REQUIRE(!vm.status);
		VMController controller(vm);
		controller.setGUI(gui);
		REQUIRE(controller.parseInput("step") == "0x00000001");
		REQUIRE(controller.parseInput("step") == "0x00000002");
		REQUIRE(controller.parseInput("step") == "0x00000003");
		REQUIRE(controller.parseInput("step") == "0x00000004");
		REQUIRE(controller.parseInput("step") == "0x00000005");
		REQUIRE(controller.parseInput("step") == "0x00000006");
		REQUIRE(controller.parseInput("step") == "0x00000007");
		REQUIRE(controller.parseInput("step") == "0x00000008");
		REQUIRE(controller.parseInput("step") == "0x00000009");
		REQUIRE(controller.parseInput("step") == "0x0000000a");
		REQUIRE(controller.parseInput("step") == "0x0000000b");
	}
}