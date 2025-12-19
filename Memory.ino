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
#include "U_ptr.hpp"
#include "S_ptr.hpp"

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

Memory::U_ptr<TestObject> u_test{ };

void setup() {
  // put your setup code here, to run once:

  #ifdef _TEST_U_PTR
  Serial.begin(9600);
  delay(2000);
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
  #endif
}

void loop() {
  
}
