use criterion::{criterion_group, criterion_main, Criterion};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
struct Node<T> {
    data: T,
    next: Option<Box<Node<T>>>,
}

#[derive(Debug)]
struct Stack<T> {
    top: Option<Box<Node<T>>>,
}

impl<T: std::fmt::Display + std::str::FromStr + Serialize + Deserialize> Stack<T> {
    // Ваш код остается неизменным

    fn bench_push(c: &mut Criterion) {
        let mut my_stack = Stack::new();

        c.bench_function("push", |b| b.iter(|| my_stack.push(42)));
    }

    fn bench_serialize_text(c: &mut Criterion) {
        let mut my_stack = Stack::new();
        my_stack.push(1);
        my_stack.push(2);
        my_stack.push(3);

        c.bench_function("serialize_text", |b| b.iter(|| my_stack.serialize_text()));
    }

    fn bench_deserialize_text(c: &mut Criterion) {
        let mut my_stack = Stack::new();
        let text_data = "3 2 1";

        c.bench_function("deserialize_text", |b| b.iter(|| my_stack.deserialize_text(text_data)));
    }

    fn bench_serialize_binary(c: &mut Criterion) {
        let mut my_stack = Stack::new();
        my_stack.push(1);
        my_stack.push(2);
        my_stack.push(3);

        c.bench_function("serialize_binary", |b| b.iter(|| my_stack.serialize_binary("binary_data.bin")));
    }

    fn bench_deserialize_binary(c: &mut Criterion) {
        let mut my_stack = Stack::new();
        my_stack.push(1);
        my_stack.push(2);
        my_stack.push(3);
        my_stack.serialize_binary("binary_data.bin");

        c.bench_function("deserialize_binary", |b| b.iter(|| my_stack.deserialize_binary("binary_data.bin")));
    }
}

criterion_group!(
    benches,
    Stack::<i32>::bench_push,
    Stack::<i32>::bench_serialize_text,
    Stack::<i32>::bench_deserialize_text,
    Stack::<i32>::bench_serialize_binary,
    Stack::<i32>::bench_deserialize_binary,
);

criterion_main!(benches);
