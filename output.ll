; ModuleID = 'compiler_output'
source_filename = "compiler_output"

declare void @print_int(i32)
declare void @print_float(double)
declare void @print_string(i8*)
declare void @print_bool(i1)
declare void @println_int(i32)
declare void @println_float(double)
declare void @println_string(i8*)
declare void @println_bool(i1)
declare i8* @input_string()
declare i32 @input_int()
declare double @input_float()
declare i8* @string_concat(i8*, i8*)

@.str.1 = private unnamed_addr constant [4 x i8] c"Doe\00"
@.str.0 = private unnamed_addr constant [5 x i8] c"John\00"

@.newline = private unnamed_addr constant [2 x i8] c"\0A\00"
define i32 @main() {
entry:
  %0 = alloca i32
  %1 = mul i32 3, 4
  %2 = add i32 2, %1
  store i32 %2, i32* %0
  %3 = alloca i32
  %4 = add i32 2, 3
  %5 = mul i32 %4, 4
  store i32 %5, i32* %3
  %6 = alloca i32
  %7 = sub i32 10, 5
  %8 = sub i32 %7, 2
  store i32 %8, i32* %6
  %9 = alloca i32
  %10 = mul i32 2, 3
  %11 = mul i32 4, 5
  %12 = add i32 %10, %11
  store i32 %12, i32* %9
  %13 = alloca i32
  store i32 5, i32* %13
  %14 = alloca i32
  store i32 0, i32* %14
  %15 = alloca i1
  store i1 0, i1* %15
  %16 = alloca i32
  store i32 0, i32* %16
  %17 = alloca i32
  store i32 0, i32* %17
  %18 = alloca i1
  br label %and.start2
and.start2:
  %19 = icmp sgt i32 5, 3
  br i1 %19, label %and.rhs3, label %and.end4
and.rhs3:
  %20 = icmp sgt i32 3, 1
  br label %and.end4
and.end4:
  %21 = phi i1 [ true, %and.start2 ], [ %20, %and.rhs3]
  store i1 %21, i1* %18
  %22 = alloca i1
  br label %or.start5
or.start5:
  %23 = icmp slt i32 10, 20
  br i1 %23, label %or.end7, label %or.rhs6
or.rhs6:
  %24 = icmp slt i32 30, 15
  br label %or.end7
or.end7:
  %25 = phi i1 [ false, %or.start5 ], [ %24, %or.rhs6]
  store i1 %25, i1* %22
  %26 = alloca i1
  store i1 0, i1* %26
  %27 = alloca double
  %28 = add i32 5, 3.14
  store double %28, double* %27
  %29 = alloca i32
  %30 = add i32 10, 5
  %31 = mul i32 %30, 2
  %32 = sub i32 %31, 3
  %33 = add i32 4, 1
  %34 = sdiv i32 %32, %33
  store i32 %34, i32* %29
  %35 = alloca i32
  store i32 0, i32* %35
  %36 = icmp sgt i32 10, 20
  br i1 %36, label %if.then8, label %if.else9
if.then8:
  store i32 10, i32* %35
  br label %if.merge10
if.else9:
  store i32 20, i32* %35
  br label %if.merge10
if.merge10:
  %37 = alloca i1
  store i1 25, i1* %37
  %38 = alloca i1
  store i1 1, i1* %38
  %39 = alloca i1
  br label %and.start11
and.start11:
  %40 = load i1, i1* %37
  %41 = icmp sge i32 %40, 18
  br i1 %41, label %and.rhs12, label %and.end13
and.rhs12:
  %42 = load i1, i1* %38
  br label %and.end13
and.end13:
  %43 = phi i1 [ true, %and.start11 ], [ %42, %and.rhs12]
  store i1 %43, i1* %39
  %44 = alloca i8*
  %45 = getelementptr inbounds [5 x i8], [5 x i8]* @.str.0, i64 0, i64 0
  store i8* %45, i8** %44
  %46 = alloca i8*
  %47 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.1, i64 0, i64 0
  store i8* %47, i8** %46
  %48 = alloca i8*
  %49 = load i8*, i8** %44
  %50 = add i32 %49, null
  %51 = load i8*, i8** %46
  %52 = add i32 %50, %51
  store i8* %52, i8** %48
  %53 = alloca i32
  %54 = and i32 12, 10
  store i32 %54, i32* %53
  %55 = alloca i32
  %56 = or i32 12, 10
  store i32 %56, i32* %55
  %57 = alloca i32
  %58 = xor i32 12, 10
  store i32 %58, i32* %57
  %59 = alloca i32
  store i32 0, i32* %59
  %60 = alloca i32
  %61 = shl i32 3, 2
  store i32 %61, i32* %60
  %62 = alloca i32
  %63 = ashr i32 12, 2
  store i32 %63, i32* %62
  ret i32 0 ; TODO: fix type
}

define i32 @add(i32 %a, i32 %b) {
entry:
  %0 = alloca i32
  store i32 %a, i32* %0
  %1 = alloca i32
  store i32 %b, i32* %1
  %2 = load i32, i32* %0
  %3 = load i32, i32* %1
  %4 = add i32 %2, %3
  ret i32 %4 ; TODO: fix type
}

