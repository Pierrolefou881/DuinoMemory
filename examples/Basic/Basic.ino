/*
 ******************************************************************************
 *  Basic.ino
 *
 *  Testbed for DuinoMemory smart pointers.
 *
 *  Author: Pierre DEBAS
 *  Copyright (c) 2026
 *
 *  MIT License
 *  https://github.com/Pierrolefou881/DuinoMemory
 *
 *  SPDX-License-Identifier: MIT
 *
 *  Description:
 *    Example and test sketch for the DuinoMemory library smart pointers
 *    U_ptr and S_ptr.
 *
 ******************************************************************************
 */
#include <DuinoMemory.hpp>

struct TestObject
{
  TestObject(void)
  {
    Serial.println("CREATED");
  }

  TestObject(const char* name) : _name{ name }
  {
    Serial.print("CREATED\t");
    Serial.println(name);
  }

  virtual ~TestObject(void)
  {
    Serial.print("DESTROYED\t");
    Serial.println(_name);   
  }
  void do_something(void)
  {
    Serial.print("CALLED\t");
    Serial.println(_name);
  }

  const char* _name = "";
};

struct DerivedTestObject : public TestObject
{
  DerivedTestObject(void) : TestObject{ } 
  {
    Serial.println("DERIVED");
  }
  DerivedTestObject(const char* name) : TestObject{ name }
  {
    Serial.print("DERIVED\t");
    Serial.println(name);
  }
  virtual ~DerivedTestObject(void) = default;
};

// CAUTION: Do not instantiate TestObject yet; do it after Serial.begin().
DuinoMemory::U_ptr<TestObject> u_test{ };
DuinoMemory::S_ptr<TestObject> s_test{ };

// Test make_unique and make_shared with derived types.
DuinoMemory::U_ptr<TestObject> u_derived{ };
DuinoMemory::U_ptr<TestObject> u_derived_param{ };
DuinoMemory::S_ptr<TestObject> s_derived{ };
DuinoMemory::S_ptr<TestObject> s_derived_param{ };

// Uncomment the macro for the type of pointer you want to test
// #define _TEST_U_PTR
#define _TEST_S_PTR

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);

  #ifdef _TEST_U_PTR
  Serial.println("==U_ptr TESTS==");
  DuinoMemory::U_ptr<TestObject> machin = DuinoMemory::make_unique<TestObject>();
  DuinoMemory::U_ptr<TestObject> bob = DuinoMemory::make_unique<TestObject>("Bob");
  u_test = DuinoMemory::make_unique<TestObject>();

  (*bob).do_something();
  bob->do_something();

  for (int i = 0; i < 5; i++)
  {
    Serial.println(!u_test);
    if (i > 2)
    {
      u_test = nullptr;
    }
  }

  // Change of ownership
  bob = machin.release();

  u_derived = DuinoMemory::make_unique<TestObject, DerivedTestObject>();
  u_derived_param = DuinoMemory::make_unique<TestObject, DerivedTestObject>("Chad");
  #endif

  #ifdef _TEST_S_PTR
  s_test = DuinoMemory::make_shared<TestObject>("Michael");
  Serial.println(s_test.count()); // 1

  s_test->do_something();
  (*s_test).do_something();

  s_test = s_test;
  Serial.println(s_test.count()); // 1

  DuinoMemory::S_ptr<TestObject> machin = DuinoMemory::make_shared<TestObject>();
  Serial.println(machin.count()); // 1
  s_test = nullptr;
  Serial.println(s_test.count()); // 0
  s_test = machin;
  Serial.println(s_test.count()); // 2
  machin = nullptr;
  Serial.println(s_test.count()); // 1
  Serial.println(machin.count()); // 0
  machin = DuinoMemory::make_shared<TestObject>("John");
  Serial.println(machin.count());

  // CAUTION: don't do this, as it may lead to undefined behavior
  // DuinoMemory::S_ptr<TestObject> ub = s_test.get();

  auto bob = DuinoMemory::make_shared<TestObject>("Bob");

  // We need a block to test deallocation when going out of scope.
  if (true)
  {
    DuinoMemory::S_ptr<TestObject> copies[5];
    for (auto i = 0; i < 5; i++)
    {
      copies[i] = s_test;
      Serial.println(s_test.count());
    }
  }

  Serial.println(s_test.count());
  s_test = nullptr;
  Serial.println(!s_test);
  Serial.println(s_test != nullptr);

  s_derived = DuinoMemory::make_shared<TestObject, DerivedTestObject>();
  s_derived_param = DuinoMemory::make_shared<TestObject, DerivedTestObject>("Zack");
  #endif
}

void loop() {
  
}
