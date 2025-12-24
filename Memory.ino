/*
 * ----------------------------------------------------------------------------
 * Memory
 * Testbed for smart pointers.
 * Part of the ArduinoLibraries project, to be used with any Arduino board.
 * <https://github.com/Pierrolefou881/ArduinoLibraries>
 * ----------------------------------------------------------------------------
 *
 * Copyright (C) 2025  Pierre DEBAS
 * <dpierre394@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "Memory.hpp"

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
    Serial.print("DERIVED");
    Serial.println(name);
  }
  virtual ~DerivedTestObject(void) = default;
};

Memory::U_ptr<TestObject> u_test{ };
Memory::S_ptr<TestObject> s_test{ };

// Test make_unique and make_shared with derived types.
Memory::U_ptr<TestObject> u_derived{ };
Memory::U_ptr<TestObject> u_derived_param{ };
Memory::S_ptr<TestObject> s_derived{ };
Memory::S_ptr<TestObject> s_derived_param{ };

// #define _TEST_U_PTR
#define _TEST_S_PTR

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);

  #ifdef _TEST_U_PTR
  Memory::U_ptr<TestObject> machin = Memory::make_unique<TestObject>();
  Memory::U_ptr<TestObject> bob = Memory::make_unique<TestObject>("Bob");
  u_test = Memory::make_unique<TestObject>();

  for (int i = 0; i < 5; i++)
  {
    Serial.println(u_test == nullptr);
    if (i > 2)
    {
      u_test = nullptr;
    }
  }

  bob = machin;

  u_derived = Memory::make_unique<TestObject, DerivedTestObject>();
  u_derived_param = Memory::make_unique<TestObject, DerivedTestObject>("Chad");
  #endif

  #ifdef _TEST_S_PTR
  s_test = Memory::make_shared<TestObject>("Michael");
  Memory::S_ptr<TestObject> machin = Memory::make_shared<TestObject>();
  auto bob = Memory::make_shared<TestObject>("Bob");

  if (true)
  {
    Memory::S_ptr<TestObject> copies[5];
    for (auto i = 0; i < 5; i++)
    {
      copies[i] = s_test;
      Serial.println(s_test.count());
    }
  }

  Serial.println(s_test.count());
  s_test = nullptr;
  Serial.println(s_test == nullptr);
  Serial.println(s_test != nullptr);

  s_derived = Memory::make_shared<TestObject, DerivedTestObject>();
  s_derived_param = Memory::make_shared<TestObject, DerivedTestObject>("Zack");
  #endif
}

void loop() {
  
}
