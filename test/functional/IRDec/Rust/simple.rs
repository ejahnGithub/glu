//
// RUN: rustc --crate-type=lib --emit=llvm-ir -g %s -o %t.ll
// RUN: gluc %t.ll -print-interface | FileCheck -v %s
//

#![no_main]

// CHECK: @linkage_name({{.*}}) public func get_pizza_count() -> UInt64;
pub fn get_pizza_count() -> u64
{
    42
}

// CHECK: @linkage_name({{.*}}) public func get_cc() -> [[STRUCT_NAME:.*]];
pub fn get_cc() -> &'static str
{
    "Rust"
}

// CHECK: struct [[STRUCT_NAME]] {
// CHECK-NEXT:    data_ptr: *UInt8,
// CHECK-NEXT:    length: UInt64,
// CHECK-NEXT: }
