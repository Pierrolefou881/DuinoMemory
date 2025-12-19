/*
 * ----------------------------------------------------------------------------
 * SmartPointer
 * Base behavior for custom smart pointers.
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
#pragma once

namespace Memory
{
    /**
     * Base behavior for all smart pointers for Arduino. Smart pointers
     * manage memory deallocation automatically and greatly reduce risks
     * of memory leaks.
     * @param T can be any type.
     */
    template<typename T>
    class SmartPointer
    {
    public:
        // Don't manage deallocation here, derived types shall do it.
        virtual ~SmartPointer(void) = default;

        /**
         * @return _data member as a non mutable pointer.
         */
        T* get(void) const
        {
            return _data;
        }

        T& operator *(void) { return *_data; }
        T* operator ->(void) { return _data; }

        friend bool operator ==(const SmartPointer<T>& a, const SmartPointer<T>& b)
        {
            return a.get() == b.get();
        }

        friend bool operator !=(const SmartPointer<T>& a, const SmartPointer<T>& b)
        {
            return a.get() != b.get();
        }

        friend bool operator ==(const SmartPointer<T>& sp, const T* p)
        {
            return sp.get() == p;
        }

        friend bool operator ==(const T* p, const SmartPointer<T>& sp)
        {
            return sp == p;
        }

        friend bool operator !=(const SmartPointer<T>& sp, const T* p)
        {
            return sp.get() != p;
        }

        friend bool operator !=(T* p, const SmartPointer<T>& sp)
        {
            return sp != p;
        }

    protected:
        /**
         * Initializes this SmartPointer as the nullptr.
         */
        SmartPointer(void) = default;

        /**
         * Initializes this SmartPointer with the provided traditional
         * pointer.
         * @param data can be nullptr.
         */
        SmartPointer(T* data) : _data{ data }
        {
            // Empty body
        }

        /**
         * Changes the value of the _data member.
         * @param new_data to assign. Can be nullptr.
         */
        void set_data(T* new_data) { _data = new_data; }

    private:
        mutable T* _data{ };
    };
}