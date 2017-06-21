//
// Created by Ivan Shynkarenka on 20.06.2017
//

#include "catch.hpp"

#include "intrusive/stack.h"

using namespace CppCommon;

struct MyStackNode : public CppCommon::Stack<MyStackNode>::Node
{
    int value;
    MyStackNode(int v) : value(v) {}
};

TEST_CASE("Intrusive stack", "[CppCommon][Intrusive]")
{
    CppCommon::Stack<MyStackNode> stack;
    REQUIRE(stack.empty());
    REQUIRE(stack.size() == 0);

    MyStackNode item1(1);
    stack.Push(item1);
    REQUIRE(stack.size() == 1);
    MyStackNode item2(2);
    stack.Push(item2);
    REQUIRE(stack.size() == 2);
    MyStackNode item3(3);
    stack.Push(item3);
    REQUIRE(stack.size() == 3);

    REQUIRE(!stack.empty());

    int sum = 0;
    for (auto& it : stack)
        sum += it.value;
    REQUIRE(sum == 6);

    MyStackNode* node = stack.Pop();
    REQUIRE(node != nullptr);
    REQUIRE(node->value == 3);
    REQUIRE(stack.size() == 2);

    node = stack.Pop();
    REQUIRE(node != nullptr);
    REQUIRE(node->value == 2);
    REQUIRE(stack.size() == 1);

    stack.Push(item2);
    REQUIRE(stack.size() == 2);
    stack.Push(item3);
    REQUIRE(stack.size() == 3);

    stack.Reverse();

    node = stack.Pop();
    REQUIRE(node != nullptr);
    REQUIRE(node->value == 1);
    REQUIRE(stack.size() == 2);

    node = stack.Pop();
    REQUIRE(node != nullptr);
    REQUIRE(node->value == 2);
    REQUIRE(stack.size() == 1);

    node = stack.Pop();
    REQUIRE(node != nullptr);
    REQUIRE(node->value == 3);
    REQUIRE(stack.size() == 0);

    REQUIRE(stack.empty());
}